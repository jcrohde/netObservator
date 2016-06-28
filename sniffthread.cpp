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

#include <ws2tcpip.h>
#include "sniffthread.h"
#include "util.h"
#include <QDebug>

static bool findaddr(std::vector<ipAddress> &addrVec, ipAddress addr) {
    return std::find(addrVec.begin(),addrVec.end(),addr) != addrVec.end();
}

void SniffThread::start(int deviceNumber) {
    if (!thread.joinable()) {
        devId = deviceNumber;
        bRun = true;
        thread = std::thread(&SniffThread::run, this);
    }
}

void SniffThread::stop(QString &content) {
    if (thread.joinable()) {
        bRun = false;
        thread.join();
        content = xmlContent;
    }
}

void SniffThread::run() {
    if (setting.save) {
        xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        xmlContent += "<"+ SNIFFED + ">";
    }
    else
        xmlContent.clear();

    devRef.getLocalIpAddresses(devId,localAddresses);

    executeMainLoop(DNSsingleton::getInstance());

    if (setting.save) xmlContent += "</" + SNIFFED + ">";
}

void SniffThread::executeMainLoop(DNSsingleton &cache) {
    pcap_t* handle;
    struct pcap_pkthdr *header;
    const u_char *packetData;
    int readCode;

    handle = pcap_open_live(devRef.getDevice(devId)->name,BUFSIZ,0,1000,errorBuffer);
    if(handle == NULL) {
        QString errorMessage = "Can not open device.";
        errorMessage += errorBuffer;
        setErrorMessage(errorMessage, QMessageBox::Critical);
    }
    else {
        do {
            readCode = pcap_next_ex(handle, &header, &packetData);
            if (readCode > 0) handlePacket(header, packetData,cache);
            else if (readCode < 0) setErrorMessage("An error occured while reading a packet",QMessageBox::Critical);
        } while(bRun && readCode >= 0);
        pcap_close(handle);
    }
}

void SniffThread::handlePacket(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache) {
    getPacketInfo(header,packetData,cache);
    presentPacketInfo();
}

void SniffThread::getPacketInfo(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache) {
    ipHeader *ih = (ipHeader *) (packetData + 14);
    u_int ipLength = (ih->versionAndInternetHeaderLength & 0xf) * 4;
    udpHeader *uh = (udpHeader *) ((u_char*)ih + ipLength);

    currentTimeInSeconds = header->ts.tv_sec;

    getTimeString();
    foundNew = getConnectedAddressInfo(ih,uh,cache);
    content[PORT] = QString::number(currentPort);
    content[ADDRESS] = currentHome.toQString();
    if (bRun) {
        if (setting.showInfo[ENTIRE_PACKET] || setting.showInfo[PAYLOAD] || setting.save) readPacket(header,packetData);
    }
}

void SniffThread::getTimeString() {
    char time_string[20];

    if (currentTimeInSeconds > 0) {
        struct tm* ptm;
        ptm = localtime (&currentTimeInSeconds);

        strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
        content[TIME] = time_string;
    }
    else content[TIME] = "null";
}

bool SniffThread::updateConnectedAddresses(ipAddress &addr,DNSsingleton &cache) {
    addressItem item(addr,"name",currentTimeInSeconds);
    std::vector<addressItem>::iterator ptr = std::find(addresses.begin(),addresses.end(),item);

    if (ptr == addresses.end()) {
        cache.handle(item);
        content[HOSTNAME] = item.hostname;
        addresses.push_back(item);
        return true;
    }
    else {
        ptr->timeStamp = currentTimeInSeconds;
        content[HOSTNAME] = ptr->hostname;
        return false;
    }
}

bool SniffThread::getConnectedAddressInfo(ipHeader *ih,udpHeader *uh,DNSsingleton &cache) {
    if (findaddr(localAddresses,(ipAddress) ih->destAddr)) {
        content[DIRECTION] = "in";
        incoming = true;
        currentPort = ntohs(uh->srcPort);
        currentHome = ih->srcAddr;
        return updateConnectedAddresses(ih->srcAddr,cache);
    }
    else {
        content[DIRECTION] = "out";
        incoming = false;
        currentPort = ntohs(uh->destPort);
        currentHome = ih->destAddr;
        return updateConnectedAddresses(ih->destAddr,cache);
    }
}

void SniffThread::readPacket(const struct pcap_pkthdr *header,const u_char *packetData) {
    int k;

    content[PAYLOAD].clear();
    content[ENTIRE_PACKET].clear();
    for(unsigned int i=0; i<header->caplen; i++) {
        k = packetData[i];
        if (k<16) content[ENTIRE_PACKET] += "0";
        content[ENTIRE_PACKET] += QString::number(k,16) + " ";
        if (isprint(k)) content[PAYLOAD] += (short) k;
        else content[PAYLOAD] += ".";
    }
}

void SniffThread::presentPacketInfo() {
    if (bRun) killOldEntries();
    if (bRun && setting.save) {
        storePacketInfo();
    }
    if (bRun && foundNew) {
        showPacketInfo();
    }
}

void SniffThread::killOldEntries() {
    int i = addresses.size();
    while (i > 0) {
        i--;
        if (currentTimeInSeconds-addresses[i].timeStamp>setting.duration) {
            addresses.erase(addresses.begin() +i);
            display->removeRow(i);
        }
    }
}

void SniffThread::storePacketInfo() {
    xmlContent += "<"+ PACKETINFO + ">";
    for (int i = 0; i < COLUMNNUMBER; i++)
        xmlContent +=  "<" + LABEL[i] + ">" + escape(content[i]) + "</" +LABEL[i] + ">";
    xmlContent += "</" + PACKETINFO + ">";
}

void SniffThread::showPacketInfo() {
    column = 0;
    row = display->rowCount();
    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (setting.showInfo[i]) {
            showAttribute(column,content[i]);
            column++;
        }
    }
}

void SniffThread::showAttribute(int column, QString attribute) {
    QStandardItem *messageItem = new QStandardItem(unEscape(attribute));
    incoming ? messageItem->setBackground(inBrush) : messageItem->setBackground(outBrush);
    display->setItem(row,column,messageItem);
}
