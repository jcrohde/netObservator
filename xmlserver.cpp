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

#include "xmlserver.h"
#include "util.h"

#include <QDir>

XmlServer::XmlServer() {
    title = "Title";
    changed = false;
    empty = true;
    clear();
}

XmlServer::~XmlServer() {

}

void XmlServer::load(QString xmlContent, std::set<ipAddress> addr) {
    if (xmlContent.size() > 0)
        output[currentOutputIter] = xmlContent;
    addresses = addr;
    changed = true;
    empty = false;
}

void XmlServer::loadFolder(QString folderName, const QStringList &fileNames) {
    folderLoaded = true;
    this->folderName = folderName;
    sliceNames = fileNames;
}

bool XmlServer::search(SearchCommand cmd) {
    bool valid = true;
    if (folderLoaded) parser->search(cmd, sliceNames);
    else {
        QStringList list;
        list.append(title);
        parser->search(cmd, list);
    }

    if (valid) {
        if (folderLoaded) {
            title += "/Search";
            folderName += "/Search";
            loadSlicesOfCurrentFolder();
            if (sliceNames.size() > 0) setAsLastDocument(sliceNames[0]);
        }
        else {
            title = title.left(title.size() - 5) + "Search.pcap";
            title = title.left(title.lastIndexOf("/")) + "/Search/" + title.right(title.size()-title.lastIndexOf("/")-1);
            setAsLastDocument(title);
        }

        changed = true;
    }

    return valid;
}

void XmlServer::changeText(bool forward) {
    if (forward) {
        currentOutputIter++;  
    }
    else {
        currentOutputIter--;
    }
    if (folderLoaded) {
        if (forward) folderName += "/Search";
        else folderName = folderName.left(folderName.size() - 7);
        title = folderName;
        loadSlicesOfCurrentFolder();
    }
    else title = output[currentOutputIter];
    changed = true;
}

void XmlServer::getState(serverState &state) {
    state.firstDocument = currentOutputIter == 0;
    state.lastDocument = (currentOutputIter == output.size()-1);
    state.addresses = addresses;
    state.title = title;
    state.foldername = folderName;
    state.sliceNames = sliceNames;
    state.empty = empty;

    changed = false;
}

bool XmlServer::copy(const QString &destination) {
    bool valid = true;

    parseInstruction instruction;
    instruction.mode = Mode::COPY;
    instruction.destination = destination;
    instruction.folder = folderLoaded;

    if (folderLoaded) parser->parse(sliceNames, instruction);
    else {
        QStringList list;
        list.append(title);
        parser->parse(list, instruction);
    }

    return valid;
}

void XmlServer::exportFormat(const QString &destination, Mode mode, const viewSettings &settings) {
    parseInstruction instruction;
    instruction.settings = settings;
    instruction.mode = mode;
    instruction.folder = sliceNames.size() > 0;
    instruction.destination = destination;
    if (instruction.folder)
        parser->parse(sliceNames, instruction);
    else
        parser->parse(title, instruction);
}

void XmlServer::clear() {
    output.clear();
    currentOutputIter = 0;
    output.append("");
    changed = true;
    sliceNames.clear();
    addresses.clear();
    empty = true;
    folderLoaded = false;
}

void XmlServer::loadSlicesOfCurrentFolder() {
    QDir dir(folderName);
    sliceNames.clear();
    QStringList entryList = dir.entryList();
    for (const QString &entry : entryList) {
        if (entry.lastIndexOf(".pcap") == entry.size() - 5 && entry.size() > 5) {
            sliceNames.append(folderName + "/" + entry);
        }
    }
}

void XmlServer::setAsLastDocument(QString &document) {
    currentOutputIter++;

    while (output.size() > currentOutputIter)
        output.removeLast();

    output.append(document);
}
