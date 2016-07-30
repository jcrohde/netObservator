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

#ifndef PACKETINFOPRESENTER_H
#define PACKETINFOPRESENTER_H

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItemModel>
#include <QTextEdit>
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

class PacketInfoPresenter : public SettingObserver
{
public:
    PacketInfoPresenter() {;}

    enum class colorCode {
        INCOMINGSING = 0,
        OUTGOINGSING = 1,
        INCOMINGGROUP = 2,
        OUTGOINGGROUP = 3,
        INCOMINGOTHER = 4,
        OUTGOINGOTHER = 5,
    };

    DisplayTableView display;

    void show(QString *content);
    virtual void show(QString *content, colorCode direction) = 0;

    virtual void init() = 0;

private:
    colorCode determineDirection(QString *content);
};

class TablePacketInfoPresenter : public PacketInfoPresenter
{
public:
    TablePacketInfoPresenter();

    void show(QString *content, colorCode direction);

    void init();

private:
    int row, column;

    QBrush inBrush;
    QBrush outBrush;
    QBrush inGroupBrush;
    QBrush outGroupBrush;
    QBrush inOtherBrush;
    QBrush outOtherBrush;

    QBrush *background;

    void showAttribute(int column, QString attribute);
};

// will appear in a later release
class TextPacketInfoPresenter : public PacketInfoPresenter {
public:
    TextPacketInfoPresenter();

    void show(QString *content, colorCode direction);

    QTextEdit *display;

    void init();

    QString displayText;

private:



    QString inString;
    QString outString;
    QString inGroupString;
    QString outGroupString;
    QString inOtherString;
    QString outOtherString;

    QString *background;

    /*void showAttribute(int column, QString attribute);*/
};

#endif // PACKETINFOPRESENTER_H
