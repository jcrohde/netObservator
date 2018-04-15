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

#ifndef APPDATA_H
#define APPDATA_H

#include "ippacket.h"
#include "settings.h"

#include <unordered_set>

struct Settings {
    int viewSettings;
    sniffSettings sniff;
};

void getSetting(Settings &settings);
void getDNS(std::unordered_set<addressItem> &addresses, std::vector<addressItem> &oldHosts);

QString generateAppDataFolder(const QString &foldername);

void writeDNS(const std::unordered_set<addressItem> &addresses);
void writeSettings(const Settings &settings);

#endif // APPDATA_H
