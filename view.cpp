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
#include <QFile>
#include "view.h"

void ViewXmlReader::read(QString XmlContent, PacketInfoPresenter *&packetInfo) {
    presenter = packetInfo;
    column = 0;

    XmlReader::read(XmlContent);
}

void ViewXmlReader::readerAction(QString elementText) {
    content[column] = elementText;
    accountRowsAndColumns();
}

void ViewXmlReader::accountRowsAndColumns() {
    if (column == COLUMNNUMBER - 1) {
        presenter->show(content);
        column = 0;
    }
    else
        column++;
}

/*--------------------------------------------------------------------------*/

void View::update(const serverState &state) {
    rewriteInfo();
}

void View::update(const Settings &set) {
    getSettings(set);

    rewriteInfo();
}

void View::rewriteInfo() {
    init();
    QString xmlContent;
    getContent(xmlContent);
    if (xmlContent.size() > 0) {
        xmlReader->read(xmlContent, packetInfo);
    }
}

void View::init() {
    packetInfo->init();
}

/*--------------------------------------------------------------------------*/

DatabaseView::DatabaseView(ViewXmlReader *reader) {
    View::xmlReader=reader;
    packetInfo = &tablePacketInfo;
}

/*--------------------------------------------------------------------------*/

StatisticsView::StatisticsView() {
    xmlReader = &myXmlReader;
    packetInfo = &statisticsPacketInfo;
    COLUMN = PROTOCOL;
}

void StatisticsView::update(const serverState &state) {
    sliceNames = state.sliceNames;

    if (sliceNames.size() == 0) {
        View::update(state);
    }
    else {
        rewriteFileInfo();
    }
}

void StatisticsView::init() {
    statisticsPacketInfo.init(COLUMN);
}

void StatisticsView::update(column C) {
    COLUMN = C;

    if (sliceNames.size() == 0)
        rewriteInfo();
    else
        rewriteFileInfo();
}

void StatisticsView::rewriteInfo() {
    View::rewriteInfo();
    statisticsPacketInfo.evaluate();
}

void StatisticsView::rewriteFileInfo() {
    init();
    for (int i = 0; i < sliceNames.size(); i++) {
        QFile file(sliceNames[i]);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString content = QString::fromUtf8(file.readAll());
            if (content.size() > 0)
                xmlReader->read(content, packetInfo);
            file.close();
        }
        else {
            setErrorMessage("Can not open the file " + sliceNames[i], QMessageBox::Critical);
            return;
        }
    }
    statisticsPacketInfo.evaluate();
}

/*--------------------------------------------------------------------------*/

void TrafficView::update(const serverState &state) {
    sliceNames = state.sliceNames;

    if (sliceNames.size() == 0) {
        View::update(state);   
    }
    else {
        rewriteFileInfo();
    }
}

void TrafficView::rewriteInfo() {
    if (sliceNames.size() == 0) {
        View::rewriteInfo();
        trafficPacketInfo.evaluateLastPackets();
    }
    else
        rewriteFileInfo();

}

TrafficView::TrafficView(TrafficPacketInfoPresenter::infoType type)
    : trafficPacketInfo(type){
    packetInfo = &trafficPacketInfo;
    xmlReader = &r;
}

void TrafficView::rewriteFileInfo() {
    init();
    for (int i = 0; i < sliceNames.size(); i++) {
        QFile file(sliceNames[i]);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString content = QString::fromUtf8(file.readAll());
            if (content.size() > 0)
                xmlReader->read(content, packetInfo);
            file.close();
        }
        else {
            setErrorMessage("Can not open the file " + sliceNames[i], QMessageBox::Critical);
            return;
        }
    }
    trafficPacketInfo.evaluateLastPackets();
}
