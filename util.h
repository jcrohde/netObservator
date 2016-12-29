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

#ifndef UTIL_H
#define UTIL_H

#include <QString>

#include <QMessageBox>
#include <memory>
#include <functional>

#include <mutex>

const QString SNIFFED = "sniffed";
const QString PACKETINFO = "packetInfo";

struct ViewComposition {
    enum class presentation {
        TABLE,
        TEXT
    };

    presentation mode;
};

const QString ARBITRARY = "Arbitrary";

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
