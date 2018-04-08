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

#include "settings.h"


viewSettings::viewSettings() {
    for (int i = 0; i < COLUMNNUMBER; i++) {
        showInfo[i] = i == PROTOCOL || i == HOSTPORT || i == HOSTADDRESS;
    }

    shownColumns = 3;
}

sniffSettings::sniffSettings() {
    mode = Mode::SAVEANDSHOW;
    duration = 20;
    sliceFileName = "Slice";
    storeFolderName = "Sniffed";
    sliceSize = 1000;
}

QString onLocalOrSrc(QString str) {
    if (str == LABEL[LOCALSRCADDRESS])
        return "Local or SRC Address";
    else if (str == LABEL[LOCALSRCPORT])
        return "Local or SRC Port";
    else
        return str;
}
