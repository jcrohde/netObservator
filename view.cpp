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

#include <QStandardItemModel>
#include <QFile>
#include <QLabel>
#include <QScrollBar>
#include "view.h"

void View::update(const serverState &state) {
    if (state.sliceNames.isEmpty()) {
        sliceNames.clear();
        sliceNames.append(state.title);
    }
    else sliceNames = state.sliceNames;
    if (state.blockedBySniffThread)
        instruction.mode = Mode::FIRSTPACKET;
    else
        instruction.mode = Mode::ALL;

    rewriteInfo();
}

void View::update(const parseInstruction &inst) {
    getSettings(inst);

    rewriteInfo();
}

void View::rewriteInfo() {
    init();
    QString xmlContent;
    getContent(xmlContent);
    if (xmlContent.size() > 0) {
        parser->setPacketInfoPresenter(packetInfo);
        parser->parse(xmlContent, instruction);
    }
}

void View::init() {
    packetInfo->init();
}

/*--------------------------------------------------------------------------*/

void DatabaseView::rewriteInfo() {
    init();
    QString xmlContent;
    getContent(xmlContent);
    if (xmlContent.size() > 0) {
        parseInstruction localInstruction = instruction;
        localInstruction.plotByteNumber = localInstruction.plotPacketNumber = false;
        parser->setPacketInfoPresenter(packetInfo);
        parser->parse(xmlContent,localInstruction);

        if (instruction.mode != Mode::FIRSTPACKET && xmlContent.size() > 0) {
            localInstruction.mode = Mode::CHART;
            parser->configure(localInstruction);
            parser->executeParseloop(sliceNames);
            QPixmap map;
            chart.set(map, parser->getChart());

            QLabel *l = new QLabel();
            l->setPixmap(map);
            chartScene->setWidget(l);
            chartScene->setMaximumWidth(map.width());
        }
    }
    else if (instruction.mode == Mode::FIRSTPACKET)
        parser->configure(instruction);
}

DatabaseView::DatabaseView() {
    packetInfo = &tablePacketInfo;
}

/*--------------------------------------------------------------------------*/

StatisticsView::StatisticsView() {
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
    init();
    QString xmlContent;
    getContent(xmlContent);
    if (xmlContent.size() > 0) {
        instruction.mode = Mode::STATISTICS;
        instruction.statisticsColumn = COLUMN;
        parser->parse(xmlContent,instruction);
    }
    statisticsPacketInfo.evaluate(parser->getAppearance());
}

void StatisticsView::rewriteFileInfo() {
    init();

    instruction.mode = Mode::STATISTICS;
    instruction.statisticsColumn = COLUMN;
    parser->configure(instruction);
    parser->executeParseloop(sliceNames);
    statisticsPacketInfo.evaluate(parser->getAppearance());
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
    }
    else
        rewriteFileInfo();

}

TrafficView::TrafficView(TrafficPacketInfoPresenter::infoType type)
    : trafficPacketInfo(type) {
    packetInfo = &trafficPacketInfo;
    instruction.plotByteNumber = type == TrafficPacketInfoPresenter::infoType::BYTES;
    instruction.plotPacketNumber = type == TrafficPacketInfoPresenter::infoType::PACKETS;

    instruction.settings.shownColumns = 0;
    for (int i = 0; i < COLUMNNUMBER; i++)
        instruction.settings.showInfo[i] = false;

    instruction.mode = Mode::ALL;
}

void TrafficView::rewriteFileInfo() {
    init();
    parser->setPacketInfoPresenter(packetInfo);
    parser->configure(instruction);
    parser->executeParseloop(sliceNames);
}
