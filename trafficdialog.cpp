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

#include "trafficdialog.h"
#include <QGroupBox>
#include <QFrame>
#include <QPainter>

const int HEIGHT = 220;
const int WIDTH = 500;
const int GAP = 10;
const int DX = -5;

TrafficDialog::TrafficDialog(TrafficView *view, QWidget *parent)
    : trafficView(view),
      QDialog(parent)
{
    count = 0;

    if (view->getType() == TrafficPacketInfoPresenter::infoType::PACKETS) {
        displayLineColor = QColor(255,255,0);
        displayColor = QColor(125,125,0);
    }
    else {
        displayLineColor = QColor(255,0,255);
        displayColor = QColor(125,0,125);
    }

    setWindowTitle("Traffic");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    verticalFactor = 1000/(HEIGHT-2*GAP);

    QVBoxLayout *ordering = new QVBoxLayout();

    QHBoxLayout *top = new QHBoxLayout;

    typeInfoLabel = new QLabel(QString("Number of ") + (view->getType() == TrafficPacketInfoPresenter::infoType::PACKETS ? "Packets" : "Bytes"));

    top->addWidget(typeInfoLabel);

    top->addStretch();

    top->addWidget(new QLabel("Seconds per Tick:"));

    ticBox = new QSpinBox();
    ticBox->setRange(1,60);
    ticBox->setValue(1);
    ticTime = 1;
    top->addWidget(ticBox);

    top->addWidget(new QLabel("Max ="));

    maxLabel = new QLabel();
    QPalette palette = maxLabel->palette();
    palette.setColor(maxLabel->backgroundRole(),Qt::black);
    if (view->getType() == TrafficPacketInfoPresenter::infoType::PACKETS)
        palette.setColor(maxLabel->foregroundRole(),Qt::yellow);
    else
        palette.setColor(maxLabel->foregroundRole(),QColor(255,0,255));
    maxLabel->setPalette(palette);
    maxLabel->setAutoFillBackground(true);


    top->addWidget(maxLabel);
    ordering->addLayout(top);

    label = new QLabel();

    display = new QPixmap(WIDTH,HEIGHT);

    QPainter *p = new QPainter(display);
    p->fillRect(display->rect(),Qt::black);
    p->end();

    label->setPixmap(*display);

    ordering->addWidget(label);

    view->setOutputDevice([&](int packetNumber,long int timeStamp){evaluatePacketsOfSecond(packetNumber,timeStamp);});

    generateButtonSection(ordering);

    setLayout(ordering);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connectSignalsAndSlots();
}

void TrafficDialog::update(const sniffState & state) {
    ticBox->blockSignals(true);
    if (state.sniffing)
    {
        ticBox->setValue(1);
        ticTime = 1;
    }
    ticBox->setDisabled(state.sniffing);
    ticBox->blockSignals(false);
}

void TrafficDialog::generatePresentationSection(QHBoxLayout *settingLayout) {

}

void TrafficDialog::generateButtonSection(QVBoxLayout *ordering) {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addStretch();

    cancelBut = new QPushButton("Close");
    buttonLayout->addWidget(cancelBut);

    ordering->addLayout(buttonLayout);

}

void TrafficDialog::evaluatePacketsOfSecond(int packetNumber, long timeStamp) {
    long int numberOfPastTics = 1;

    if (lastTimeStamp == 0) {
        ticBegin = timeStamp;
        storedValues.push_back(0);
    }
    else if (ticBegin > 0) {
        numberOfPastTics = (timeStamp-ticBegin)/ticTime;
        ticBegin = ticBegin + numberOfPastTics*ticTime;
    }

    if (!numberOfPastTics) {
        storedValues[storedValues.size()-1] += packetNumber;
        packetNumber = storedValues[storedValues.size()-1];
    }

    updateMaxima(packetNumber,timeStamp);

    if (timeStamp == 0) {
        QPainter p;
        p.begin(display);
        p.fillRect(display->rect(),Qt::black);
        p.end();
        label->setPixmap(*display);
        storedValues.clear();
        storedValues.push_back(0);
        ticBegin = 0;
        lastValue = 0;
        if (count != 0) {
            count = 0;
        }
    }
    else {
        QPainter pain;
        pain.begin(display);

        if (numberOfPastTics > 0) {
            pain.drawPixmap(numberOfPastTics*DX,0,*display);
            pain.setPen(Qt::black);
            pain.fillRect(WIDTH+numberOfPastTics*DX,0,WIDTH,HEIGHT,Qt::black);

            if (numberOfPastTics > 1) {
                fillPolygon(numberOfPastTics,storedValues[storedValues.size()-1]/verticalFactor,0,pain);
                pain.setPen(displayLineColor);
                pain.drawLine(WIDTH+(numberOfPastTics-1)*DX-1,HEIGHT-1,WIDTH+DX-1,HEIGHT-1);
                for (int i = 0; i < numberOfPastTics -1; i++)
                    storedValues.push_back(0);
            }

            storedValues.push_back(packetNumber);
            if (storedValues.size() >100)
                storedValues.erase(storedValues.begin());
        }

        value = packetNumber/verticalFactor;

        fillPolygon(1,storedValues[storedValues.size()-2]/verticalFactor,value,pain);

        pain.end();

        label->setPixmap(*display);

    }
    lastTimeStamp = timeStamp;
}

void TrafficDialog::updateMaxima(int packetNumber, long timeStamp) {
    if (timeStamp == 0) {
        maxima.clear();

        maximum = 0;
        verticalFactor = 1;
        return;
    }

    while (maxima.size() > 0 && maxima[0].second < timeStamp-100*ticTime) {
        maxima.erase(maxima.begin());
    }

    while (maxima.size() > 0 && maxima[maxima.size()-1].first < packetNumber) {
        maxima.erase(maxima.begin()+maxima.size()-1);
    }

    maxima.push_back(std::pair<int, long int>(packetNumber,timeStamp));

    if (maximum != maxima[0].first) {
        int vf = maxima[0].first/(HEIGHT-2*GAP)+1;
        if (vf != verticalFactor) {
            if (vf > verticalFactor) {
                int factor = vf/verticalFactor+1;
                vf = factor*verticalFactor;
                lastValue/=factor;
                verticalFactor = vf;

                redraw();
            }
            else {
                int factor = verticalFactor/vf;
                if (factor > 1) {
                    lastValue*=factor;
                    verticalFactor/=factor;

                    redraw();
                }
            }
        }


        maximum = maxima[0].first;
    }
}

void TrafficDialog::fillPolygon(long int secondsPast, int beginValue, int endValue, QPainter &pain) {
    QPolygon poly(4);

    polygon[0].setX(WIDTH+secondsPast*DX-1);
    polygon[1].setX(WIDTH+secondsPast*DX-1);
    polygon[2].setX(WIDTH+(secondsPast-1)*DX-1);
    polygon[3].setX(WIDTH+(secondsPast-1)*DX-1);

    polygon[0].setY(HEIGHT-beginValue-1);
    polygon[1].setY(HEIGHT-1);
    polygon[2].setY(HEIGHT-1);
    polygon[3].setY(HEIGHT-1-endValue);

    poly.setPoint(0,polygon[0]);
    poly.setPoint(1,polygon[1]);
    poly.setPoint(2,polygon[2]);
    poly.setPoint(3,polygon[3]);

    QPainterPath path;
    path.addPolygon(poly);
    pain.setPen(displayColor);
    pain.setBrush(displayColor);
    pain.drawPolygon(poly);

    pain.setPen(displayLineColor);
    pain.drawLine(WIDTH+secondsPast*DX-1,HEIGHT-1-beginValue,WIDTH+(secondsPast-1)*DX-1,HEIGHT-1-endValue);
}

void TrafficDialog::redraw() {
    QPainter p;
    p.begin(display);
    p.fillRect(display->rect(),Qt::black);

    int size = storedValues.size()-1;
    for (int i = 0; i < size; i++) {
        fillPolygon(size-i,storedValues[i]/verticalFactor,storedValues[i+1]/verticalFactor,p);
    }
    p.end();
}

void TrafficDialog::connectSignalsAndSlots() {
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(close()));
    connect(ticBox,SIGNAL(valueChanged(int)),this,SLOT(setTic(int)));
}

void TrafficDialog::getForPlot(int traffic ) {
    if (isVisible()) {
        evaluatePacketsOfSecond(traffic,count);
        count++;
    }
}
