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

#include <ws2tcpip.h>
#include "sniffthread.h"
#include "util.h"
#include <QFile>
#include <QDir>

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
    return valid;
}

void SniffThread::notifyObservers() {
    sniffState state;

    state.valid = valid;
    state.folderName = currentStoreFolderName;
    state.sniffing = bRun;
    state.addresses = parser->getSeenAddresses();
    if (!valid)
        state.errorMessage = errorStr;

    for (auto observer: observers) {
        observer->update(state);
    }
}

void SniffThread::run() {
    init();

    startMainLoop();

    if (!valid)
        notifyObservers();
}

void SniffThread::init() {
    counter = 0;
    sliceNumber = 0;

    addresses.clear();
    data.packetNumber = 0;
    data.seconds = 0;
    data.seconds = time(NULL);

    long int currentTimeInSeconds = (long)time(NULL);
    char time_string[20];

    if (currentTimeInSeconds > 0) {
        struct tm* ptm;
        ptm = localtime (&currentTimeInSeconds);

        strftime (time_string, sizeof (time_string), "%Y-%m-%d_%H-%M-%S", ptm);
    }

    if (settings.mode != settings.Mode::JUSTSHOW) {
        currentStoreFolderName = settings.storeFolderName + QString(time_string);

        QDir dir(currentStoreFolderName);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
    }
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

    sliceNumber++;

    QString fileName = currentStoreFolderName + "/" + settings.sliceFileName + QString::number(sliceNumber) + ".pcap";

    if (valid && bRun && settings.mode != settings.Mode::JUSTSHOW)
        dumpfile = pcap_dump_open(handle,fileName.toLatin1().data());

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
        if (settings.mode != settings.Mode::JUSTSHOW
        && (!bRun || !valid || counter >= settings.sliceSize)) {
            generateSlice(handle);
            counter = 0;
        }

    } while(bRun && readCode >= 0);

}

void SniffThread::handlePacket(const struct pcap_pkthdr *header, const u_char *packetData, DNSsingleton &cache) {
    if (bRun && settings.mode != settings.Mode::JUSTSHOW)
        storePacketInfo(header, packetData);

    if (bRun && settings.mode != settings.Mode::JUSTSAVE)
        parser->parse(header,packetData,cache);
}



void SniffThread::storePacketInfo(const struct pcap_pkthdr *header,const u_char *packetData) {
    pcap_dump((unsigned char *)dumpfile, header, packetData);
}

void SniffThread::generateSlice(pcap_t*& handle) {
    sliceNumber++;

    QString fileName = currentStoreFolderName + "/" + settings.sliceFileName + QString::number(sliceNumber) + ".pcap";

    pcap_dump_close(dumpfile);
    if (valid && bRun) dumpfile = pcap_dump_open(handle,fileName.toLatin1().data());
}
