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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QFileDialog>
#include "searchdialog.h"
#include "settingsdialog.h"
#include "packetfiltereditor.h"
#include "viewdialog.h"
#include "helpdialog.h"
#include "modelview.h"
#include "devices.h"
#include "sniffthread.h"
#include "trafficdialog.h"
#include <memory>

class Controller : public QWidget
{
    Q_OBJECT
public:
    Controller(Model *m);

    void setModelView(modelView *mv);

private:
    Model *model;
    QString lastSavedFileName;

    SettingsDialog *settings;
    StatisticsDialog *statisticsDialog;
    TrafficDialog *packetTrafficDialog, *byteTrafficDialog;
    SearchDialog *searchTabDialog;
    SearchDialog *searchFilesDialog;
    PacketFilterEditor *filterEditor;
    QFileDialog fileDialog;

    SearchOnTabStrategy tabStrategy;
    SearchOnFilesStrategy filesStrategy;

    StatisticsView statisticsView;
    TrafficView packetTrafficView, byteTrafficView;

    void executeCommand(Command cmd);
    bool isSomeDialogOpened();

    void load();
    void saveFile();
    void saveFileAs();
    void handleSniffing(Command cmd);

public slots:
    void getCommand(Command command);

private slots:
    void search(SearchCommand &command);
    void changeText(bool forward);
    void getSettings(Settings &set);
    void getComposition(ViewComposition &composition);
    void getFilter(QString str) {model->sniff.setFilter(str);}
    void removeStatisticsView() {model->view.attachStatistics(false);}
    void removePacketView() {model->server.unregisterObserver(&packetTrafficView);}
    void removeByteView() {model->server.unregisterObserver(&byteTrafficView);}

signals:
    void packets(int);
    void bytes(int);
};

#endif // CONTROLLER_H
