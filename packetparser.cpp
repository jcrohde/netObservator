/*
Copyright (C) 2015-2016 Jan Christian Rohde

This file is part of netObservator.

netObservator is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

netObservator is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with netObservator; if not, see http://www.gnu.org/licenses.
*/

#include "packetparser.h"
#include <ws2tcpip.h>
#include "sniffthread.h"
#include <QRegExp>
#include <QDir>
#include "dnssingleton.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

parseInstruction::parseInstruction() {
    mode = Mode::ALL;
    statisticsColumn = (column) 1;
    timeStampNeeded = false;
    plotByteNumber = false;
    destination = "";
    folder = false;
}

PacketParser::PacketParser() : duration(20) {
    parseInstruction instruction;
    configure(instruction);
}

void PacketParser::extract(const QString &filename, std::set<ipAddress> &addr) {
    addr.clear();
    parseInstruction instruction;
    instruction.mode = Mode::ADDRESSES;
    parse(filename, instruction);

    addr = this->addr;
}

void PacketParser::search(const SearchCommand &command, const QString &filename) {
    if (command.inFiles) {
        setSearchCommand(command);
        executeParseloop(command.filenames);
    }
    else {
        this->command = command;

        parseInstruction instruction;
        instruction.mode = Mode::SEARCH;
        instruction.settings = command.settings;
        parse(filename,instruction);
    }
}

void PacketParser::parse(QString contentFile, const parseInstruction &instruction) {
    QStringList content;
    content.append(contentFile);
    parse(content, instruction);
}

void PacketParser::parse(QStringList contentFile, const parseInstruction &instruction) {
    this->configure(instruction);
    if (contentFile.size() > 0 && contentFile.begin()->size() > 0) {
        executeParseloop(contentFile);
    }
}

const std::map<QString,std::vector<long int> > &PacketParser::getAppearance() {
    for (auto iter = appearance.begin(); iter != appearance.end(); ++ iter) {
         std::sort(iter->second.begin(),iter->second.end());
    }
    return appearance;
}

void PacketParser::executeParseloop(const QStringList &content) {
    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];

    for (int i = 0; i < content.size(); i++) {
        fp = pcap_open_offline(content.at(i).toLatin1().data(),errbuf);

        if (fp != NULL) {
            QString filename;
            filename = destination;
            if (folder) {
               if (!QDir(filename).exists())
                   QDir().mkdir(filename);
               QString smallName = content.at(i).mid(content.at(i).lastIndexOf("/")+1);
               filename.append("/" + smallName.left(smallName.size()-5));
               if (mode == Mode::TOXML) filename.append(".xml");
               else if (mode == Mode::TOJSON) filename.append(".json");
            }
            QFile file(filename);

            if (mode == Mode::SEARCH || mode == Mode::COPY) {
                QString str = getDumpFile(mode, content.at(i));
                dumpfile = pcap_dump_open(fp,str.toLatin1().data());
            }
            else if (mode == Mode::TOXML) {
                file.open(QIODevice::WriteOnly);

                xmlWriter.reset(new QXmlStreamWriter(&file));
                xmlWriter->setAutoFormatting(true);
                xmlWriter->writeStartDocument();
            }
            else if (mode == Mode::TOJSON) {
                jsonArray.reset(new QJsonArray);
            }

            runLoop(fp);

            if (mode == Mode::SEARCH || mode == Mode::COPY)
                pcap_dump_close(dumpfile);
            else if (mode == Mode::TOXML) {
                file.close();
            }
            else if (mode == Mode::TOJSON) {
                QFile file(filename);
                file.open(QIODevice::WriteOnly);
                QJsonObject root;
                root["Packets"] = *jsonArray;
                QTextStream stream(&file);
                stream << QJsonDocument(root).toJson(QJsonDocument::Indented);
                file.close();
            }

            pcap_close(fp);
        }
    }

    firePlot();
}

void PacketParser::parse(const pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache) {
    getPacketInfo(header,packetData,cache);
    if (mode == Mode::TOXML)
        writeXML();
    else if (mode == Mode::TOJSON) {
        writeJson();
    }
    else if (mode != Mode::STATISTICS && mode != Mode::ADDRESSES)
        presentPacketInfo();
}

void PacketParser::getPacketInfo(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache) {
    ipHeader *ih = (ipHeader *) (packetData + 14);
    u_int ipLength = (ih->versionAndInternetHeaderLength & 0xf) * 4;
    udpHeader *uh = (udpHeader *) ((u_char*)ih + ipLength);

    colorCode = evaluate(header,packetData,ih,uh);
}

QString PacketParser::getDumpFile(Mode mode, const QString &contentFile) {
    QString str;
    if (mode == Mode::SEARCH) {
        str = contentFile.left(contentFile.size() - 5) + "Search.pcap";

        QDir dir(str.left(str.lastIndexOf("/")) + "/Search");
        if (!dir.exists())
            dir.mkpath(".");

        str = str.left(str.lastIndexOf("/")) + "/Search/" + str.right(str.size()-str.lastIndexOf("/")-1);
    }
    else
        str = destination;

    return str;
}

void PacketParser::runLoop(pcap_t *fp) {
    struct pcap_pkthdr *header;
    const u_char *packetData;
    int result;

    do {
        result = pcap_next_ex(fp, &header, &packetData);
        if (result > 0)
            parse(header, packetData, DNSsingleton::getInstance());
    } while (result > 0);
}

void PacketParser::firePlot() {
    if (mode == Mode::FIRSTPACKET) {
        if (byteCounter.packetNumber >0)
            fireByteNumber(byteCounter);

        if (packetCounter.packetNumber > 0)
            firePacketNumber(packetCounter);
    }
    else {
        if (packetsPerSecond.size() >0) {
            firePacketNumber(packetCounter);
            for (auto position: packetsPerSecond) {
                packetCounter.packetNumber = position.second;
                firePacketNumber(packetCounter);
            }
        }

        if (bytesPerSecond.size() >0) {
            fireByteNumber(byteCounter);
            for (auto position: bytesPerSecond) {
                byteCounter.packetNumber = position.second;
                fireByteNumber(byteCounter);
            }
        }
    }
}

void PacketParser::writeXML() {
    xmlWriter->writeStartElement("Packet");

    int c = 0;
    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (showInfo[i]) {
            xmlWriter->writeTextElement(LABEL[i], content.get()[c] );
            c++;
        }
    }

    xmlWriter->writeEndElement();
}

void PacketParser::writeJson() {
    int c = 0;
    QJsonObject packet;

    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (showInfo[i]) {
            packet[LABEL[i]] = content.get()[c];
            c++;
        }
    }
    jsonArray->append(packet);
}

void PacketParser::presentPacketInfo() {
    if ((mode == Mode::FIRSTPACKET && foundNew) || mode == Mode::ALL) {
        if (dynamic_cast<TablePacketInfoPresenter*>(packetInfo) != nullptr)
            dynamic_cast<TablePacketInfoPresenter*>(packetInfo)->show(content.get(),colorCode);
    }
    else if (mode == Mode::FIRSTPACKET) {
        killOldEntries();
    }
}

void PacketParser::setSearchCommand(const SearchCommand &command) {
    this->command = command;

    parseInstruction instruction;
    instruction.mode = Mode::SEARCH;
    instruction.settings = command.settings;

    this->configure(instruction);
}

void PacketParser::killOldEntries() {
    myCellMutex.lock();
    int i = addresses.size();
    while (i > 0) {
        i--;
        if (currentTimeInSeconds-addresses[i].timeStamp>duration) {
            addresses.erase(addresses.begin() +i);
            packetInfo->display.tableModel->removeRow(i);
        }
    }
    myCellMutex.unlock();
}
