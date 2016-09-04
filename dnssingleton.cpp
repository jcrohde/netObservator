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

#include <QFile>
#include <ctime>
#include <mutex>
#include <functional>
#include "dnssingleton.h"

std::mutex myMutex;
static const int MAXPACKETNUMBER = 100;

DNSsingleton &DNSsingleton::getInstance() {
    static DNSsingleton instance;
    return instance;
}

DNSsingleton::DNSsingleton() {
    loadDNS();
    std::function<void(std::vector<addressItem>&,int&)> upd = [&] (std::vector<addressItem> &items, int &max) {update(items,max);};
    thread.start(upd);
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
    myMutex.lock();

    std::unordered_set<addressItem>::iterator ptr = std::find(addresses.begin(),addresses.end(),item);
    if (ptr == addresses.end()) {
        item.hostname = item.address.toQString();
        unknownHostNames.push_back(item);
    }
    else {
        item.hostname = ptr->hostname;       
    }

    myMutex.unlock();
}

void DNSsingleton::update(std::vector<addressItem> &items, int &maxNumber) {
    myMutex.lock();

    int size = items.size();
    for (int i = 0; i < size; i++)
        addresses.insert(items[i]);

    items.clear();

    moveElements(unknownHostNames,items);
    moveElements(oldHosts,items);

    maxNumber = MAXPACKETNUMBER;

    myMutex.unlock();
}

void DNSsingleton::moveElements(std::vector<addressItem> &source,std::vector<addressItem> &target) {
    int bound =std::min((int)source.size(), (int) (MAXPACKETNUMBER-target.size()));

    for (int i = 0; i < bound; i++)
        target.push_back(source[i]);

    source.erase(source.begin(),source.begin()+bound);
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
                    if (entries.size() > 2) {
                        if (entries[2].toInt() > currentTime)
                            addresses.insert(addressItem(ipAddress(entries[0]),entries[1],entries[2].toInt()));
                        else {
                            addressItem item(entries[0],entries[1],0);
                            addresses.insert(item);
                            oldHosts.push_back(item);
                        }
                    }
                }
            }
        }
    }
}

DNSsingleton::~DNSsingleton() {
    thread.stop();
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
        if (iter->timeStamp > 0) {
            writer->writeStartElement(DOMAINNAME);
            writer->writeTextElement("IP",iter->address.toQString());
            writer->writeTextElement("Name",iter->hostname);
            writer->writeTextElement("ExpireTime",QString::number(iter->timeStamp));
            writer->writeEndElement();
        }
    }

    writer->writeEndElement();
}

void DNSsingleton::storeDNS(QString &output) {
    QFile file("DNS.xml");

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(output.toUtf8());
    }
}
