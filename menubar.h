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

#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>
#include "intaction.h"
#include "observers.h"

class MenuBar : public QMenuBar, public serverObserver
{
    Q_OBJECT

public:
    QMenu *fileMenu;
    QMenu *settingMenu;
    QMenu *viewMenu;
    QMenu *searchMenu;
    QMenu *helpMenu;

    intAction *newAction;
    intAction *clearAction;
    intAction *openAction;
    intAction *folderAction;
    QAction *closeAction;
    intAction *saveAction;
    intAction *saveAsAction;

    intAction *exportXmlAction;
    intAction *exportJsonAction;

    intAction *filterAction;

    intAction *tableAction;
    intAction *statisticsAction;
    intAction *trafficAction;
    intAction *byteAction;

    intAction *settingsAction;

    intAction *searchOnTabAction;
    intAction *searchOnFilesAction;

    intAction *licenseAction;
    intAction *hintAction;
    intAction *authorAction;

    explicit MenuBar(QMenuBar *parent = 0);
    ~MenuBar();

    void update(const serverState &state);

private:

private slots:
    void processCommand(CommandCode command) {emit sendCommand(command);}

signals:
    void sendCommand(CommandCode);

};

#endif // MENUBAR_H
