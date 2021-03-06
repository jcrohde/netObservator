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

#ifndef DEVICES_H
#define DEVICES_H

#include <QString>
#include <QList>
#include "ippacket.h"

class Devices
{
    pcap_if_t *alldevs;
public:
    Devices();

    void getDeviceStrings(QList<QString> &deviceList) const;
    const pcap_if_t *getDevice(int id) const;

    void getLocalIpAddresses(int id,std::vector<ipAddress> &addresses) const;
    void getLocalIpAddresses(const pcap_if_t *dev,std::vector<ipAddress> &addresses) const;
};

#endif // DEVICES_H
