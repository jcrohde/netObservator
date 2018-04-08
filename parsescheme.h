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

#ifndef PARSESCHEME_H
#define PARSESCHEME_H

#include <functional>
#include "ippacket.h"
#include "settings.h"
#include "packetinfopresenter.h"

enum class Mode {ALL, FIRSTPACKET, STATISTICS, ADDRESSES, SEARCH, COPY, TOXML, TOJSON, CHART};

struct parseInstruction {
    parseInstruction();

    viewSettings settings;
    Mode mode;
    QString destination;
    column statisticsColumn;
    bool plotPacketNumber;
    bool plotByteNumber;
    bool timeStampNeeded;
    bool folder;
};

class ParseScheme
{
public:
    std::function<void(traffic&)> firePacketNumber;
    std::function<void(traffic&)> fireByteNumber;

    ParseScheme();

    void configure(const parseInstruction &instruction);

    const std::set<ipAddress> &getSeenAddresses() {return seenAdresses;}
    const QHash<QString, Protocol> &getChart() {return chart;}

protected:
    pcap_dumper_t *dumpfile;
    std::map<QString,std::vector<long int> > appearance;
    std::vector<ipAddress> localAddresses;
    std::unique_ptr<QString> content;
    TablePacketInfoPresenter::colorCode colorCode;
    SearchCommand command;
    bool foundNew;
    std::set<ipAddress> addr;
    bool showInfo[COLUMNNUMBER];

    Mode mode;
    bool folder;

    std::vector<addressItem> addresses;

    long int currentTimeInSeconds, lastCurrentTimeInSeconds;

    TablePacketInfoPresenter::colorCode evaluate(const pcap_pkthdr* packetHeader, const u_char* packetContent, ipHeader* ih, udpHeader* uh);

    traffic packetCounter, byteCounter;
    std::map<long int, int> packetsPerSecond, bytesPerSecond;

    QString destination;

private:

    std::set<ipAddress> seenAdresses;
    column statisticsColumn;

    Direction direction;
    int colorcode;
    QHash<QString, Protocol> chart;

    ipAddress hostIp;

    std::function<void(const pcap_pkthdr* packetHeader)> determineTimestamp;
    std::function<void(const pcap_pkthdr* packetHeader)> plotPackets;
    std::function<void(const pcap_pkthdr* packetHeader)> plotBytes;
    std::function<void(const pcap_pkthdr* packetHeader)> updateTimestamp;

    std::function<void(ipHeader *ih)> determineDirection;
    std::function<void(ipHeader *ih)> determineColorCode;
    std::function<void(ipHeader *ih)> determineHostIp;
    std::function<void(ipHeader *ih)> determineNewAddress;

    std::function<void(const pcap_pkthdr*, const u_char*, ipHeader*, udpHeader*, QString&) > element[COLUMNNUMBER];
    std::function<void(const pcap_pkthdr*, const u_char*, ipHeader*, udpHeader*, QString&) > extractAddress;
    std::function<void(const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString&) > searchFunc;
    std::function<void(const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString&) > dumpFunc;

    std::function<void(const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString&)> insertStatistics;
    std::function<void(const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString&)> insertChart;
    std::vector<std::function<void(const pcap_pkthdr *packetHeader)> > headerActions;
    std::vector<std::function<void(ipHeader *ih)> > determinations;
    std::vector<std::function<void(const pcap_pkthdr*, const u_char*, ipHeader*, udpHeader*, QString&) > > procedure;
    std::vector<std::function<void(const pcap_pkthdr *packetHeader, const u_char *packetData, ipHeader *ih, udpHeader *uh, QString&) > > finalMethod;

    void resetCounter();
    void clearMethods();
    void store(const parseInstruction &instruction);
    void updateStatistics(const parseInstruction &instruction);
    void updateChart(const parseInstruction &instruction);
    void updateDefault(const parseInstruction &instruction);
};

#endif // PARSESCHEME_H
