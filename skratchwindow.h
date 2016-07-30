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

#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QFileDialog>
#include "view.h"
#include "settingsdialog.h"
#include "searchdialog.h"
#include "packetfiltereditor.h"
#include "menubar.h"
#include "model.h"
#include "sniffthread.h"
#include "helpdialog.h"
#include "displaytab.h"
#include "controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() {QString xmlContent; int i; std::set<ipAddress> addr; sniff->stop(xmlContent,i,addr); delete sniff; /*delete view;*/}

private:
    MenuBar *menuBar;
    QDialog *center;

    SettingsDialog *settings;
    SearchDialog *searchDialog;
    PacketFilterEditor *filterEditor;
    HelpDialog *help;

    DisplayTab *overView;

    StringFactory factory;
    Controller controller;

    QString filter;

    QLabel *sliceLabel;
    QComboBox *sliceBox;
    QPushButton *sliceButton;

    QComboBox *devCombo;

    QStandardItemModel *tableModel;
    QTableView *display;
    QPushButton *runBut;

    QList<QString> deviceList;

    bool sniffThreadRunning;

    Devices devs;
    SniffThread *sniff;

    void generateDialogs();
    void generateCenter();
    void registerObservers();
    void connectSignalsAndSlots();
    void connectMethod(MethodAction *action, std::function<bool()> method);

    void setSliceItemsVisible(bool visible);

    bool isSniffingAllowed();
    bool isSomeDialogOpened();
    void executeMethod(std::function<bool()> func);

private slots:
    void execute(methodWrapper func);

    void getHelp(stringKey key);

    void loadSlice();

    void startSearch();
    void changeText(bool forward);

    void handleRunButton();

    void getFilter(QString str) {filter = str;}

    void loose(QString errorMsg);

signals:
    void failed(QString);
};

#endif // MainWindow_H
