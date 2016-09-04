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
    void read(QString XmlContent, PacketInfoPresenter *&packetInfo);

private:
    void readerAction(QString elementText);
    void accountRowsAndColumns();

    PacketInfoPresenter *presenter;
    int column;
    QString content[COLUMNNUMBER];
};

/*--------------------------------------------------------------------------*/

class View : public serverObserver {
public:
    std::function<void(QString&)> getContent;

    virtual void update(const serverState &state);
    void update(const Settings &set);
    void getSettings(const Settings &set) {setting = set; packetInfo->update(set);}

    PacketInfoPresenter *packetInfo;

protected:
    Settings setting;

    virtual void rewriteInfo();
    virtual void init();
    ViewXmlReader *xmlReader;
};

/*--------------------------------------------------------------------------*/

class DatabaseView : public View
{
public:
    DatabaseView(ViewXmlReader *reader);
    ~DatabaseView() {}

    void compose(ViewComposition composition);
    TablePacketInfoPresenter tablePacketInfo;
};

/*--------------------------------------------------------------------------*/

class StatisticsView : public View
{
public:
    StatisticsView();
    ~StatisticsView() {}

    void update(const serverState &state);
    void update(column C);

    void setSliceNames(const QStringList &names) {sliceNames = names;}
    StatisticsPacketInfoPresenter statisticsPacketInfo;

protected:
    void init();
    void rewriteInfo();

private:
    column COLUMN;
    QStringList sliceNames;

    ViewXmlReader myXmlReader;

    void rewriteFileInfo();
};

/*--------------------------------------------------------------------------*/

class TrafficView : public View {
public:
    TrafficView(TrafficPacketInfoPresenter::infoType type);
    void setOutputDevice(std::function<void(int, long int)> device) {trafficPacketInfo.evaluatePacketsOfSecond = device;}
    void update(const serverState &state);

    void rewriteInfo();

    void setSliceNames(const QStringList &names) {sliceNames = names;}

    TrafficPacketInfoPresenter::infoType getType() {return trafficPacketInfo.getType();}

private:
    TrafficPacketInfoPresenter trafficPacketInfo;
    ViewXmlReader r;

    QStringList sliceNames;

    void rewriteFileInfo();
};

#endif // VIEW_H
