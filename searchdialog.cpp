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


#include "searchdialog.h"
#include "dnssingleton.h"

SearchDialog::SearchDialog(SearchStrategy *strtgy, QWidget *parent)
    : strategy(strtgy), QDialog(parent)
{
    setWindowIcon(QIcon("icons/observerLogo.png"));
    setWindowTitle(strategy->title());

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *flagLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QGridLayout *centralLayout = new QGridLayout();
    QLabel *columnLabel = new QLabel("in Column:");
    QLabel *findLabel = new QLabel("Text:");

    columnBox = new QComboBox();
    findEdit = new QLineEdit();

    invertBox = new QCheckBox();
    invertBox->setText("invert match");

    modeBox = new QComboBox();
    modeBox->addItem("case insensitive");
    modeBox->addItem("case sensitive");
    modeBox->addItem("regular expression");

    strategy->updateDisplay = [&] (const std::set<ipAddress> &addresses) {updateTableModel(addresses);};

    findBut = new QPushButton("Find");
    closeBut = new QPushButton("Close");

    centralLayout->addWidget(findLabel,0,0,1,1);
    centralLayout->addWidget(findEdit,0,1,1,1);
    centralLayout->addWidget(columnLabel,1,0,1,1);
    centralLayout->addWidget(columnBox,1,1,1,1);

    flagLayout->addWidget(invertBox);
    flagLayout->addWidget(modeBox);

    tableModel = new QStandardItemModel();
    display = new QTableView();
    display->setModel(tableModel);
    display->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList LabelList;
    LabelList.append(LABEL[HOSTADDRESS]);
    LabelList.append(LABEL[HOSTNAME]);
    tableModel->setHorizontalHeaderLabels(LabelList);

    buttonLayout->addStretch();
    buttonLayout->addWidget(findBut);
    buttonLayout->addWidget(closeBut);

    mainLayout->addLayout(centralLayout);
    mainLayout->addLayout(flagLayout);
    mainLayout->addWidget(new QLabel("by Clicking on a Host:"));
    mainLayout->addWidget(display);

    mainLayout->addLayout(strategy);
    mainLayout->addLayout(new QHBoxLayout());
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    Qt::WindowFlags flags = this->windowFlags();
    this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);

    connect(findBut,SIGNAL(clicked()),this,SLOT(change()));
    connect(closeBut,SIGNAL(clicked()),this,SLOT(close()));

    connect(display,SIGNAL(clicked(const QModelIndex &)),this,SLOT(setHost(const QModelIndex &)));
}

void SearchDialog::setCommand(SearchCommand &command) {
    command.settings = setting;
    command.column = columnBox->currentIndex();
    command.searchString = findEdit->text();
    command.invertMatch = invertBox->isChecked();
    command.mode = (SearchCommand::Mode) modeBox->currentIndex();

    strategy->complete(command);
}


void SearchDialog::updateTableModel(const std::set<ipAddress> &addresses) {
    addressItem item(ipAddress(),"name",0);
    int row = 0;

    tableModel->clear();

    QStringList LabelList;
    LabelList.append(LABEL[HOSTADDRESS]);
    LabelList.append(LABEL[HOSTNAME]);
    tableModel->setHorizontalHeaderLabels(LabelList);

    for (std::set<ipAddress>::iterator iter = addresses.begin(); iter != addresses.end(); ++iter) {
        item.address = *iter;
        DNSsingleton::getInstance().handle(item);
        QStandardItem *messageItem = new QStandardItem(item.address.toQString());
        tableModel->setItem(row,0,messageItem);
        if (!setting.showInfo[HOSTADDRESS])
            messageItem->setFlags(Qt::NoItemFlags);
        QStandardItem *messageItem2 = new QStandardItem(item.hostname);
        tableModel->setItem(row,1,messageItem2);
        if (!setting.showInfo[HOSTNAME])
            messageItem2->setFlags(Qt::NoItemFlags);

        row++;
    }
}

void SearchDialog::enlistColumnBox() {
    columnBox->clear();

    int count = 0;
    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (i == HOSTADDRESS)
            addressIndex = count;
        if (i == HOSTNAME)
            nameIndex = count;
        if (setting.showInfo[i]) {
            columnBox->addItem(onLocalOrSrc(LABEL[i]));
            count++;
        }
    }

    columnBox->setCurrentIndex(0);

    int size = tableModel->rowCount();
    for (int i = 0; i < size; i++) {
        if (setting.showInfo[HOSTADDRESS])
            tableModel->item(i,0)->setFlags(Qt::ItemIsEnabled);
        else
            tableModel->item(i,0)->setFlags(Qt::NoItemFlags);
        if (setting.showInfo[HOSTNAME])
            tableModel->item(i,1)->setFlags(Qt::ItemIsEnabled);
        else
            tableModel->item(i,1)->setFlags(Qt::NoItemFlags);
    }
}

void SearchDialog::change() {
    SearchCommand command;

    setCommand(command);
    emit sig(command);
}

void SearchDialog::setHost(const QModelIndex &index) {
    if (index.isValid()) {
        QString host = index.data().toString();
        if (index.column() && setting.showInfo[HOSTNAME]) {
            columnBox->setCurrentIndex(nameIndex);
            findEdit->setText(host);
            modeBox->setCurrentIndex(0);
        }
        else if (!index.column() && setting.showInfo[HOSTADDRESS]) {
            columnBox->setCurrentIndex(addressIndex);
            findEdit->setText(host);
            modeBox->setCurrentIndex(0);
        }
    }
}
