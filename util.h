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

#ifndef UTIL_H
#define UTIL_H

#include <QString>

#include <QMessageBox>
#include <memory>
#include <functional>

#include <mutex>

struct Protocol {
    Protocol() : TCP(0), UDP(0), ICMP(0), IGMP(0), other(0) {}

    int TCP;
    int UDP;
    int ICMP;
    int IGMP;
    int other;

    inline int sum() const {return TCP + UDP + ICMP + IGMP + other;}
};

const QString SNIFFED = "sniffed";
const QString PACKETINFO = "packetInfo";

struct ViewComposition {
    enum class presentation {
        TABLE,
        TEXT
    };

    presentation mode;
};

struct traffic {
    int packetNumber;
    long int seconds;
};

Q_DECLARE_METATYPE(traffic)

extern std::mutex myCellMutex;


extern QString escape(QString content);
extern QString unEscape(QString content);

extern void setErrorMessage(QString message, QMessageBox::Icon icon);

extern void getPcapFileNames(const QString &folderName, QStringList &fileNames);

#endif // UTIL_H
