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

#include <ws2tcpip.h>
#include "packetinfopresenter.h"

/*--------------------------------------------------------------------------*/

TablePacketInfoPresenter::TablePacketInfoPresenter() : PacketInfoPresenter() {
    inBrush = QBrush(QColor(170,255,185));
    outBrush = QBrush(QColor(255,190,160));
    thirdBrush = QBrush(QColor(170,200,250));
    inGroupBrush = QBrush(QColor(85,255,93));
    outGroupBrush = QBrush(QColor(255,95,88));
    thirdGroupBrush = QBrush(QColor(130,150,252));
    inOtherBrush = QBrush(QColor(0,255,0));
    outOtherBrush = QBrush(QColor(255,0,0));
    thirdOtherBrush = QBrush(QColor(85,95,255));
}

void TablePacketInfoPresenter::show(QString *content) {
    evaluateTime(content[TIME]);
    show(content,determineDirection(content));
}

void TablePacketInfoPresenter::show(QString *content, colorCode direction) {
    setBackGround(direction);

    row = display.tableModel->rowCount();
    for (int i = 0; i < setting.shownColumns; i++) {
        showAttribute(i,content[i]);
    }
}

void TablePacketInfoPresenter::init() {
    display.tableModel->clear();
    display.tableModel->setRowCount(0);

    QStringList LabelList;

    for (int i = 0; i< COLUMNNUMBER; i++) {
        if (setting.showInfo[i]) LabelList.push_back(onLocalOrSrc(LABEL[i]));
    }

    display.tableModel->setHorizontalHeaderLabels(LabelList);
}

void TablePacketInfoPresenter::showAttribute(int column, QString attribute) {
    QStandardItem *messageItem = new QStandardItem(unEscape(attribute));
    messageItem->setBackground(*background);
    display.tableModel->setItem(row,column,messageItem);
}

TablePacketInfoPresenter::colorCode TablePacketInfoPresenter::determineDirection(QString *content) {
    if (content[DIRECTION] == "->" || content[DIRECTION] == "-&gt;" || content[DIRECTION] == "in") {
        if (content[PROTOCOL] == "TCP" || content[PROTOCOL] == "UDP")
            return colorCode::INCOMINGSING;
        else if (content[PROTOCOL] == "ICMP" || content[PROTOCOL] == "IGMP")
            return colorCode::INCOMINGGROUP;
        else
            return colorCode::INCOMINGOTHER;
    }
    else if (content[DIRECTION] == "out") {
        if (content[PROTOCOL] == "TCP" || content[PROTOCOL] == "UDP")
            return colorCode::OUTGOINGSING;
        else if (content[PROTOCOL] == "ICMP" || content[PROTOCOL] == "IGMP")
            return colorCode::OUTGOINGGROUP;
        else
            return colorCode::OUTGOINGOTHER;
    }
    else {
        if (content[PROTOCOL] == "TCP" || content[PROTOCOL] == "UDP")
            return colorCode::THIRDPARTYSING;
        else if (content[PROTOCOL] == "ICMP" || content[PROTOCOL] == "IGMP")
            return colorCode::THIRDPARTYGROUP;
        else
            return colorCode::THIRDPARTYOTHER;
    }
}

void TablePacketInfoPresenter::setBackGround(colorCode direction) {
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
    else if (direction == colorCode::OUTGOINGOTHER)
        background = &outOtherBrush;
    else if (direction == colorCode::THIRDPARTYSING)
        background = &thirdBrush;
    else if (direction == colorCode::THIRDPARTYGROUP)
        background = &thirdGroupBrush;
    else if (direction == colorCode::THIRDPARTYOTHER)
        background = &thirdOtherBrush;
}

void TablePacketInfoPresenter::evaluateTime(QString &time) {
    char time_string[20];

    long int currentTimeInSeconds = time.toInt();

    if (currentTimeInSeconds > 0) {
        struct tm* ptm;
        ptm = localtime (&currentTimeInSeconds);

        strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
        time = time_string;
    }
    else time = "null";
}

/*--------------------------------------------------------------------------*/

StatisticsPacketInfoPresenter::StatisticsPacketInfoPresenter() {
    COLUMN = PROTOCOL;
}

void StatisticsPacketInfoPresenter::show(QString *content) {
    if (appearance.find(content[COLUMN])!=appearance.end()) {
        appearance.at(content[COLUMN]).push_back(content[TIME].toLong());
    }
    else {
        std::vector<long int> v;
        v.push_back(content[TIME].toLong());
        appearance.insert(std::pair<QString,std::vector<long int> >(content[COLUMN], v));
    }

    long int timeStamp = content[TIME].toLong();
    if (timeBegin == 0)
        timeBegin = timeStamp;
    if (timeStamp > timeEnd)
        timeEnd = timeStamp;

    totalNumber++;
}

void StatisticsPacketInfoPresenter::init(column C) {
    init();

    COLUMN = C;

    QStringList LabelList;

    for (int i = 0; i< COLUMNNUMBER; i++) {
        if (i == C) {
            LabelList.push_back(onLocalOrSrc(LABEL[i]));
            break;
        }
    }
    LabelList.push_back("Packet Number");
    LabelList.push_back("Percent");

    display.tableModel->setHorizontalHeaderLabels(LabelList);
}

void StatisticsPacketInfoPresenter::init() {
    timeBegin = 0;
    timeEnd = 0;
    totalNumber = 0;
    appearance.clear();
    display.tableModel->clear();
    display.tableModel->setRowCount(0);
}

void StatisticsPacketInfoPresenter::evaluate(const std::map<QString,std::vector<long int> > &data) {
    std::vector<sizedPair> pairs;

    timeBegin = timeEnd = 0;
    totalNumber = 0;

    for (auto iter = data.begin(); iter != data.end(); ++ iter) {
        totalNumber += iter->second.size();
        if (timeEnd == 0) {
            timeBegin = iter->second[0];
            timeEnd = iter->second[iter->second.size()-1];
        }
        else {
            if (timeBegin > iter->second[0])
                timeBegin = iter->second[0];

            if (timeEnd < iter->second[iter->second.size()-1])
                timeEnd = iter->second[iter->second.size()-1];
        }
        sizedPair pair;
        pair.value.first = iter->first;
        pair.value.second = iter->second;
        pairs.push_back(pair);
    }

    std::sort(pairs.begin(),pairs.end());

    int size = pairs.size();
    for (int i = 0; i < size; i++) {
        QStandardItem *messageItem = new QStandardItem(unEscape(pairs[size-i-1].value.first));
        display.tableModel->setItem(i,0,messageItem);
        QStandardItem *mItem = new QStandardItem(QString::number(pairs[size-i-1].value.second.size()));
        display.tableModel->setItem(i,1,mItem);
        QStandardItem *pItem = new QStandardItem(QString::number(100.0*(double)pairs[size-i-1].value.second.size()/(double) totalNumber));
        display.tableModel->setItem(i,2,pItem);
    }

    visualize(pairs,timeBegin,timeEnd);
}

/*--------------------------------------------------------------------------*/

TrafficPacketInfoPresenter::TrafficPacketInfoPresenter(infoType type) :
    myType(type), PacketInfoPresenter()
{

}

void TrafficPacketInfoPresenter::show(QString *content) {
    packetTimeStamp = content[TIME].toLong();
    if (packetTimeStamp > timeStamp) {
        evaluatePacketsOfSecond(packetNumber,timeStamp);
        timeStamp = packetTimeStamp;
        if (myType == infoType::PACKETS)
            packetNumber = 1;
        else
            packetNumber = content[PAYLOAD].size();
    }
    else {
        if (myType == infoType::PACKETS)
            packetNumber++;
        else
            packetNumber += content[PAYLOAD].size();
    }
}

void TrafficPacketInfoPresenter::evaluateLastPackets() {
    evaluatePacketsOfSecond(packetNumber,timeStamp);    
}

void TrafficPacketInfoPresenter::init() {
    timeStamp = 0;
    packetNumber = 0;
    evaluatePacketsOfSecond(0,0);
}
