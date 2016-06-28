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

#ifndef SNIFFTHREAD_H
#define SNIFFTHREAD_H

#include <thread>
#include <QStandardItemModel>
#include "devices.h"
#include "dnssingleton.h"

class SniffThread : public SettingObserver
{
public:
    SniffThread(QStandardItemModel *model, const Devices &devs)
        : display(model), devRef(devs) {}

    void start(int deviceNumber);
    void stop(QString &content);

private:

    QString xmlContent;
    QStandardItemModel *display;
    const Devices &devRef;

    char errorBuffer[PCAP_ERRBUF_SIZE];

    int devId;
    volatile bool bRun;
    std::thread thread;

    std::vector<addressItem> addresses;
    std::vector<ipAddress> localAddresses;

    ipAddress currentHome;
    u_short currentPort;
    long int currentTimeInSeconds;

    int row, column;
    bool foundNew, incoming;

    QString content[COLUMNNUMBER];

    void run();
    void executeMainLoop(DNSsingleton &cache);

    void handlePacket(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);

    void getPacketInfo(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);
    void getTimeString();
    bool updateConnectedAddresses(ipAddress &addr, DNSsingleton &cache);
    bool getConnectedAddressInfo(ipHeader *ih,udpHeader *uh, DNSsingleton &cache);
    void readPacket(const struct pcap_pkthdr *header,const u_char *packetData);

    void presentPacketInfo();
    void killOldEntries();
    void storePacketInfo();
    void showPacketInfo();
    void showAttribute(int i, QString attribute);
};

#endif // SNIFFTHREAD_H
