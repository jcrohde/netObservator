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
#include <QFile>

static bool findaddr(std::vector<ipAddress> &addrVec, ipAddress addr) {
    return std::find(addrVec.begin(),addrVec.end(),addr) != addrVec.end();
}

SniffThread::~SniffThread() {
    observers.clear();
    stop();
}

void SniffThread::unregisterObserver(SniffObserver *observer) {
    std::vector<SniffObserver*>::iterator pos = std::find(observers.begin(),observers.end(),observer);
    if (pos != observers.end())
        observers.erase(pos);
}

void SniffThread::start(int deviceNumber) {
    if (!thread.joinable()) {
        devId = deviceNumber;

        bRun = true;
        valid = true;
        notifyObservers();

        thread = std::thread(&SniffThread::run, this);
    }
}

bool SniffThread::stop() {
    if (thread.joinable()) {
        bRun = false;
        thread.join();

        notifyObservers();
    }
    return valid && setting.save;
}

void SniffThread::notifyObservers() {
    sniffState state;

    state.valid = valid;
    state.sniffing = bRun;
    state.addresses = seenAdresses;
    if (valid && !bRun) {
        for (int i = 1; i <= sliceNumber; i++) {
            state.sliceNames.append(setting.sliceFileName + QString::number(i) + ".xml");
        }
    }
    if (!valid)
        state.errorMessage = errorStr;

    for (auto observer: observers) {
        observer->update(state);
    }
}

void SniffThread::run() {
    if (setting.save) {
        xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        xmlContent += "<"+ SNIFFED + ">";
    }
    else
        xmlContent.clear();

    init();

    startMainLoop();

    if (setting.save) xmlContent += "</" + SNIFFED + ">";

    if (!valid)
        notifyObservers();
}

void SniffThread::init() {
    counter = 0;
    sliceNumber = 0;

    seenAdresses.clear();
    addresses.clear();
    data.packetNumber = 0;
    data.seconds = 0;
    firePacketNumber(data);
    data.seconds = time(NULL);

    devs.getLocalIpAddresses(devId,localAddresses);
}

void SniffThread::startMainLoop() {
    pcap_t* handle;

    handle = pcap_open_live(devs.getDevice(devId)->name,BUFSIZ,0,1000,errorBuffer);
    if(handle == NULL) {
        QString errorStr = "Can not open device. ";
        errorStr += errorBuffer;
        valid = false;
    }
    else {
        if (setFilter(handle)) {
            executeMainLoop(handle,DNSsingleton::getInstance());
        }
        pcap_close(handle);
    }
}

bool SniffThread::setFilter(pcap_t*& handle) {
    if (!filterString.isEmpty()) {

        std::string str = filterString.toStdString();
        char *filter = new char [str.size()+1];
        strcpy(filter, str.c_str());

        bpf_u_int32 netmask =((struct sockaddr_in *) devs.getDevice(devId)->addresses->netmask)->sin_addr.S_un.S_addr;
        bpf_program prog;

        if (valid) {
            int i = pcap_compile(handle,&prog,filter,0,netmask);
            if (i < 0) {
                char *error = pcap_geterr(handle);
                errorStr = "Can not compile filtering expression: " + QString(error);
                valid = false;
            }
        }

        if (valid) {
            if (pcap_setfilter(handle, &prog) < 0) {
                errorStr = "Can not set the packet filter for the selected device";
                valid = false;
            }
        }
    }

    return valid;
}

void SniffThread::executeMainLoop(pcap_t*& handle,DNSsingleton &cache) {
    struct pcap_pkthdr *header;
    const u_char *packetData;
    int readCode;

    do {
        readCode = pcap_next_ex(handle, &header, &packetData);
        if (readCode > 0) {
            handlePacket(header, packetData,cache);
            counter++;
        }
        else if (readCode < 0) {
            errorStr = "An error occured while reading a packet";
            valid = false;
        }
        if (setting.save && (!bRun || !valid || counter >= setting.sliceSize)) {
            generateSlice();
            counter = 0;
        }
        if (bRun && (readCode == 0 || currentTimeInSeconds > data.seconds)) {
            firePacketNumber(data);
            fireByteNumber(bytes);
            if (readCode) {
                data.packetNumber = 1;
                bytes.packetNumber = header->caplen;
            }
            else {
                data.packetNumber = 0;
                bytes.packetNumber = 0;
            }
            data.seconds = currentTimeInSeconds;
        }
        else {
            data.packetNumber++;
            bytes.packetNumber += header->caplen;
        }

    } while(bRun && readCode >= 0);

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

    foundNew = getConnectedAddressInfo(ih,uh,cache);
    getProtocol(ih);
    content[TIME] = QString::number(currentTimeInSeconds);
    content[HOSTPORT] = QString::number(hostPort);
    content[LOCALSRCADDRESS] = localIP.toQString();
    content[LOCALSRCPORT] = QString::number(localPort);
    content[HOSTADDRESS] = currentHome.toQString();
    if (setting.save) seenAdresses.insert(currentHome);
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
    thirdParty = false;
    if (findaddr(localAddresses,(ipAddress) ih->destAddr)) {
        content[DIRECTION] = "in";
        incoming = true;
        hostPort = ntohs(uh->srcPort);
        localPort = ntohs(uh->destPort);
        currentHome = ih->srcAddr;
        localIP = ih->destAddr;
        return updateConnectedAddresses(ih->srcAddr,cache);
    }
    else {

        incoming = false;
        hostPort = ntohs(uh->destPort);
        localPort = ntohs(uh->srcPort);
        currentHome = ih->destAddr;
        localIP = ih->srcAddr;
        if (findaddr(localAddresses,(ipAddress) ih->srcAddr))
            content[DIRECTION] = "out";
        else {
            thirdParty = true;
            content[DIRECTION] = "to Host from SRC";
        }
        return updateConnectedAddresses(ih->destAddr,cache);
    }
}

void SniffThread::getProtocol(ipHeader *ih) {
    switch (ih->protocol) {
    case 6:
        content[PROTOCOL] = "TCP";
        colorCode = 0;
        break;

    case 17:
        content[PROTOCOL] = "UDP";
        colorCode = 0;
        break;

    case 1:
        content[PROTOCOL] = "ICMP";
        colorCode = 3;
        break;

    case 2:
        content[PROTOCOL] = "IGMP";
        colorCode = 3;
        break;

    default:
        int i = ih->protocol;
        colorCode = 6;
        content[PROTOCOL] = QString::number(i);
        break;
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
        getTimeString();
        packetInfo->show(content,(TablePacketInfoPresenter::colorCode) (colorCode + !incoming +thirdParty));
    }
}

void SniffThread::killOldEntries() {
    myCellMutex.lock();
    int i = addresses.size();
    while (i > 0) {
        i--;
        if (currentTimeInSeconds-addresses[i].timeStamp>setting.duration) {
            addresses.erase(addresses.begin() +i);
            packetInfo->display.tableModel->removeRow(i);
        }
    }
    myCellMutex.unlock();
}

void SniffThread::storePacketInfo() {
    xmlContent += "<"+ PACKETINFO + ">";
    for (int i = 0; i < COLUMNNUMBER; i++)
        xmlContent +=  "<" + LABEL[i] + ">" + escape(content[i]) + "</" +LABEL[i] + ">";
    xmlContent += "</" + PACKETINFO + ">";
}

void SniffThread::generateSlice() {
    xmlContent += "</" + SNIFFED + ">";

    sliceNumber++;

    QString fileName = setting.sliceFileName + QString::number(sliceNumber) + ".xml";
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(xmlContent.toUtf8());
        file.close();
    }
    else {
        setErrorMessage("Can not open the file " + fileName, QMessageBox::Critical);
        valid = false;
    }

    if (valid) {
        xmlContent = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        xmlContent += "<"+ SNIFFED + ">";
    }
}
