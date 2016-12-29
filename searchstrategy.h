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

#ifndef SEARCHSTRATEGY_H
#define SEARCHSTRATEGY_H

#include <QLabel>
#include <QList>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <functional>
#include "ippacket.h"
#include "settings.h"
#include "observers.h"

class SearchStrategy : public QHBoxLayout
{
    Q_OBJECT
public:
    virtual QString title() = 0;
    std::function<void(const std::set<ipAddress> &addresses)> updateDisplay;
    virtual void complete(SearchCommand &command) = 0;
};

class SearchOnTabStrategy : public SearchStrategy, public serverObserver
{
    Q_OBJECT
public:
    SearchOnTabStrategy();

    void update(const serverState &state);
    QString title() {return "Search on Tab";}
    void complete(SearchCommand &command) {command.inFiles = false;}

private:
    QPushButton *backBut, *forwardBut;

    void enableBackForwardButtons(bool backEnable, bool forwardEnable);

private slots:
    void back();
    void forward();

signals:
    void browse(bool);
};

class SearchOnFilesStrategy : public SearchStrategy, public SniffObserver
{
    Q_OBJECT
public:
    SearchOnFilesStrategy();

    void update(const sniffState &state);
    QString title() {return "Search on Files";}
    void complete(SearchCommand &command);

private:
    QLineEdit *fileEdit;
    QPushButton *fileBut;
    QFileDialog *fileDialog;

    void setFilesForSearch(QStringList filenames);


private slots:
    void selectFiles();
};

#endif // SEARCHSTRATEGY_H
