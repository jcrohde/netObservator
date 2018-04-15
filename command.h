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

#ifndef COMMAND_H
#define COMMAND_H

#include <QStringList>

enum class CommandCode {
    CLEAR,
    LOAD,
    LOADFILE,
    LOADFOLDER,
    SAVEFILEAS,
    SAVEFILE,

    EXPORTXML,
    EXPORTJSON,

    LOADSLICE,

    ADDTAB,

    BACK,
    FORWARD,

    SHOWPACKETFILTEREDITOR,

    CHARTVISIBLE,
    CHARTINVISIBLE,

    SHOWSTATISTICSDIALOG,
    SHOWTRAFFICDIALOG,
    SHOWBYTEDIALOG,
    SHOWCOLUMNSETTINGSDIALOG,

    SHOWSETTINGSDIALOG,

    SHOWSEARCHTABDIALOG,
    SHOWSEARCHFILEDIALOG,

    SNIFF,
    CHANGEDEVICE,

    HELP,
    LICENSE,
    ABOUT
};

struct Command {
    CommandCode code;
    QStringList arguments;
};

#endif // COMMAND_H
