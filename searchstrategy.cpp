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

#include <QIcon>
#include "searchstrategy.h"
#include "util.h"

SearchOnTabStrategy::SearchOnTabStrategy() {
    backBut = new QPushButton(QIcon("icons/back.png"),"");
    forwardBut = new QPushButton(QIcon("icons/forward.png"),"");

    addWidget(backBut);
    addWidget(forwardBut);

    enableBackForwardButtons(false,false);

    connect(backBut,SIGNAL(clicked()),this,SLOT(back()));
    connect(forwardBut,SIGNAL(clicked()),this,SLOT(forward()));
}

void SearchOnTabStrategy::update(const serverState &state) {
    enableBackForwardButtons(!state.firstDocument,!state.lastDocument);
    updateDisplay(state.addresses);
}

void SearchOnTabStrategy::enableBackForwardButtons(bool backEnable, bool forwardEnable) {
    forwardBut->setEnabled(forwardEnable);
    backBut->setEnabled(backEnable);
}

void SearchOnTabStrategy::back() {
    emit browse(false);
}

void SearchOnTabStrategy::forward() {
    emit browse(true);
}

SearchOnFilesStrategy::SearchOnFilesStrategy() {
    fileDialog = new QFileDialog();

    addWidget(new QLabel("Files:"));
    fileEdit = new QLineEdit();
    addWidget(fileEdit);
    fileBut = new QPushButton();
    fileBut->setIcon(QIcon("icons/open.png"));
    addWidget(fileBut);

    connect(fileBut,SIGNAL(clicked()),this,SLOT(selectFiles()));
}

void SearchOnFilesStrategy::update(const sniffState &state) {
    if (state.valid && !state.sniffing) {
        QStringList names;
        getPcapFileNames(state.folderName,names);
    }
}

void SearchOnFilesStrategy::update(const serverState &state) {
    updateDisplay(state.addresses);
    if (state.sliceNames.isEmpty()) {
        QStringList list;
        list.append(state.title);
        setFilesForSearch(list);
    }
    else setFilesForSearch(state.sliceNames);
}

void SearchOnFilesStrategy::complete(SearchCommand &command) {
    command.inFiles = true;

    QString text = fileEdit->text();

        while (text.indexOf(",") > 0) {
            command.filenames.append(text.left(text.indexOf(",")));
            text = text.right(text.size()-text.indexOf(",")-1);
        }
        command.filenames.append(text);
}

void SearchOnFilesStrategy::setFilesForSearch(QStringList filenames) {
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

void SearchOnFilesStrategy::selectFiles() {
    QStringList filenames = fileDialog->getOpenFileNames(
        NULL,
        tr("Search in Files"),
        QDir::currentPath(),
        tr("pcap (*.pcap)") );

    if (filenames.size() > 0) {
        QString files = filenames.at(0);
        for (int i = 1; i < filenames.size(); i++) {
            files += ",";
            files += filenames.at(i);
        }
        fileEdit->insert(files);
    }
}
