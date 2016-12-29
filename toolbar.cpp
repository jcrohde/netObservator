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

#include "toolbar.h"

ToolBar::ToolBar()
{
    addButton = new QToolButton;
    addButton->setIcon(QIcon("icons/add.png"));
    addWidget(addButton);

    openButton = new QToolButton;
    openButton->setIcon(QIcon("icons/open.png"));
    addWidget(openButton);

    saveButton = new QToolButton;
    saveButton->setIcon(QIcon("icons/save.png"));
    addWidget(saveButton);

    addSeparator();

    backButton = new QToolButton;
    backButton->setIcon(QIcon("icons/back.png"));
    addWidget(backButton);

    forwardButton = new QToolButton;
    forwardButton->setIcon(QIcon("icons/forward.png"));;
    addWidget(forwardButton);

    backButton->setDisabled(true);
    forwardButton->setDisabled(true);

    connect(addButton,SIGNAL(pressed()),this,SLOT(add()));
    connect(openButton,SIGNAL(pressed()),this,SLOT(open()));
    connect(saveButton,SIGNAL(pressed()),this,SLOT(save()));
    connect(backButton,SIGNAL(pressed()),this,SLOT(back()));
    connect(forwardButton,SIGNAL(pressed()),this,SLOT(forward()));
}

void ToolBar::update(const serverState &state) {
    saveButton->setDisabled(state.empty);
    backButton->setDisabled(state.firstDocument);
    forwardButton->setDisabled(state.lastDocument);
}

void ToolBar::add() {
    emit send(CommandCode::ADDTAB);
}

void ToolBar::open() {
    send(CommandCode::LOAD);
}

void ToolBar::save() {
    send(CommandCode::SAVEFILE);
}

void ToolBar::back() {
    send(CommandCode::BACK);
}

void ToolBar::forward() {
    send(CommandCode::FORWARD);
}
