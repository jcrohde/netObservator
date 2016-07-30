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

    settings = new SettingsDialog();
    searchDialog = new SearchDialog();
    filterEditor = new PacketFilterEditor();
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

    QHBoxLayout *sliceLayout = new QHBoxLayout();
    sliceLabel = new QLabel("recently sniffed:");
    sliceLayout->addWidget(sliceLabel);
    sliceBox = new QComboBox();
    sliceLayout->addWidget(sliceBox);
    sliceButton = new QPushButton("Load");
    sliceLayout->addWidget(sliceButton);
    setSliceItemsVisible(false);
    sliceLayout->addStretch();

    vert->addLayout(sliceLayout);

    tableModel = new QStandardItemModel();
    display = new QTableView();
    display->setModel(tableModel);
    display->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStandardItemModel *tableModel2 = new QStandardItemModel();
    QTableView *display2 = new QTableView();
    display2->setModel(tableModel2);
    display2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    sniff = new SniffThread(devs);

    std::function<bool()> allowModify = [&]() {
        return !(sniffThreadRunning || isSomeDialogOpened());
    };
    overView = new DisplayTab(&controller,sniff,settings,searchDialog,allowModify);

    vert->addWidget(overView);

    center = new QDialog();
    center->setLayout(vert);
    this->setCentralWidget(center);
}

void MainWindow::registerObservers() {
    settings->registerObserver(sniff);
    settings->registerObserver(searchDialog);
    settings->registerObserver(overView);
}

void MainWindow::connectSignalsAndSlots() {
    connectMethod(menuBar->newAction,[&]()->bool {controller.clear(); return true;});
    connectMethod(menuBar->openAction,[&]()->bool {return controller.load();});
    connectMethod(menuBar->saveAsAction,[&]()->bool {return controller.saveFileAs();});
    connectMethod(menuBar->saveAction,[&]()->bool {return controller.saveFile();});
    connectMethod(menuBar->closeAction,[&]()->bool {return close();});

    connectMethod(menuBar->settingsAction,[&]()->bool {settings->show(); return true;});
    connectMethod(menuBar->searchAction,[&]()->bool {
                      searchDialog->setWindowFlags(searchDialog->windowFlags() | Qt::WindowStaysOnTopHint);
                      searchDialog->show();
                      return true;});
    connectMethod(menuBar->filterAction,[&]()->bool {filterEditor->show(); return true;});

    QObject::connect(searchDialog, SIGNAL (sig()),this, SLOT (startSearch()));
    QObject::connect(searchDialog,SIGNAL (browse(bool)),this, SLOT(changeText(bool)));

    QObject::connect(runBut, SIGNAL (clicked()),this, SLOT (handleRunButton()));

    QObject::connect(this->menuBar->hintAction,SIGNAL(sigCode(stringKey)),this,SLOT(getHelp(stringKey)));
    QObject::connect(this->menuBar->licenseAction,SIGNAL(sigCode(stringKey)),this,SLOT(getHelp(stringKey)));
    QObject::connect(this->menuBar->authorAction,SIGNAL(sigCode(stringKey)),this,SLOT(getHelp(stringKey)));

    QObject::connect(this->filterEditor,SIGNAL(commit(QString)),this,SLOT(getFilter(QString)));

    QObject::connect(this,SIGNAL(failed(QString)),this,SLOT(loose(QString)));

    QObject::connect(this->sliceButton,SIGNAL(clicked()),this,SLOT(loadSlice()));
}

void MainWindow::connectMethod(MethodAction *action, std::function<bool()> method) {
    action->setMethod(method);
    connect(action,SIGNAL(sigMethod(methodWrapper)),this,SLOT(execute(methodWrapper)));
}

void MainWindow::setSliceItemsVisible(bool visible) {
    sliceLabel->setVisible(visible);
    sliceBox->setVisible(visible);
    sliceButton->setVisible(visible);
}

bool MainWindow::isSniffingAllowed() {
    if (isSomeDialogOpened()) {
        setErrorMessage(QString("It is not allowed to sniff while a sub dialog is opened."),QMessageBox::Warning);
        return false;
    }

    return true;
}

bool MainWindow::isSomeDialogOpened() {
    return (controller.fileDialog.isVisible())
        || (searchDialog && searchDialog->isVisible())
        || (settings && settings->isVisible())
        || (filterEditor && filterEditor->isVisible());
}

void MainWindow::executeMethod(std::function<bool ()> func) {
    if (!func()) {
        setErrorMessage(QString("The selected baction did not perform successfully."),QMessageBox::Warning);
    }
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

void MainWindow::getHelp(stringKey key) {
    QString helpInfo = factory.getStringFromKey(key);
    help->setWindowTitle(factory.getTitleFromKey(key));
    help->load(helpInfo);
    help->show();
}

void MainWindow::loadSlice() {
    methodWrapper wrapper;
    wrapper.method = [&]()->bool {
        return controller.loadFile(sliceBox->itemText(sliceBox->currentIndex()));
    };
    execute(wrapper);
}

void MainWindow::startSearch() {
    executeMethod([&]()->bool {
        SearchCommand command;
        searchDialog->setCommand(command);

        return controller.search(command);
    });
}

void MainWindow::changeText(bool forward) {
    executeMethod([&]()->bool {return controller.changeText(forward);});
}

void MainWindow::loose(QString errorMsg) {
    handleRunButton();
    setErrorMessage(errorMsg, QMessageBox::Critical);
}

void MainWindow::handleRunButton() {
    if (sniffThreadRunning) {
        QString sliceName;
        int sliceNumber;
        std::set<ipAddress> threadAdresses;
        runBut->setText("Run");
        runBut->setIcon(QIcon("icons/run.png"));
        sniffThreadRunning = false;
        if (sniff->stop(sliceName, sliceNumber, threadAdresses)) {

            sliceBox->clear();
            for (int i = 1; i <= sliceNumber; i++) {
                sliceBox->addItem(sliceName + QString::number(i) + ".xml");
            }

            setSliceItemsVisible(true);


            QStringList filenames;
            for (int i = 1; i <= sliceNumber; i++) {
                filenames.append(sliceName + QString::number(i) + ".xml");
            }

            searchDialog->setFilesForSearch(filenames);
        }
        else {
            searchDialog->setTabToSearch();
            setSliceItemsVisible(false);
        }
        overView->setTabTitle("Sniffed",false);
        controller.clear();
        overView->setSniffing(false);
    }
    else if (isSniffingAllowed()) {
        sniffThreadRunning = true;
        controller.clear();
        overView->setTabTitle("Sniffing",false);
        overView->setSniffing(true);
        std::function<void(QString)> lambda = [&](QString error) {emit failed(error);};
        sniff->start(devCombo->currentIndex(),filter,lambda);
        runBut->setIcon(QIcon("icons/stop.png"));
        runBut->setText("Stop");
    }
}
