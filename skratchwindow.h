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
#include "menubar.h"
#include "model.h"
#include "displaytab.h"
#include "controller.h"

class MainWindow : public QMainWindow, public SniffObserver
{
    Q_OBJECT
public:
    explicit MainWindow(Controller *c, Model *model, QWidget *parent = 0);
    ~MainWindow() {}

    void update(const sniffState &state);
private:
    MenuBar *menuBar;
    QDialog *center;

    StringFactory factory;
    HelpDialog *help;

    DisplayTab *overView;
    Controller *controller;

    /*QLabel *sliceLabel;
    QComboBox *sliceBox;
    QPushButton *sliceButton;*/

    QComboBox *devCombo;
    QPushButton *runBut;

    void generateCenter(Model *model);
    void generateSniffThreadControll(QVBoxLayout *vert, Devices &devs);
    //void generateSliceSection(QVBoxLayout *vert);
    void connectSignalsAndSlots();

    void updateSniffStart(const sniffState &state);
    void updateSniffStop(const sniffState &state);
    //void setSliceItemsVisible(bool visible);

private slots:
    void execute(CommandCode command);
    void getHelp(CommandCode code);
    //void loadSlice();
    void handleRunButton();
};

#endif // MainWindow_H
