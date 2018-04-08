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

#ifndef OBSERVERS_H
#define OBSERVERS_H

#include "ippacket.h"
#include <QStringList>

struct serverState{
    std::set<ipAddress> addresses;
    bool firstDocument;
    QStringList sliceNames;
    bool lastDocument;
    bool blockedBySniffThread;
    bool empty;
    QString title;
};

class serverObserver {
public:
    virtual void update(const serverState &state) {storedState = state;}
private:
    serverState storedState;
};

struct sniffState {
    std::set<ipAddress> addresses;
    QString folderName;
    QString errorMessage;
    bool sniffing;
    bool valid;
};

class SniffObserver {
public:
    virtual void update(const sniffState &state) {storedState = state;}
private:
    sniffState storedState;
};

#endif // OBSERVERS_H
