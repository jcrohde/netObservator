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

#include <QLabel>
#include <QList>
#include <QGridLayout>
#include "searchdialog.h"
#include "dnssingleton.h"
#include <QDebug>

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Search");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    fileDialog = new QFileDialog();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *set = new QHBoxLayout();
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

    backBut = new QPushButton(QIcon("icons/back.png"),"");
    forwardBut = new QPushButton(QIcon("icons/forward.png"),"");

    findBut = new QPushButton("Find");
    closeBut = new QPushButton("Close");

    centralLayout->addWidget(findLabel,0,0,1,1);
    centralLayout->addWidget(findEdit,0,1,1,1);
    centralLayout->addWidget(columnLabel,1,0,1,1);
    centralLayout->addWidget(columnBox,1,1,1,1);

    enableBackForwardButtons(false,false);

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

    set->addWidget(backBut);
    set->addWidget(forwardBut);

    buttonLayout->addStretch();
    buttonLayout->addWidget(findBut);
    buttonLayout->addWidget(closeBut);

    mainLayout->addLayout(centralLayout);
    mainLayout->addLayout(flagLayout);
    mainLayout->addWidget(new QLabel("by Clicking on a Host:"));
    mainLayout->addWidget(display);

    optionBox = new QComboBox;
    optionBox->addItem("search in current tab:");
    optionBox->addItem("search in files:");
    mainLayout->addWidget(optionBox);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileEdit = new QLineEdit();
    fileLayout->addWidget(fileEdit);
    fileBut = new QPushButton();
    fileBut->setIcon(QIcon("icons/open.png"));
    fileLayout->addWidget(fileBut);
    mainLayout->addLayout(fileLayout);

    mainLayout->addLayout(set);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    fileEdit->setDisabled(true);
    fileBut->setDisabled(true);

    connect(findBut,SIGNAL(clicked()),this,SLOT(change()));
    connect(closeBut,SIGNAL(clicked()),this,SLOT(close()));
    connect(backBut,SIGNAL(clicked()),this,SLOT(back()));
    connect(forwardBut,SIGNAL(clicked()),this,SLOT(forward()));
    connect(display,SIGNAL(clicked(const QModelIndex &)),this,SLOT(setHost(const QModelIndex &)));
    connect(optionBox,SIGNAL(currentIndexChanged(int)),this,SLOT(enableFileSearch(int)));
    connect(fileBut,SIGNAL(clicked()),this,SLOT(selectFiles()));
}

void SearchDialog::setCommand(SearchCommand &command) {
    command.settings = setting;
    command.columnName = columnBox->currentText();
    command.searchString = findEdit->text();
    command.invertMatch = invertBox->isChecked();
    command.mode = (SearchCommand::Mode) modeBox->currentIndex();
    command.inFiles = optionBox->currentIndex();

    QString text = fileEdit->text();

    while (text.indexOf(",") > 0) {
        command.filenames.append(text.left(text.indexOf(",")));
        text = text.right(text.size()-text.indexOf(",")-1);
    }
    command.filenames.append(text);
}

void SearchDialog::setFilesForSearch(QStringList filenames) {
    optionBox->setCurrentIndex(1);

    fileEdit->clear();

    if (filenames.size() > 0) {
        QString str = filenames.at(0);
        for (int i = 1; i < filenames.size(); i++) {
            str += ",";
            str += filenames.at(i);
        }
        fileEdit->insert(str);
    }
}

void SearchDialog::setTabToSearch() {
    optionBox->setCurrentIndex(0);

    fileEdit->clear();
}

void SearchDialog::update(modelState state) {
    enableBackForwardButtons(!state.firstDocument,!state.lastDocument);
    updateTableModel(state.addresses);
}

void SearchDialog::enableBackForwardButtons(bool backEnable, bool forwardEnable) {
    forwardBut->setEnabled(forwardEnable);
    backBut->setEnabled(backEnable);
}

void SearchDialog::updateTableModel(std::set<ipAddress> &addresses) {
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
            columnBox->addItem(LABEL[i]);
            count++;
        }
    }

    columnBox->addItem(ARBITRARY);
    columnBox->setCurrentIndex(count);

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

void SearchDialog::enableFileSearch(int file) {
    fileEdit->setEnabled(file);
    fileBut->setEnabled(file);
}

void SearchDialog::change() {
    emit sig();
}

void SearchDialog::back() {
    emit browse(false);
}

void SearchDialog::forward() {
    emit browse(true);
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

void SearchDialog::selectFiles() {
    QStringList filenames = fileDialog->getOpenFileNames(
        this,
        tr("Search in Files"),
        QDir::currentPath(),
        tr("XML (*.xml)") );

    if (filenames.size() > 0) {
        QString files = filenames.at(0);
        for (int i = 1; i < filenames.size(); i++) {
            files += ",";
            files += filenames.at(i);
        }
        fileEdit->insert(files);
    }
}

