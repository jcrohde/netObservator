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

#ifndef DISPLAYTAB_H
#define DISPLAYTAB_H

#include <QTabWidget>
#include <QToolButton>
#include "modelview.h"
#include "controller.h"


struct Page : public QDialog {
    Q_OBJECT
public:
    modelView mv;

    Page() : mv() {
        QHBoxLayout *mainLayout = new QHBoxLayout();
        QVBoxLayout *pageLayout = new QVBoxLayout();

        QHBoxLayout *topLayout = new QHBoxLayout();
        sliceBox = new QComboBox();
        topLayout->addWidget(sliceBox);
        sliceBox->setVisible(false);
        pageLayout->addLayout(topLayout);

        pageLayout->addWidget(mv.view.tablePacketInfo.display.display);
        pageLayout->addWidget(new QLabel("Content of Selected Cell:"));
        cellContent = new QTextEdit;
        pageLayout->addWidget(cellContent);

        QScrollArea *scr = new QScrollArea();
        mv.view.dialog = this;
        QPixmap *map = new QPixmap(100, 100);
        HostChart chart;
        chart.set(*map, QHash<QString, Protocol>());
        QLabel *label = new QLabel;
        label->setPixmap(*map);
        scr->setWidget(label);
        scr->setMaximumWidth(map->width());
        mv.view.chartScene = scr;

        mainLayout->addWidget(scr);
        mainLayout->addLayout(pageLayout);
        setLayout(mainLayout);

        cellContent->setReadOnly(true);
        cellContent->setMaximumHeight(90);
        cellContent->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
        mv.view.tablePacketInfo.display.display->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QObject::connect(mv.view.tablePacketInfo.display.display,SIGNAL(clicked(const QModelIndex &)),this,SLOT(printContent(const QModelIndex &)));
    }

    void setSlicesVisible(QStringList sliceNames) {
        if (sliceNames != slices) {
            slices = sliceNames;
            sliceBox->setVisible(sliceNames.size() > 0);

            sliceBox->blockSignals(true);
            sliceBox->clear();
            for (int i = 0; i < sliceNames.size(); i++) {
                sliceBox->addItem(sliceNames[i]);
            }
            sliceBox->blockSignals(false);
            sliceBox->setEditable(true);
        }
    }

    QComboBox *sliceBox;

private:
    QTextEdit *cellContent;
    QStringList slices;
    //QLabel *sniffLabel;

private slots:
    void printContent(const QModelIndex &index) {
        myCellMutex.lock();
        if (index.row()< mv.view.tablePacketInfo.display.tableModel->rowCount())
            cellContent->setText(index.data().toString());
        myCellMutex.unlock();
    }
};

class DisplayTab : public QTabWidget, public serverObserver
{
    Q_OBJECT
public:
    DisplayTab(Controller *c);

    void update(const serverState &state);
    void fixCurrentTab(bool fix);

private:
    bool fixed;
    int fixIndex;

    Controller *controll;

    int displayCount;

    void updateController(int index);

public slots:
    void add();

private slots:
    void closeTab(int index);
    void setToCurrentTab(int index);
    void loadSlice(QString sliceName);
};

#endif // DISPLAYTAB_H
