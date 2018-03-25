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
#include <set>
#include "devices.h"

#include "packetparser.h"
#include "observers.h"

class SniffThread
{
public:
    Devices devs;

    SniffThread() {bRun = false;}
    ~SniffThread();

    void registerObserver(SniffObserver *observer) {observers.push_back(observer);}
    void unregisterObserver(SniffObserver *observer);

    void start(int deviceNumber);
    bool stop();

    PacketParser *parser;

    void setFilter(QString filter) {filterString = filter;}

    void update(const sniffSettings &settings) {
        this->settings = settings;
    }

    bool isSniffing() {return bRun;}

private:
    sniffSettings settings;
    QString currentStoreFolderName;

    pcap_dumper_t *dumpfile;

    std::vector<SniffObserver*> observers;

    QString filterString;
    QStandardItemModel *display;

    QString errorStr;

    char errorBuffer[PCAP_ERRBUF_SIZE];

    int colorCode;
    int devId;
    volatile bool bRun;
    bool valid, thirdParty;
    std::thread thread;

    std::vector<addressItem> addresses;
    std::vector<ipAddress> localAddresses;

    long int currentTimeInSeconds, lastTime;

    traffic data, bytes;
    int counter, sliceNumber, packetNumber;

    void notifyObservers();

    void run();
    void init();
    void startMainLoop();
    bool setFilter(pcap_t*& handle);
    void executeMainLoop(pcap_t*& handle,DNSsingleton &cache);

    void handlePacket(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);


    void storePacketInfo(const struct pcap_pkthdr *header,const u_char *packetData);

    void generateSlice(pcap_t*& handle);
};

#endif // SNIFFTHREAD_H
