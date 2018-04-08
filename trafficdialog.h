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

#ifndef TRAFFICDIALOG_H
#define TRAFFICDIALOG_H


#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include "util.h"
#include "view.h"
#include <QPixmap>

class TrafficDialog : public QDialog, public SniffObserver
{
    Q_OBJECT
public:
    explicit TrafficDialog(TrafficView *view, QWidget *parent = 0);

    void update(const sniffState & state);

protected:
    void paintEvent(QPaintEvent *event) {
        maxLabel->setText(QString::number(maximum));
        QDialog::paintEvent(event);
    }

    void showEvent(QShowEvent *e) {
        trafficView->rewriteInfo();
        QDialog::showEvent(e);
    }

    void closeEvent(QCloseEvent *e) {
        emit closeSignal();
        QDialog::closeEvent(e);
    }

private:
    QSpinBox *ticBox;
    int ticTime;
    long int ticBegin;

    int count;
    QPushButton *cancelBut;

    QPixmap *display;

    std::vector<int> storedValues;

    void generatePresentationSection(QHBoxLayout *settingLayout);
    void generateButtonSection(QVBoxLayout *ordering);
    void connectSignalsAndSlots();
    void evaluatePacketsOfSecond(int packetNumber, long int timeStamp);
    void fillPolygon(long int secondsPast,int beginValue,int endValue, QPainter &pain);
    void updateMaxima(int packetNumber, long int timeStamp);
    void redraw();

    QLabel *label;
    QLabel *maxLabel, *typeInfoLabel;
    int value, lastValue;
    long int lastTimeStamp;
    double verticalFactor;
    QPoint polygon[4];
    TrafficView *trafficView;

    QColor displayColor, displayLineColor;

    std::vector<std::pair<int, long int> > maxima;
    int maximum;

private slots:
    void getForPlot(int traffic);
    void setTic(int t) {ticTime = t; trafficView->rewriteInfo();}

signals:
    closeSignal();
    change(ViewComposition&);
};

#endif // TRAFFICDIALOG_H
