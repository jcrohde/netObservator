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

#include <QStandardItemModel>
#include "view.h"

void ViewXmlReader::read(QString XmlContent, PacketInfoPresenter *packetInfo) {
    presenter = packetInfo;
    column = 0;

    XmlReader::read(XmlContent);
}

void ViewXmlReader::readerAction(QString elementText) {
    content[column] = elementText;
    accountRowsAndColumns();
}

void ViewXmlReader::accountRowsAndColumns() {
    if (column == COLUMNNUMBER - 1) {
        presenter->show(content);
        column = 0;
    }
    else
        column++;
}

View::View(ViewXmlReader *reader) : xmlReader(reader){
    //page = new QDialog();
    //packetInfo = &tablePacketInfo;

    /*QVBoxLayout *pageLayout = new QVBoxLayout();
    pageLayout->addWidget(tablePacketInfo.display.display);
    page->setLayout(pageLayout);*/
}

void View::update(const Settings &set) {
    SettingObserver::update(set);

    tablePacketInfo.update(set);

    rewriteInfo();
}

void View::rewriteInfo() {
    init();
    if (xmlContent.size() > 0) {
        xmlReader->read(xmlContent, &tablePacketInfo);
    }
}

void View::update(modelState state) {
    xmlContent = state.document;

    rewriteInfo();
}

void View::init() {
    tablePacketInfo.init();
}
