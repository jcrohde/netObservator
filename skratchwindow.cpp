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

#include "SkratchWindow.h"

MainWindow::MainWindow(Controller *c, Model *model, QWidget *parent)
    : controller(c), QMainWindow(parent)
{
    setWindowTitle(factory.getStringFromKey(stringKey::PROGRAMNAME));
    setWindowIcon(QIcon("icons/observerLogo.png"));

    menuBar = new MenuBar;
    setMenuBar(menuBar);
    model->server.registerObserver(menuBar);

    toolBar = new ToolBar;
    addToolBar(toolBar);
    model->server.registerObserver(toolBar);

    help = new HelpDialog;

    generateCenter(model);

    connectSignalsAndSlots(c);
}

void MainWindow::update(const sniffState &state) {
    if (!state.valid && state.sniffing)
        // In this case a user call is emulated to join the invalid sniff thread.
        emit runBut->clicked();
    else if (state.sniffing)
        updateSniffStart(state);
    else
        updateSniffStop(state);
}

void MainWindow::generateCenter(Model *model) {
    QVBoxLayout *vert = new QVBoxLayout;

    generateSniffThreadControll(vert,model->sniff.devs);

    overView = new DisplayTab(controller);
    model->server.registerObserver(overView);
    vert->addWidget(overView);

    center = new QDialog();
    center->setLayout(vert);
    this->setCentralWidget(center);
}

void MainWindow::generateSniffThreadControll(QVBoxLayout *vert, Devices &devs) {
    QHBoxLayout *horizontal = new QHBoxLayout;

    QLabel *devLabel = new QLabel("devices:");
    horizontal->addWidget(devLabel);

    devCombo=new QComboBox();
    QList<QString> deviceList;
    devs.getDeviceStrings(deviceList);
    for (int i = 0; i<deviceList.size(); i++) {
        devCombo->addItem(deviceList.at(i));
    }
    horizontal->addWidget(devCombo);

    horizontal->addSpacing(20);

    runBut = new QPushButton("Run");
    runBut->setIcon(QIcon("icons/run.png"));
    horizontal->addWidget(runBut);

    horizontal->addStretch();

    vert->addLayout(horizontal);
}

void MainWindow::connectSignalsAndSlots(Controller *c) {
    QObject::connect(menuBar,SIGNAL(sendCommand(CommandCode)),this,SLOT(execute(CommandCode)));
    QObject::connect(toolBar,SIGNAL(send(CommandCode)),this,SLOT(execute(CommandCode)));

    QObject::connect(menuBar->closeAction,SIGNAL(triggered()),this,SLOT(close()));

    QObject::connect(runBut, SIGNAL (clicked()),this, SLOT (handleRunButton()));
    QObject::connect(devCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(changeDevice(int)));

    QObject::connect(this->menuBar->hintAction,SIGNAL(sigCode(CommandCode)),this,SLOT(getHelp(CommandCode)));
    QObject::connect(this->menuBar->licenseAction,SIGNAL(sigCode(CommandCode)),this,SLOT(getHelp(CommandCode)));
    QObject::connect(this->menuBar->authorAction,SIGNAL(sigCode(CommandCode)),this,SLOT(getHelp(CommandCode)));

    QObject::connect(controller,SIGNAL(addTab()),overView,SLOT(add()));
}

void MainWindow::updateSniffStart(const sniffState &state) {
    runBut->setIcon(QIcon("icons/stop.png"));
    runBut->setText("Stop");
    overView->fixCurrentTab(true);
}

void MainWindow::updateSniffStop(const sniffState &state) {
    runBut->setText("Run");
    runBut->setIcon(QIcon("icons/run.png"));
    overView->fixCurrentTab(false);

    if (!state.valid)
        setErrorMessage(state.errorMessage, QMessageBox::Critical);
}

void MainWindow::execute(CommandCode command) {
    Command cmd;
    cmd.arguments.clear();
    cmd.code = command;

    controller->getCommand(cmd);
}

void MainWindow::getHelp(CommandCode code) {
    stringKey key = stringKey::HELP;

    if (code == CommandCode::LICENSE)
        key = stringKey::LICENSE;
    else if (code == CommandCode::ABOUT)
        key = stringKey::ABOUT;

    QString helpInfo = factory.getStringFromKey(key);
    help->setWindowTitle(factory.getTitleFromKey(key));
    help->load(helpInfo);
    help->show();
}

void MainWindow::changeDevice(int i) {
    Command cmd;
    cmd.arguments.append(QString::number(i));
    cmd.code = CommandCode::CHANGEDEVICE;
    controller->getCommand(cmd);
}

void MainWindow::handleRunButton() {
    Command cmd;
    cmd.arguments.append(QString::number(devCombo->currentIndex()));
    cmd.code = CommandCode::SNIFF;
    controller->getCommand(cmd);
}
