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

#include "displaytab.h"


DisplayTab::DisplayTab(Controller *c)
    : controll(c)
{
    fixed = false;
    displayCount = 0;

    add();
    setToCurrentTab(0);

    setTabsClosable(true);

    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(setToCurrentTab(int)));
}

void DisplayTab::update(const serverState &state) {
    setTabText(currentIndex(),state.title);
    dynamic_cast<Page*>(widget(currentIndex()))->setSlicesVisible(state.sliceNames);
}

void DisplayTab::fixCurrentTab(bool fix) {
    fixed = fix;
    fixIndex = currentIndex();
}

void DisplayTab::updateController(int index) {
    controll->setModelView(&dynamic_cast<Page*>(widget(index))->mv);
}

void DisplayTab::add() {
    if (!fixed) {
        Page *page = new Page();

        displayCount++;
        addTab(page,"Display " + QString::number(displayCount));
        page->mv.model.setTitle("Display " + QString::number(displayCount));
        setCurrentWidget(widget(count()-1));

        connect(page->sliceBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(loadSlice(QString)));
    }
}

void DisplayTab::closeTab(int index) {
    if (!fixed && count() > 1) {
        QWidget *tab = widget(index);
        removeTab(index);
        delete tab;
    }
}

void DisplayTab::setToCurrentTab(int index) {
    if (fixed) {
        if (index != fixIndex)
            setCurrentWidget(widget(fixIndex));
    }
    else
        updateController(index);
}

void DisplayTab::loadSlice(QString sliceName) {
    if (sliceName.size() > 0) {
        Command cmd;
        cmd.arguments.append(sliceName);
        cmd.code = CommandCode::LOADSLICE;
        controll->getCommand(cmd);
    }
}
