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

#ifndef PACKETINFOPRESENTER_H
#define PACKETINFOPRESENTER_H

#include <QTableView>
#include <QStandardItemModel>
#include <QTextEdit>
#include "settings.h"
#include "util.h"

struct DisplayTableView {
    QStandardItemModel *tableModel;
    QTableView *display;

    DisplayTableView() {
        tableModel = new QStandardItemModel();
        display = new QTableView();
        display->setModel(tableModel);
        display->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
};

class PacketInfoPresenter
{
public:
    PacketInfoPresenter() {;}

    DisplayTableView display;

    void update(const viewSettings &set) {setting = set;}
    virtual void show(QString *content) = 0;
    virtual void init() = 0;

protected:
    viewSettings setting;
};

/*--------------------------------------------------------------------------*/

class TablePacketInfoPresenter : public PacketInfoPresenter
{
public:

    enum class colorCode {
        INCOMINGSING = 0,
        OUTGOINGSING = 1,
        THIRDPARTYSING = 2,
        INCOMINGGROUP = 3,
        OUTGOINGGROUP = 4,
        THIRDPARTYGROUP = 5,
        INCOMINGOTHER = 6,
        OUTGOINGOTHER = 7,
        THIRDPARTYOTHER = 8
    };

    TablePacketInfoPresenter();

    void show(QString *content);
    void show(QString *content, colorCode direction);

    void init();

private:
    int row, column;

    QBrush inBrush;
    QBrush outBrush;
    QBrush thirdBrush;
    QBrush inGroupBrush;
    QBrush outGroupBrush;
    QBrush thirdGroupBrush;
    QBrush inOtherBrush;
    QBrush outOtherBrush;
    QBrush thirdOtherBrush;

    QBrush *background;

    void showAttribute(int column, QString attribute);
    colorCode determineDirection(QString *content);
    void setBackGround(colorCode direction);
    void evaluateTime(QString &time);
};

/*--------------------------------------------------------------------------*/

struct sizedPair {
    std::pair<QString, std::vector<long int> > value;

    operator<(const sizedPair &pair) const {
        if (value.second.size() < pair.value.second.size())
            return true;
        else if (value.second.size() == pair.value.second.size())
            return value.first < pair.value.first;
        else
            return false;
    }
};

class StatisticsPacketInfoPresenter : public PacketInfoPresenter
{
public:
    StatisticsPacketInfoPresenter();

    std::function<void(std::vector<sizedPair>&, long int, long int)> visualize;

    void show(QString *content);

    void init(column C);
    void init();

    void evaluate(const std::map<QString,std::vector<long int> > &data);

private:
    int totalNumber;

    long int timeBegin, timeEnd;

    column COLUMN;
    std::map<QString,std::vector<long int> > appearance;
};

/*--------------------------------------------------------------------------*/

class TrafficPacketInfoPresenter : public PacketInfoPresenter {
public:
    enum class infoType {
        PACKETS,
        BYTES
    };

    TrafficPacketInfoPresenter(infoType type);

    virtual void show(QString *content);
    virtual void init();

    std::function<void(int,long int)> evaluatePacketsOfSecond;

    infoType getType() {return myType;}
    void evaluateLastPackets();
private:
    infoType myType;
    long int timeStamp, packetTimeStamp;
    int packetNumber;
};

#endif // PACKETINFOPRESENTER_H
