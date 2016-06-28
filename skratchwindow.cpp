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

#include <QVBoxLayout>
#include <QDebug>
#include "SkratchWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(factory.getStringFromKey(stringKey::PROGRAMNAME));
    setWindowIcon(QIcon("icons/observerLogo.png"));

    menuBar = new MenuBar;
    setMenuBar(menuBar);

    generateDialogs();

    generateCenter();

    registerObservers();

    connectSignalsAndSlots();
}

void MainWindow::generateDialogs() {
    help = new HelpDialog();

    fileDialog = new QFileDialog;

    settings = new SettingsDialog();
    searchDialog = new SearchDialog();
    settings->registerObserver(searchDialog);
}

void MainWindow::generateCenter() {
    sniffThreadRunning = false;

    QVBoxLayout *vert = new QVBoxLayout;
    QHBoxLayout *hori = new QHBoxLayout;

    QLabel *devLabel = new QLabel("devices:");
    hori->addWidget(devLabel);

    devCombo=new QComboBox();
    devs.getDeviceStrings(deviceList);
    for (int i = 0; i<deviceList.size(); i++) {
        devCombo->addItem(deviceList.at(i));
    }
    hori->addWidget(devCombo);

    hori->addSpacing(20);

    runBut = new QPushButton("Run");
    runBut->setIcon(QIcon("icons/run.png"));
    hori->addWidget(runBut);

    hori->addStretch();

    vert->addLayout(hori);

    tableModel = new QStandardItemModel();
    display = new QTableView();
    display->setModel(tableModel);
    display->setEditTriggers(QAbstractItemView::NoEditTriggers);

    vert->addWidget(display);

    center = new QDialog();
    center->setLayout(vert);
    this->setCentralWidget(center);

    view = new View(tableModel);
    view->init();

    sniff = new SniffThread(tableModel,devs);
}

void MainWindow::registerObservers() {
    server.registerObserver(searchDialog);
    server.registerObserver(view);

    settings->registerObserver(view);
    settings->registerObserver(sniff);
}

void MainWindow::connectSignalsAndSlots() {
    connectMethod(menuBar->newAction,[&]()->bool {server.clear(); return true;});
    connectMethod(menuBar->openAction,[&]()->bool {return this->load();});
    connectMethod(menuBar->saveAsAction,[&]()->bool {return this->saveFileAs();});
    connectMethod(menuBar->saveAction,[&]()->bool {return this->saveFile();});
    connectMethod(menuBar->closeAction,[&]()->bool {return this->close();});

    connectMethod(menuBar->settingsAction,[&]()->bool {return this->showSettingsDialog();});
    connectMethod(menuBar->searchAction,[&]()->bool {return this->showSearchDialog();});

    QObject::connect(searchDialog, SIGNAL (sig()),this, SLOT (startSearch()));
    QObject::connect(searchDialog,SIGNAL (browse(bool)),this, SLOT(changeText(bool)));

    QObject::connect(runBut, SIGNAL (clicked()),this, SLOT (handleRunButton()));

    QObject::connect(this->menuBar->hintAction,SIGNAL(sigCode(stringKey)),this,SLOT(getHelp(stringKey)));
    QObject::connect(this->menuBar->licenseAction,SIGNAL(sigCode(stringKey)),this,SLOT(getHelp(stringKey)));
    QObject::connect(this->menuBar->authorAction,SIGNAL(sigCode(stringKey)),this,SLOT(getHelp(stringKey)));
}

void MainWindow::connectMethod(MethodAction *action, std::function<bool()> method) {
    action->setMethod(method);
    connect(action,SIGNAL(sigMethod(methodWrapper)),this,SLOT(execute(methodWrapper)));
}

bool MainWindow::load() {
    QString filename = fileDialog->getOpenFileName(
        this,
        tr("Open Document"),
        QDir::currentPath(),
        tr("XML (*.xml)") );

    bool valid = true;

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());
        server.load(content);
    }
    else valid = (filename.size() == 0);

    return valid;
}

bool MainWindow::save() {
    QFile file(fileName);

    QString content = server.getContent();
    if (content.size() == 0) {
        setErrorMessage(QString("No sniffed information to save."),QMessageBox::Critical);
        return true;
    }

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(content.toUtf8());
        return true;
    }

    return false;
}

bool MainWindow::saveFileAs() {
    fileName = fileDialog->getSaveFileName(this,tr("Save"),"Capture",tr("XML (*.xml)"));
    if(fileName == "") return true;
    else return save();
}

bool MainWindow::saveFile() {
    if (fileName == "") return saveFileAs();
    else return save();
}

bool MainWindow::showSettingsDialog() {
    settings->show();
    return true;
}

bool MainWindow::showSearchDialog() {
    searchDialog->setWindowFlags(searchDialog->windowFlags() | Qt::WindowStaysOnTopHint);
    searchDialog->show();
    return true;
}

bool MainWindow::isSniffingAllowed() {
    if (isSomeDialogOpened()) {
        setErrorMessage(QString("It is not allowed to sniff while a sub dialog is opened."),QMessageBox::Warning);
        return false;
    }

    return true;
}

bool MainWindow::isSomeDialogOpened() {
    return (fileDialog != nullptr && fileDialog->isVisible())
        || (searchDialog != nullptr && searchDialog->isVisible())
        || (settings != nullptr && settings->isVisible());
}

void MainWindow::executeMethod(std::function<bool ()> func) {
    if (!func()) {
        setErrorMessage(QString("The selected action did not perform successfully."),QMessageBox::Warning);
    }

    server.notifyObservers();
}

void MainWindow::getHelp(stringKey key) {
    QString helpInfo = factory.getStringFromKey(key);
    help->setWindowTitle(factory.getTitleFromKey(key));
    help->load(helpInfo);
    help->show();
}

void MainWindow::execute(methodWrapper func) {
    if (sniffThreadRunning) {
        setErrorMessage(QString("The selected action cannot be performed while the program is sniffing."),QMessageBox::Warning);
    }
    else if (isSomeDialogOpened()) {
        setErrorMessage(QString("The selected action cannot be performed while some other sub dialog is opened."),QMessageBox::Warning);
    }
    else
        executeMethod(func.method);
}

void MainWindow::startSearch() {
    executeMethod([&]()->bool {
        SearchCommand command;
        searchDialog->setCommand(command);

        return server.search(command);
    });
}

void MainWindow::changeText(bool forward) {
    executeMethod([&]()->bool {server.changeText(forward); return true;});
}

void MainWindow::handleRunButton() {
    if (sniffThreadRunning) {
        QString xmlContent;

        runBut->setText("Run");
        runBut->setIcon(QIcon("icons/run.png"));
        sniffThreadRunning = false;
        sniff->stop(xmlContent);
        server.load(xmlContent);
        server.notifyObservers();
    }
    else if (isSniffingAllowed()) {
        sniffThreadRunning = true;
        view->init();
        sniff->start(devCombo->currentIndex());
        runBut->setIcon(QIcon("icons/stop.png"));
        runBut->setText("Stop");
    }
}
