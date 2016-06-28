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

#include <QStandardItemModel>
#include "view.h"

void View::update(const Settings &set) {
    SettingObserver::update(set);
    column = 0;
    row = 0;
    shownColumn = 0;

    init();
    if (xmlContent.size() > 0)
        read(xmlContent);
}

void View::update(modelState state) {
    xmlContent = state.document;
    column = 0;
    shownColumn = 0;
    row = 0;

    init();

    read(xmlContent);
}

void View::readerAction() {
    QString attribute = reader->readElementText();
    if (column == DIRECTION) {
        incoming = attribute == "->" || attribute == "-&gt;" || attribute == "in";
    }
    if (SettingObserver::setting.showInfo[column]) {
        showAttribute(attribute);
    }
    accountRowsAndColumns();
}

void View::showAttribute(QString attribute) {

    QStandardItem *messageItem = new QStandardItem(unEscape(attribute));  
    rowItems.append(messageItem);
    display->setItem(row,shownColumn,messageItem);
    shownColumn++;
}

void View::accountRowsAndColumns() {
    if (column == COLUMNNUMBER - 1) {
        for (int i = 0; i < shownColumn; i++)
            incoming ? rowItems[i]->setBackground(inBrush) : rowItems[i]->setBackground(outBrush);
        incoming = false;
        rowItems.clear();
        shownColumn = 0;
        column = 0;
        row++;
    }
    else
        column++;
}

void View::init() {
    rowItems.clear();
    display->clear();
    display->setRowCount(0);

    QStringList LabelList;

    for (int i = 0; i< COLUMNNUMBER; i++) {
        if (SettingObserver::setting.showInfo[i]) LabelList.push_back(LABEL[i]);
    }

    display->setHorizontalHeaderLabels(LabelList);
    row = 0;
}
