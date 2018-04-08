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

#ifndef STRINGFACTORY_H
#define STRINGFACTORY_H

#include <QString>
#include "util.h"

enum class stringKey {
    PROGRAMNAME,
    PROGRAMNAMELONG,
    VERSION,

    HELP,
    LICENSE,
    ABOUT
};

class StringFactory
{
public:
    StringFactory();
    QString getStringFromKey(stringKey key);
    QString getTitleFromKey(stringKey key);

private:
    QString getProgramName();
    QString getProgramNameLong();
    QString getVersion();

    QString getTitle();

    QString getHelp();
    QString getLicense();
    QString getAbout();
};

#endif // STRINGFACTORY_H
