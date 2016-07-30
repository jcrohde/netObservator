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

#include <functional>
#include <thread>
#include <set>
#include <QStandardItemModel>
#include "devices.h"
#include "dnssingleton.h"
#include "packetinfopresenter.h"

class SniffThread : public SettingObserver
{
public:
    SniffThread(const Devices &devs)
        : devRef(devs) {}

    void start(int deviceNumber, const QString &filter, std::function<void(QString)> errorfunc);
    bool stop(QString &name, int &number, std::set<ipAddress> &threadAdresses);

    void setPacketInfoPresenter(TablePacketInfoPresenter *presenter) {packetInfo = presenter;}

private:
    QString xmlContent;
    QString filterString;
    QStandardItemModel *display;
    const Devices &devRef;

    std::function<void(QString)> errorHandle;

    QString errorStr;

    TablePacketInfoPresenter *packetInfo;

    char errorBuffer[PCAP_ERRBUF_SIZE];

    int colorCode;
    int devId;
    volatile bool bRun;
    bool valid;
    std::thread thread;

    std::vector<addressItem> addresses;
    std::vector<ipAddress> localAddresses;

    std::set<ipAddress> seenAdresses;

    ipAddress currentHome;
    u_short hostPort, localPort;
    long int currentTimeInSeconds;

    int counter, sliceNumber;
    bool foundNew, incoming;

    QString content[COLUMNNUMBER];

    void run();
    void init();
    void startMainLoop();
    bool setFilter(pcap_t*& handle);
    void executeMainLoop(pcap_t*& handle,DNSsingleton &cache);

    void handlePacket(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);

    void getPacketInfo(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);
    void getTimeString();
    bool updateConnectedAddresses(ipAddress &addr, DNSsingleton &cache);
    bool getConnectedAddressInfo(ipHeader *ih,udpHeader *uh, DNSsingleton &cache);
    void getProtocol(ipHeader *ih);
    void readPacket(const struct pcap_pkthdr *header,const u_char *packetData);

    void presentPacketInfo();
    void killOldEntries();
    void storePacketInfo();

    void generateSlice();
};

#endif // SNIFFTHREAD_H
