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

#ifndef PACKETPARSER_H
#define PACKETPARSER_H

#include "settings.h"
#include "ippacket.h"
#include "dnssingleton.h"
#include "packetinfopresenter.h"
#include "util.h"
#include "parsescheme.h"

class PacketParser : public ParseScheme
{
public:
    PacketParser();

    void setDurationTime(int time) {duration = time;}

    void extract(const QString &filename, std::set<ipAddress> &addr);
    void search(const QString &filename, const SearchCommand &command);

    void setSearchCommand(const SearchCommand &command) {
        this->command = command;

        parseInstruction instruction;
        instruction.mode = Mode::SEARCH;
        instruction.settings = command.settings;

        this->configure(instruction);
    }

    void setPacketInfoPresenter(PacketInfoPresenter *presenter) {packetInfo = presenter;}

    void parse(QString contentFile, const parseInstruction &instruction);
    void parse(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);

    void update(const std::vector<ipAddress> &localAddr) {localAddresses = localAddr;}

    const std::map<QString,std::vector<long int> > &getAppearance() {
        for (auto iter = appearance.begin(); iter != appearance.end(); ++ iter) {
             std::sort(iter->second.begin(),iter->second.end());
        }
        return appearance;
    }

    void executeParseloop(const QStringList &content);

private:

    int duration;
    PacketInfoPresenter *packetInfo;


    void getPacketInfo(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache);

    void presentPacketInfo(const struct pcap_pkthdr *header,const u_char *packetData);
    void killOldEntries();
};

#endif // PACKETPARSER_H
