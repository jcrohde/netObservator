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

#ifndef DNSTHREAD_H
#define DNSTHREAD_H

#include <condition_variable>
#include <thread>
#include <chrono>
#include <functional>
#include "util.h"

class DNSThread
{
public:
    DNSThread();

    void start(std::function<void(std::vector<addressItem>&,int&)> updFunc);
    void stop();

private:
    std::function<void(std::vector<addressItem>&,int&)> update;
    std::vector<addressItem> addresses;

    std::condition_variable stopCondition;
    std::mutex stopMutex;

    volatile bool bRun;
    std::thread thread;

    time_t atLoopBegin, afterGettingNames;

    int count;

    void run();
    void getHostNames();
    QString getHostnameFromIp(ipAddress &addr);
    void waitForLoopElapse();
};

#endif // DNSTHREAD_H
