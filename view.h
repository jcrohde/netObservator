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

#include <QVBoxLayout>
#include "util.h"
#include "packetinfopresenter.h"
#include "xmlreader.h"

#ifndef VIEW_H
#define VIEW_H

class ViewXmlReader : private XmlReader {
public:
    void read(QString XmlContent, PacketInfoPresenter *packetInfo);

private:
    void readerAction(QString elementText);
    void accountRowsAndColumns();

    PacketInfoPresenter *presenter;
    int column;
    QString content[COLUMNNUMBER];
};

class View: public SettingObserver, public ModelObserver
{
public:
    View(ViewXmlReader *reader);
    ~View() {}

    void update(const Settings &set);
    void update(modelState state);
    void init();

    TablePacketInfoPresenter tablePacketInfo;

private:    
    void rewriteInfo();

    ViewXmlReader *xmlReader;
    QString xmlContent;

};

#endif // VIEW_H
