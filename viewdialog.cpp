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

#include "viewdialog.h"
#include <QPainter>
#include <QGroupBox>
#include <QFrame>

static const int WIDTH = 400;
static const int GAP = 50;
static const int HDIFF = 20;
static const int FAT = 5;


StatisticsDialog::StatisticsDialog(StatisticsView *statistics, QWidget *parent)
    : statisticsView(statistics),
      QDialog(parent)
{
    setWindowTitle("Statistics");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    statisticsView->statisticsPacketInfo.visualize = [&](std::vector<sizedPair> &results, long int begin, long int end) {
        visualize(results, begin, end);
    };

    QVBoxLayout *ordering = new QVBoxLayout();

    selection = new QComboBox();
    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (i != TIME && i != ENTIRE_PACKET && i != PAYLOAD)
            selection->addItem(onLocalOrSrc(LABEL[i]));
    }

    ordering->addWidget(selection);

    QHBoxLayout *middlePart = new QHBoxLayout;

    middlePart->addWidget(statisticsView->packetInfo->display.display);

    statisticsView->packetInfo->display.display->setMinimumWidth(350);

    QVBoxLayout *timeLayout = new QVBoxLayout;

    timeLineTitle = new QLabel("Time Lines");
    timeLayout->addWidget(timeLineTitle);

    label = new QLabel();

    scr = new QScrollArea;

    map = new QPixmap(300,400);
    QPainter p;
    p.begin(map);
    p.fillRect(map->rect(),Qt::black);
    p.end();
    label->setPixmap(*map);

    scr->setBackgroundRole(QPalette::Dark);
    scr->setWidget(label);
    timeLayout->addWidget(scr);

    middlePart->addLayout(timeLayout);

    ordering->addLayout(middlePart);

    generateButtonSection(ordering);

    setLayout(ordering);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connectSignalsAndSlots();
}

void StatisticsDialog::generateButtonSection(QVBoxLayout *ordering) {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addStretch();

    cancelBut = new QPushButton("Close");
    buttonLayout->addWidget(cancelBut);

    ordering->addLayout(buttonLayout);
}

void StatisticsDialog::changeView(int index) {
    statisticsView->update((column) (index+1));
}

void StatisticsDialog::visualize(std::vector<sizedPair> &results, long int begin, long int end) {
    timeLineTitle->setText("Time Lines Sniffed from " + evaluateTimeStamp(begin) + " till " + evaluateTimeStamp(end));
    long int duration = end-begin;
    int size = results.size();

    int widht = std::max((long int) WIDTH,FAT*duration+2*GAP);
    int height = std::max(300, HDIFF*(size+1));

    map = new QPixmap(widht,height);
    label = new QLabel;

    QPainter paint;
    paint.begin(map);
    paint.fillRect(map->rect(),Qt::black);
    paint.setPen(Qt::white);
    paint.setBrush(QColor(255,140,0));
    int h;
    std::vector<long int> row;
    int rowSize;
    long int curVal,lastValue,lastBegin,xBegin,xEnd;

    for (int i = 0; i < size; i++) {
        h = HDIFF*(i+1);
        row = results[size-i-1].value.second;
        rowSize = row.size();
        lastValue = lastBegin = 0;

        paint.drawText(GAP/3,h+4,QString::number(i+1));
        paint.drawLine(GAP-2,h,GAP+FAT*(end-begin)+1,h);

        for (int j = 0; j < rowSize; j++) {
            curVal = row[j];
            if (curVal -lastValue > 2) {
                if (lastBegin > 0) {
                    xBegin = GAP+FAT*(lastBegin-begin);
                    xEnd = GAP+FAT*(lastValue-begin);
                    paint.drawRect(xBegin-2,h-2,xEnd-xBegin+3,5);
                }
                lastBegin = curVal;
            }
            lastValue = curVal;
        }

        xBegin = GAP+FAT*(lastBegin-begin);
        xEnd = GAP+FAT*(lastValue-begin);
        paint.drawRect(xBegin-2,h-2,xEnd-xBegin+3,5);
    }
    paint.end();
    label->setPixmap(*map);
    scr->setWidget(label);
}

QString StatisticsDialog::evaluateTimeStamp(long int timeStamp) {
    char time_string[20];
    struct tm* ptm;
    ptm = localtime (&timeStamp);

    strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
    return QString(time_string);
}

void StatisticsDialog::connectSignalsAndSlots() {
    connect(selection,SIGNAL(currentIndexChanged(int)),this,SLOT(changeView(int)));
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(close()));
}
