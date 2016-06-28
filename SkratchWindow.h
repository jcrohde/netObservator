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
#include "menubar.h"
#include "model.h"
#include "sniffthread.h"
#include "helpdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() {QString xmlContent; sniff->stop(xmlContent); delete sniff; delete view;}

private:
    MenuBar *menuBar;
    QDialog *center;

    QFileDialog *fileDialog;
    SettingsDialog *settings;
    SearchDialog *searchDialog;
    HelpDialog *help;

    StringFactory factory;

    QString fileName;

    QComboBox *devCombo;

    QStandardItemModel *tableModel;
    QTableView *display;
    QPushButton *runBut;
    View *view;

    QList<QString> deviceList;

    bool sniffThreadRunning;

    Devices devs;
    XmlServer server;
    SniffThread *sniff;

    void generateDialogs();
    void generateCenter();
    void registerObservers();
    void connectSignalsAndSlots();
    void connectMethod(MethodAction *action, std::function<bool()> method);

    bool load();
    bool save();
    bool saveFileAs();
    bool saveFile();

    bool showSettingsDialog();
    bool showSearchDialog();

    bool isSniffingAllowed();
    bool isSomeDialogOpened();

    void executeMethod(std::function<bool()> func);

private slots:
    void getHelp(stringKey key);

    void execute(methodWrapper func);

    void startSearch();
    void changeText(bool forward);
    void handleRunButton();
};

#endif // MainWindow_H
