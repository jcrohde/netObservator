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

#include "dnsthread.h"
#include "time.h"
#include <ws2tcpip.h>

static const unsigned int SECONDSPERDAY = 86400;

DNSThread::DNSThread()
{

}

void DNSThread::start(std::function<void(std::vector<addressItem>&,int&)> updFunc) {
    if (!thread.joinable()) {
        update = updFunc;
        bRun = true;
        thread = std::thread(&DNSThread::run, this);
    }
}

void DNSThread::stop() {
    if (thread.joinable()) {
        bRun = false;
        stopCondition.notify_all();
        thread.join();
    }
}

void DNSThread::run() {
    int maxPacketNumber = 0;

    count = 0;

    while (bRun) {
        time(&atLoopBegin);

        getHostNames();

        if (bRun && addresses.size() < maxPacketNumber) {
            waitForLoopElapse();
        }

        update(addresses,maxPacketNumber);
    }
}

void DNSThread::getHostNames() {
    int size = addresses.size();
    for (int i = 0; i < size; i++) {
        if (bRun) {
            addresses[i].hostname = getHostnameFromIp(addresses[i].address);
            addresses[i].timeStamp += ((count%7)+1)*SECONDSPERDAY;
            count++;
        }
        else {
            addresses.erase(addresses.begin()+i,addresses.end());
            break;
        }
    }
}

QString DNSThread::getHostnameFromIp(ipAddress &saddr)
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

void DNSThread::waitForLoopElapse() {
    int remainingSecondsTillUpdate;
    const int loopPeriod = 10;

    time(&afterGettingNames);

    remainingSecondsTillUpdate = loopPeriod - ((int) difftime(afterGettingNames,atLoopBegin));
    if (bRun && remainingSecondsTillUpdate > 0) {
        std::unique_lock<std::mutex> lock(stopMutex);
        stopCondition.wait_for(lock, std::chrono::seconds(remainingSecondsTillUpdate));
    }
}
