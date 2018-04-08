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

#ifndef PACKETPARSER_H
#define PACKETPARSER_H

#include "settings.h"
#include "ippacket.h"
#include "dnssingleton.h"
#include "packetinfopresenter.h"
#include "util.h"
#include "parsescheme.h"

#include <qjsonarray.h>

class PacketParser : public ParseScheme
{
public:
    PacketParser();

    void setDurationTime(int time) {duration = time;}

    void extract(const QString &filename, std::set<ipAddress> &addr);
    void search(const SearchCommand &command, const QString &filename = "");

    void setPacketInfoPresenter(PacketInfoPresenter *presenter) {packetInfo = presenter;}

    void parse(QString contentFile, const parseInstruction &instruction);
    void parse(QStringList contentFile, const parseInstruction &instruction);
    void parse(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);

    void update(const std::vector<ipAddress> &localAddr) {localAddresses = localAddr;}

    const std::map<QString,std::vector<long int> > &getAppearance();

    void executeParseloop(const QStringList &content);

private:

    int duration;
    PacketInfoPresenter *packetInfo;
    QScopedPointer<QXmlStreamWriter> xmlWriter;
    QScopedPointer<QJsonArray> jsonArray;

    void getPacketInfo(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);

    void setSearchCommand(const SearchCommand &command);

    QString getDumpFile(Mode mode, const QString &contentFile);
    void runLoop(pcap_t *fp);
    void firePlot();
    void writeXML();
    void writeJson();
    void presentPacketInfo();
    void killOldEntries();
};

#endif // PACKETPARSER_H
