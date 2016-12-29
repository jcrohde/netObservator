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

#include "devices.h"
#include "util.h"

Devices::Devices()
{
    char error[PCAP_ERRBUF_SIZE];
    if(pcap_findalldevs(&alldevs,error)==-1) {
        setErrorMessage("Error in finding devices: " + QString(error),QMessageBox::Critical);
    }
}

void Devices::getDeviceStrings(QList<QString> &deviceList) const {
    pcap_if_t *device;

    for(device=alldevs; device; device = device->next)
    {
        deviceList.push_back(QString(device->description));
    }
}

const pcap_if_t *Devices::getDevice(int id) const {
    pcap_if_t *device=alldevs;
    for(int i=0; i< id; i++)
        device=device->next;
    return device;
}

void Devices::getLocalIpAddresses(int id, std::vector<ipAddress> &addresses) const {
    getLocalIpAddresses(getDevice(id),addresses);
}

void Devices::getLocalIpAddresses(const pcap_if_t *dev,std::vector<ipAddress> &addresses) const {
    std::vector<QString> localAddress;

    for(pcap_addr_t *address=dev->addresses; address!=NULL; address=address->next) {
        if(address->addr->sa_family == AF_INET)
            localAddress.push_back(QString(inet_ntoa(((struct sockaddr_in*)address->addr)->sin_addr)));
    }

    addresses.clear();
    for (QString str: localAddress) {
        addresses.push_back(ipAddress(str));
    }
}

