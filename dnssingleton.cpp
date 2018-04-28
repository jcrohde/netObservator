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

#include <QFile>
#include <ctime>
#include <mutex>
#include <functional>
#include "dnssingleton.h"
#include "appdata.h"

std::mutex myMutex;
static const int MAXPACKETNUMBER = 100;

DNSsingleton &DNSsingleton::getInstance() {
    static DNSsingleton instance;
    return instance;
}

DNSsingleton::DNSsingleton() {
    getDNS(addresses, oldHosts);
    std::function<void(std::vector<addressItem>&,int&)> upd = [&] (std::vector<addressItem> &items, int &max) {update(items,max);};
    thread.start(upd);
    count = 0;
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
        if (!items[i].hostname.isEmpty()) {
            addresses.insert(items[i]);
        }

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

DNSsingleton::~DNSsingleton() {
    thread.stop();
    saveDNS();
}

void DNSsingleton::saveDNS() {
    writeDNS(addresses);
}
