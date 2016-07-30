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

#include "packetinfopresenter.h"

void PacketInfoPresenter::show(QString *content) {
    show(content,determineDirection(content));
}

PacketInfoPresenter::colorCode PacketInfoPresenter::determineDirection(QString *content) {
    if (content[DIRECTION] == "->" || content[DIRECTION] == "-&gt;" || content[DIRECTION] == "in") {
        if (content[PROTOCOL] == "TCP" || content[PROTOCOL] == "UDP")
            return colorCode::INCOMINGSING;
        else if (content[PROTOCOL] == "ICMP" || content[PROTOCOL] == "IGMP")
            return colorCode::INCOMINGGROUP;
        else
            return colorCode::INCOMINGOTHER;
    }
    else
        if (content[PROTOCOL] == "TCP" || content[PROTOCOL] == "UDP")
            return colorCode::OUTGOINGSING;
        else if (content[PROTOCOL] == "ICMP" || content[PROTOCOL] == "IGMP")
            return colorCode::OUTGOINGGROUP;
        else
            return colorCode::OUTGOINGOTHER;
}

TablePacketInfoPresenter::TablePacketInfoPresenter() : PacketInfoPresenter() {
    inBrush = QBrush(QColor(170,255,185));
    outBrush = QBrush(QColor(250,190,170));
    inGroupBrush = QBrush(QColor(85,255,93));
    outGroupBrush = QBrush(QColor(252,95,85));
    inOtherBrush = QBrush(QColor(0,255,0));
    outOtherBrush = QBrush(QColor(255,0,0));
}

void TablePacketInfoPresenter::show(QString *content, colorCode direction) {
    if (direction == colorCode::INCOMINGSING)
        background = &inBrush;
    else if (direction == colorCode::OUTGOINGSING)
        background = &outBrush;
    else if (direction == colorCode::INCOMINGGROUP)
        background = &inGroupBrush;
    else if (direction == colorCode::OUTGOINGGROUP)
        background = &outGroupBrush;
    else if (direction == colorCode::INCOMINGOTHER)
        background = &inOtherBrush;
    else
        background = &outOtherBrush;

    column = 0;
    row = display.tableModel->rowCount();
    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (setting.showInfo[i]) {
            showAttribute(column,content[i]);
            column++;
        }
    }
}

void TablePacketInfoPresenter::init() {
    display.tableModel->clear();
    display.tableModel->setRowCount(0);

    QStringList LabelList;

    for (int i = 0; i< COLUMNNUMBER; i++) {
        if (SettingObserver::setting.showInfo[i]) LabelList.push_back(LABEL[i]);
    }

    display.tableModel->setHorizontalHeaderLabels(LabelList);
}

void TablePacketInfoPresenter::showAttribute(int column, QString attribute) {
    QStandardItem *messageItem = new QStandardItem(unEscape(attribute));
    messageItem->setBackground(*background);
    display.tableModel->setItem(row,column,messageItem);
}

TextPacketInfoPresenter::TextPacketInfoPresenter() {
    display = new QTextEdit();

    inString = "005500";
    outString = "550000";
    inGroupString = "00AA00";
    outGroupString = "AA0000";
    inOtherString = "00FF00";
    outOtherString = "FF0000";
}

void TextPacketInfoPresenter::show(QString *content, colorCode direction) {
    displayText += "<font color=\"#";

    if (direction == colorCode::INCOMINGSING)
        displayText += inString;
    else if (direction == colorCode::OUTGOINGSING)
        displayText += outString;
    else if (direction == colorCode::INCOMINGGROUP)
        displayText += inGroupString;
    else if (direction == colorCode::OUTGOINGGROUP)
        displayText += outGroupString;
    else if (direction == colorCode::INCOMINGOTHER)
        displayText += inOtherString;
    else
        displayText += outOtherString;

    displayText += "\">";

    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (setting.showInfo[i]) {

            if (!(i == ENTIRE_PACKET || i== PAYLOAD))
                displayText += " ";
            else
                displayText += "<br>";

            displayText += LABEL[i] + " = " + content[i];
        }
    }
    displayText += "</font><br>";
}

void TextPacketInfoPresenter::init() {
    display->clear();
    displayText.clear();
}
