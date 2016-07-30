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

#include "menubar.h"

MenuBar::MenuBar(QMenuBar *parent) :
    QMenuBar(parent)
{
    newAction = new MethodAction("New",this);
    openAction = new MethodAction("Open",this);
    saveAction = new MethodAction("Save",this);
    saveAsAction = new MethodAction("Save As",this);
    closeAction = new MethodAction("Quit",this);

    newAction->setIcon(QIcon("icons/new.png"));
    openAction->setIcon(QIcon("icons/open.png"));
    saveAction->setIcon(QIcon("icons/save.png"));
    saveAsAction->setIcon(QIcon("icons/saveAs.png"));

    newAction->setShortcut(QKeySequence("Ctrl+N"));
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    saveAsAction->setShortcut(QKeySequence("Ctrl+S"));
    closeAction->setShortcut(QKeySequence("Ctrl+Q"));

    licenseAction = new intAction(stringKey::LICENSE, "License", this);
    hintAction = new intAction(stringKey::HELP, "Help",this);
    authorAction = new intAction(stringKey::ABOUT, "About",this);

    hintAction->setShortcut(QKeySequence("Ctrl+H"));

    fileMenu = new QMenu("File");
    filterAction = new MethodAction("Packet Filter",this);
    settingsAction = new MethodAction("Settings",this);
    searchAction = new MethodAction("Search",this);
    helpMenu = new QMenu("Help");

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);

    helpMenu->addAction(hintAction);
    helpMenu->addSeparator();
    helpMenu->addAction(licenseAction);
    helpMenu->addAction(authorAction);

    addMenu(fileMenu);
    addAction(filterAction);
    addAction(settingsAction);
    addAction(searchAction);
    addMenu(helpMenu);

    newAction->setIcon(QIcon("icons/new.png"));
    saveAction->setIcon(QIcon("icons/save.png"));
    saveAsAction->setIcon(QIcon("icons/saveAs.png"));
    hintAction->setIcon(QIcon("icons/help.png"));
}

MenuBar::~MenuBar()
{

}
