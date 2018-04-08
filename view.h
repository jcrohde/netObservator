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

#include <QVBoxLayout>
#include "observers.h"
#include "packetinfopresenter.h"
#include "packetparser.h"
#include "hostchart.h"

#ifndef VIEW_H
#define VIEW_H

/*--------------------------------------------------------------------------*/

class View : public serverObserver {
public:

    PacketParser *parser;

    std::function<void(QString&)> getContent;

    virtual void update(const serverState &state);
    void update(const parseInstruction &inst);
    void getSettings(const parseInstruction &inst) {instruction = inst; packetInfo->update(instruction.settings);}

    PacketInfoPresenter *packetInfo;

protected:
    parseInstruction instruction;
    QStringList sliceNames;

    virtual void rewriteInfo();
    virtual void init();
};

/*--------------------------------------------------------------------------*/

class DatabaseView : public View
{
public:
    DatabaseView();
    ~DatabaseView() {}

    void rewriteInfo();

    void compose(ViewComposition composition);
    TablePacketInfoPresenter tablePacketInfo;
    HostChart chart;
    QScrollArea *chartScene;
    QDialog * dialog;
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

    void rewriteFileInfo();
};

#endif // VIEW_H
