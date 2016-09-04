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

#include "xmlserver.h"

bool XmlFilter::generateFilteredXmlDocument(const SearchCommand &cmd, QString &input, QString &output) {
    writer.reset(new QXmlStreamWriter(&output));

    command = cmd;
    regexp = QRegExp(command.searchString);
    reinitialize();

    writer->writeStartDocument();
    writer->writeStartElement(SNIFFED);
    bool valid = read(input);
    writer->writeEndElement();

    return valid;
}

void XmlFilter::readerAction(QString elementText) {
    handleElementText(elementText);
    columnCount++;
    if (columnCount == COLUMNNUMBER) {
        closePacketInfo();
    }
}

void XmlFilter::handleElementText(QString &elementText) {
    subNodes.append(elementText);
    if ((command.settings.showInfo[columnCount])
      && (command.columnName == ARBITRARY || onLocalOrSrc(LABEL[columnCount]) == command.columnName)) {
        if (command.mode == command.REGEX)
            found = found || regexp.indexIn(elementText) != -1;
        else if (command.mode == command.CASEINSENSITIVE)
            found = found || elementText.contains(command.searchString,Qt::CaseInsensitive);
        else
            found = found || elementText.contains(command.searchString);
    }
}

void XmlFilter::closePacketInfo() {
    if (command.invertMatch)
        found = !found;
    if (found) {
        writePacketInfo();
    }
    reinitialize();
}

void XmlFilter::writePacketInfo() {
    writer->writeStartElement(PACKETINFO);
    for (int i = 0; i < COLUMNNUMBER; i++) {
        writer->writeTextElement(LABEL[i],subNodes[i]);
    }
    writer->writeEndElement();
}

void XmlFilter::reinitialize() {
    found = false;
    columnCount = 0;
    subNodes.clear();
}

XmlServer::XmlServer(XmlFilter *xmlFilter) : filter(xmlFilter) {
    title = "Title";
    changed = false;
    clear();
}

XmlServer::~XmlServer() {

}

void XmlServer::load(QString xmlContent, std::set<ipAddress> addr) {
    if (xmlContent.size() > 0)
        output[0] = xmlContent;
    addresses = addr;
    changed = true;
}

bool XmlServer::search(SearchCommand cmd) {
    QString result;

    bool valid = filter->generateFilteredXmlDocument(cmd,output[currentOutputIter],result);

    if (valid) {
        setAsLastDocument(result);
        title += "->search";
        changed = true;
    }

    return valid;
}

void XmlServer::changeText(bool forward) {
    if (forward) {
        currentOutputIter++;
        title += "->search";
    }
    else {
        currentOutputIter--;
        title = title.left(title.lastIndexOf("->search"));
    }
    changed = true;
}

void XmlServer::getState(serverState &state) {
    state.firstDocument = currentOutputIter == 0;
    state.lastDocument = (currentOutputIter == output.size()-1);
    state.addresses = addresses;
    state.title = title;
    state.sliceNames = sliceNames;

    changed = false;
}

void XmlServer::clear() {
    output.clear();
    currentOutputIter = 0;
    output.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?><"+SNIFFED+"></"+SNIFFED+">");
    changed = true;
    sliceNames.clear();
    addresses.clear();
}

void XmlServer::setAsLastDocument(QString &document) {
    currentOutputIter++;

    while (output.size() > currentOutputIter)
        output.removeLast();

    output.append(document);
}

void XmlServer::setSniffed(const QStringList &slices) {
    sliceNames = slices;
}
