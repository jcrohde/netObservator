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

#include "dnssingleton.h"
#include <ws2tcpip.h>
#include <QDebug>
#include <QFile>
#include <ctime>

DNSsingleton &DNSsingleton::getInstance() {
    static DNSsingleton instance;
    return instance;
}

DNSsingleton::DNSsingleton() {
    loadDNS();
    count = 0;
}

void DNSsingleton::loadDNS() {
    QString filename = "DNS.xml";

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());
        readDNS(content);
    }
}

void DNSsingleton::handle(addressItem &item) {
    std::unordered_set<addressItem>::iterator ptr = std::find(addresses.begin(),addresses.end(),item);
    if (ptr == addresses.end()) {
        item.hostname = getHostnameFromIp(item.address);
        addressItem ai = item;
        ai.timeStamp = item.timeStamp + ((count%7)+1)*SECONDSPERDAY;
        addresses.insert(ai);
        count++;
    }
    else {
        item.hostname = ptr->hostname;       
    }
}

QString DNSsingleton::getHostnameFromIp(ipAddress &saddr)
{
    char *hostAddress;
    struct addrinfo *info = 0;
    char host[512];
    bool valid = true;

    saddr.print(hostAddress);
    valid = valid && getaddrinfo(hostAddress, 0, 0, &info) == 0;

    if (valid) {
        valid = getnameinfo(info->ai_addr, info->ai_addrlen, host, 512, 0, 0, 0) == 0;
        freeaddrinfo(info);
    }

    delete []hostAddress;

    if (valid)
        return QString(host);
    else
        return "";
}



void DNSsingleton::readDNS(QString content) {
    reader.reset(new QXmlStreamReader(content));

    int currentTime = static_cast<unsigned int>(std::time(nullptr));
    if (reader->readNextStartElement()) {
        if (reader->name() == DNS){
            while (reader->readNextStartElement()) {
                if (reader->name() == DOMAINNAME) {
                    QStringList entries;
                    while (reader->readNextStartElement()) {
                        entries.push_back(reader->readElementText());
                    }
                    if (entries.size() > 2 && entries[2].toInt() > currentTime)
                        addresses.insert(addressItem(ipAddress(entries[0]),entries[1],entries[2].toInt()));
                }
            }
        }
    }
}

DNSsingleton::~DNSsingleton() {
    saveDNS();
}

void DNSsingleton::saveDNS() {
    QString output;

    writeDNS(output);
    storeDNS(output);
}

void DNSsingleton::writeDNS(QString &output) {
    writer.reset(new QXmlStreamWriter(&output));

    writer->writeStartDocument();
    writer->writeStartElement(DNS);

    for (std::unordered_set<addressItem>::iterator iter = addresses.begin(); iter != addresses.end(); ++iter) {
        writer->writeStartElement(DOMAINNAME);
        writer->writeTextElement("IP",iter->address.toQString());
        writer->writeTextElement("Name",iter->hostname);
        writer->writeTextElement("ExpireTime",QString::number(iter->timeStamp));
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

void DNSsingleton::storeDNS(QString &output) {
    QFile file("DNS.xml");

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(output.toUtf8());
    }
}
