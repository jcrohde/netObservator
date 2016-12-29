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

#include <QRegExp>

#include "parsescheme.h"
#include "dnssingleton.h"

static bool findaddr(std::vector<ipAddress> &addrVec, ipAddress addr) {
    return std::find(addrVec.begin(),addrVec.end(),addr) != addrVec.end();
}

ParseScheme::ParseScheme()
{
    statisticsColumn = (column) 1;
    viewSettings settings;
    content.reset(new QString[settings.shownColumns]);

    element[TIME] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &timeString)
    {
        char time_string[20];

        if (currentTimeInSeconds > 0) {
            struct tm* ptm;
            ptm = localtime (&currentTimeInSeconds);

            strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
            timeString = time_string;
        }
        else timeString = "null";
    };

    element[VERSIONANDINTERNETHEADERLENGHT] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &versionAndLength)
    {
        versionAndLength = QString::number((unsigned int)ih->versionAndInternetHeaderLength / 16) + ", " + QString::number((unsigned int)ih->versionAndInternetHeaderLength % 16);
    };

    element[TYPEOFSERVICE] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &tos)
    {
        tos = QString::number((unsigned int)ih->typeOfService);
    };

    element[TOTALLENGTH] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &totalLength)
    {
        totalLength = QString::number(ntohs(ih->totalLength));
    };

    element[IDENTIFICATION] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &id)
    {
        id = QString::number(ntohs(ih->identification));
    };

    element[FLAGSANDFRAGMENTOFFSET] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &flagsAndOffset)
    {
        int i = ntohs(ih->flagsAndfragmentOffset);
        if (i / 0x4000)
            flagsAndOffset = "no fragments allowed";
        else {
            if (!(i / 0x2000))
                flagsAndOffset = "last fragment, ";
            else
                flagsAndOffset.clear();
            flagsAndOffset += "position " + QString::number(i % 0x1fff);
        }
    };

    element[TIMETOLIFE] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &ttl)
    {
        ttl = QString::number((unsigned int)ih->timeToLive);
    };

    element[PROTOCOL] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &protocol)
    {
        switch (ih->protocol) {
        case 6:
            protocol = "TCP";
            colorcode = 0;
            break;

        case 17:
            protocol = "UDP";
            colorcode = 0;
            break;

        case 1:
            protocol = "ICMP";
            colorcode = 3;
            break;

        case 2:
            protocol = "IGMP";
            colorcode = 3;
            break;

        default:
            protocol = QString::number((int)ih->protocol);
            colorcode = 6;

            break;
        }
    };

    element[HOSTPORT] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &port)
    {
        if (direction == Direction::INBOUND)
            port = QString::number(ntohs(uh->srcPort));
        else
            port = QString::number(ntohs(uh->destPort));
    };

    element[HOSTADDRESS] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &address)
    {
        if (direction == Direction::INBOUND)
            address = ih->srcAddr.toQString();
        else
            address = ih->destAddr.toQString();
    };

    element[HOSTNAME] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &hostName)
    {
        ipAddress ip;
        if (direction == Direction::INBOUND)
            ip = ih->srcAddr;
        else
            ip = ih->destAddr;

        addressItem address(ip,"name",currentTimeInSeconds);

        DNSsingleton::getInstance().handle(address);

        hostName = address.hostname;
    };

    element[DIRECTION] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &dir)
    {
        if (direction == Direction::INBOUND)
            dir = "inbound";
        else if (direction == Direction::OUTBOUND)
            dir = "outbound";
        else
            dir = "to Host from SRC";
    };

    element[LOCALSRCPORT] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &port)
    {
        if (direction == Direction::INBOUND)
            port = QString::number(ntohs(uh->destPort));
        else
            port = QString::number(ntohs(uh->srcPort));
    };

    element[LOCALSRCADDRESS] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &address)
    {
        if (direction == Direction::INBOUND)
            address = ih->destAddr.toQString();
        else
            address = ih->srcAddr.toQString();
    };

    element[OPTIONANDPADDING] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &optionAndPadding)
    {
        optionAndPadding = QString::number((unsigned long int)ih->optionAndPadding);
    };

    element[UDPLENGTH] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &udpLength)
    {
        udpLength = QString::number(ntohs(uh->length));
    };

    element[ENTIRE_PACKET] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &packet)
    {
        int k;

        for(unsigned int i=0; i<packetHeader->caplen; i++) {
            k = packetData[i];
            if (k<16) packet += "0";
            packet += QString::number(k,16) + " ";
        }
    };

    element[PAYLOAD] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &payload)
    {
        int k;

        for(unsigned int i=0; i<packetHeader->caplen; i++) {
            k = packetData[i];
            if (isprint(k)) payload += (short) k;
            else payload += ".";
        }
    };

    element[IPHEADERCHECKSUM] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &checksum)
    {
        checksum = QString::number(ntohs(ih->checksum));
    };

    element[UDPHEADERCHECKSUM] = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &checksum)
    {
        checksum = QString::number(ntohs(uh->checksum));
    };

    extractAddress = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString &checksum)
    {
        if (direction == Direction::INBOUND)
            addr.insert(ih->srcAddr);
        else
            addr.insert(ih->destAddr);
    };

    searchFunc = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, QString &checksum)
    {
        bool found = false;
        if (command.mode == command.CASEINSENSITIVE)
            found = found || checksum.contains(command.searchString,Qt::CaseInsensitive);
        else if (command.mode == command.REGEX)
            found = found || checksum.contains(QRegularExpression(command.searchString));
        else
            found = found || checksum.contains(command.searchString);

        if (command.invertMatch)
            found = !found;

        if (found)
            pcap_dump((unsigned char *)dumpfile, packetHeader, packetData);
    };

    dumpFunc = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, QString &checksum)
    {
        pcap_dump((unsigned char *)dumpfile, packetHeader, packetData);
    };

    insertStatistics = [&](const pcap_pkthdr *packetHeader, const u_char *packetData, QString &value) {
        if (appearance.find(value) != appearance.end()) {
            appearance.at(value).push_back(currentTimeInSeconds);
        }
        else {
            std::vector<long int> v;
            v.push_back(currentTimeInSeconds);
            appearance.insert(std::make_pair(value, v));
        }
    };

    determineColorCode = [&](ipHeader *ih) {
        switch (ih->protocol) {
        case 6:
        case 17:
            colorcode = 0;
            break;

        case 1:
        case 2:
            colorcode = 3;
            break;

        default:
            colorcode = 6;
            break;
        }
    };

    determineHostIp = [&](ipHeader *ih) {
        if (direction == Direction::INBOUND)
            hostIp = ih->srcAddr;
        else
            hostIp = ih->destAddr;
        seenAdresses.insert(hostIp);
    };

    determineDirection = [&](ipHeader *ih) {
        if (findaddr(localAddresses,ih->destAddr ))
            direction = Direction::INBOUND;
        else if (findaddr(localAddresses,ih->srcAddr))
            direction = Direction::OUTBOUND;
        else
            direction = Direction::THIRDPARTY;
    };

    determineNewAddress = [&](ipHeader *ih) {
        addressItem item(hostIp,"name",currentTimeInSeconds);
        auto position = std::find(addresses.begin(),addresses.end(),item);
        foundNew = position == addresses.end();
        if (foundNew) {
            DNSsingleton::getInstance().handle(item);
            addresses.push_back(item);
        }
        else
            position->timeStamp = currentTimeInSeconds;
    };

    determineTimestamp = [&](const pcap_pkthdr *packetHeader) {
        currentTimeInSeconds = packetHeader->ts.tv_sec;
    };

    plotPackets = [&](const pcap_pkthdr *packetHeader) {
        if (mode == Mode::FIRSTPACKET) {
            if (currentTimeInSeconds > lastCurrentTimeInSeconds) {
                firePacketNumber(packetCounter);
                packetCounter.packetNumber = 1;
            }
            else
                packetCounter.packetNumber++;
        }
        else {
            auto position = packetsPerSecond.find(currentTimeInSeconds);
            if (position == packetsPerSecond.end())
                packetsPerSecond.insert(std::make_pair(currentTimeInSeconds,1));
            else
                packetsPerSecond.at(currentTimeInSeconds)++;
        }
    };

    plotBytes = [&](const pcap_pkthdr *packetHeader) {
        if (mode == Mode::FIRSTPACKET) {
            if (currentTimeInSeconds > lastCurrentTimeInSeconds) {
                 fireByteNumber(byteCounter);
                 byteCounter.packetNumber = packetHeader->caplen;
            }
            else
                byteCounter.packetNumber += packetHeader->caplen;
        }
        else {
            auto position = bytesPerSecond.find(currentTimeInSeconds);
            if (position == bytesPerSecond.end())
                bytesPerSecond.insert(std::make_pair(currentTimeInSeconds,1));
            else
                bytesPerSecond.at(currentTimeInSeconds)+= packetHeader->caplen;
        }
    };

    updateTimestamp = [&](const pcap_pkthdr *packetHeader) {
        lastCurrentTimeInSeconds = currentTimeInSeconds;
    };
}

void ParseScheme::configure(const parseInstruction &instruction) {
    resetCounter();
    clearMethods();
    store(instruction);

    if (instruction.mode == Mode::STATISTICS)
        updateStatistics(instruction);
    else if (instruction.mode == Mode::COPY)
        finalMethod.push_back(dumpFunc);
    else if (instruction.mode == Mode::SEARCH) {
        determinations.push_back(determineDirection);
        int counter = 0;
        int hits = 0;
        for (int i = 0; i < COLUMNNUMBER; i++) {
            if (instruction.settings.showInfo[i])
                hits++;
            if (hits > command.column)
                break;
            counter++;
        }
        procedure.push_back(element[counter]);
        finalMethod.push_back(searchFunc);
    }
    else if (instruction.mode == Mode::ADDRESSES) {
        determinations.push_back(determineDirection);
        procedure.push_back(extractAddress);
    }
    else
        updateDefault(instruction);
}

void ParseScheme::resetCounter() {
    lastCurrentTimeInSeconds = 0;
    packetCounter.packetNumber = 0;
    byteCounter.packetNumber = 0;
    packetsPerSecond.clear();
    bytesPerSecond.clear();
}

void ParseScheme::clearMethods() {
    headerActions.clear();
    determinations.clear();
    procedure.clear();
    finalMethod.clear();
    appearance.clear();

    seenAdresses.clear();
}

void ParseScheme::store(const parseInstruction &instruction) {
    mode = instruction.mode;

    if (instruction.settings.shownColumns > 0)
        content.reset(new QString[instruction.settings.shownColumns]);

    destination = instruction.destination;
}

void ParseScheme::updateStatistics(const parseInstruction &instruction) {
    headerActions.push_back(determineTimestamp);
    determinations.push_back(determineDirection);
    procedure.push_back(element[instruction.statisticsColumn]);
    finalMethod.push_back(insertStatistics);
}

void ParseScheme::updateDefault(const parseInstruction &instruction) {
    if (instruction.timeStampNeeded
            || instruction.settings.showInfo[TIME]
            || instruction.mode == Mode::FIRSTPACKET
            || instruction.settings.showInfo[HOSTNAME]
            || instruction.plotByteNumber
            || instruction.plotPacketNumber)
        headerActions.push_back(determineTimestamp);

    if (instruction.plotPacketNumber)
        headerActions.push_back(plotPackets);

    if (instruction.plotByteNumber)
        headerActions.push_back(plotBytes);

    if (instruction.plotByteNumber || instruction.plotPacketNumber)
        headerActions.push_back(updateTimestamp);

    determinations.push_back(determineDirection);

    if (instruction.mode == Mode::FIRSTPACKET) {
        determinations.push_back(determineHostIp);
        determinations.push_back(determineNewAddress);
    }

    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (instruction.settings.showInfo[i])
            procedure.push_back(element[i]);
        else if (i == PROTOCOL)
            determinations.push_back(determineColorCode);
    }
}

TablePacketInfoPresenter::colorCode ParseScheme::evaluate(const pcap_pkthdr *packetHeader, const u_char *packetContent, ipHeader *ih, udpHeader *uh) {
    QString *info = content.get();

    for (int i = 0; i < headerActions.size(); i++)
        headerActions[i](packetHeader);

    for (int i = 0; i < determinations.size(); i++)
        determinations[i](ih);

    for (int i = 0; i < procedure.size(); i++) {
        info[i].clear();
        procedure[i](packetHeader, packetContent, ih, uh, info[i]);
    }

    for (int i = 0; i < finalMethod.size(); i++)
        finalMethod[i](packetHeader, packetContent, info[0]);

    return (TablePacketInfoPresenter::colorCode)((int)direction + colorcode);
}
