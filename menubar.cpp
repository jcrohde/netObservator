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
    newAction = new intAction(CommandCode::CLEAR,"New",this);
    openAction = new intAction(CommandCode::LOAD,"Open",this);
    saveAction = new intAction(CommandCode::SAVEFILE,"Save",this);
    saveAsAction = new intAction(CommandCode::SAVEFILEAS,"Save As",this);
    closeAction = new QAction("Quit",this);

    newAction->setIcon(QIcon("icons/new.png"));
    openAction->setIcon(QIcon("icons/open.png"));
    saveAction->setIcon(QIcon("icons/save.png"));
    saveAsAction->setIcon(QIcon("icons/saveAs.png"));

    newAction->setShortcut(QKeySequence("Ctrl+N"));
    openAction->setShortcut(QKeySequence("Ctrl+O"));
    saveAsAction->setShortcut(QKeySequence("Ctrl+S"));
    closeAction->setShortcut(QKeySequence("Ctrl+Q"));

    statisticsAction = new intAction(CommandCode::SHOWSTATISTICSDIALOG,"Statistics",this);
    trafficAction = new intAction(CommandCode::SHOWTRAFFICDIALOG,"Packet Number",this);
    byteAction = new intAction(CommandCode::SHOWBYTEDIALOG,"Byte Number",this);

    searchOnTabAction = new intAction(CommandCode::SHOWSEARCHTABDIALOG,"Search on Tab",this);
    searchOnFilesAction = new intAction(CommandCode::SHOWSEARCHFILEDIALOG,"Search on Files",this);

    licenseAction = new intAction(CommandCode::LICENSE, "License", this);
    hintAction = new intAction(CommandCode::HELP, "Help",this);
    authorAction = new intAction(CommandCode::ABOUT, "About",this);

    hintAction->setShortcut(QKeySequence("Ctrl+H"));

    fileMenu = new QMenu("File");
    filterAction = new intAction(CommandCode::SHOWPACKETFILTEREDITOR,"Packet Filter",this);
    viewMenu = new QMenu("View");
    settingsAction = new intAction(CommandCode::SHOWSETTINGSDIALOG,"Settings",this);
    searchMenu = new QMenu("Search");
    helpMenu = new QMenu("Help");

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);

    viewMenu->addAction(statisticsAction);
    viewMenu->addAction(trafficAction);
    viewMenu->addAction(byteAction);

    searchMenu->addAction(searchOnTabAction);
    searchMenu->addAction(searchOnFilesAction);

    searchOnTabAction->setIcon(QIcon("icons/searchTab.png"));
    searchOnFilesAction->setIcon(QIcon("icons/searchFiles.png"));

    helpMenu->addAction(hintAction);
    helpMenu->addSeparator();
    helpMenu->addAction(licenseAction);
    helpMenu->addAction(authorAction);

    hintAction->setIcon(QIcon("icons/help.png"));

    addMenu(fileMenu);
    addAction(filterAction);
    addMenu(viewMenu);
    addAction(settingsAction);
    addMenu(searchMenu);
    addMenu(helpMenu);

    QObject::connect(newAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(openAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(saveAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(saveAsAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));

    QObject::connect(filterAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(statisticsAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(trafficAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(byteAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(settingsAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));

    QObject::connect(searchOnTabAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
    QObject::connect(searchOnFilesAction,SIGNAL(sigCode(CommandCode)),this,SLOT(processCommand(CommandCode)));
}

MenuBar::~MenuBar()
{

}
