/*
Copyright (C) 2015-2018 Jan Christian Rohde

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

#ifndef DNSSINGLETON_H
#define DNSSINGLETON_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <memory>
#include <unordered_set>
#include "util.h"
#include "dnsthread.h"


const QString DNS = "dns";
const QString DOMAINNAME = "domainName";

class DNSsingleton
{
public:
    static DNSsingleton &getInstance();

    void handle(addressItem &item);

private:
    std::unique_ptr<QXmlStreamReader> reader;
    std::unique_ptr<QXmlStreamWriter> writer;

    std::unordered_set<addressItem> addresses;
    std::vector<addressItem> unknownHostNames;
    std::vector<addressItem> oldHosts;

    DNSThread thread;

    unsigned int count;

    void update(std::vector<addressItem> &items, int &maxNumber);
    void moveElements(std::vector<addressItem> &source,std::vector<addressItem> &target);

    DNSsingleton();
    void loadDNS();
    void readDNS(QString content);

    ~DNSsingleton();
    void saveDNS();
    void writeDNS(QString &output);
    void storeDNS(QString &output);
};

#endif // DNSSINGLETON_H
