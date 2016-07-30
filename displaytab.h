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

#ifndef DISPLAYTAB_H
#define DISPLAYTAB_H

#include <QTabWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolButton>
#include <vector>
#include "settingsdialog.h"
#include "model.h"
#include "view.h"
#include "sniffthread.h"
#include "searchdialog.h"
#include <memory>
#include <functional>
#include "controller.h"

struct modelView {
    XmlServer model;
    View view;

    modelView(ViewXmlReader *reader, XmlFilter *filter);
};

struct Page : public QDialog {
    modelView mv;
    SettingsDialog *settings;

    Page(ViewXmlReader *reader, XmlFilter *filter) : mv(reader,filter) {
        QVBoxLayout *pageLayout = new QVBoxLayout();
        pageLayout->addWidget(mv.view.tablePacketInfo.display.display);
        setLayout(pageLayout);
    }

    ~Page() {
        settings->unregisterObserver(&mv.view);
    }
};

class DisplayTab : public QTabWidget, public SettingObserver
{
    Q_OBJECT
public:
    DisplayTab(Controller *c, SniffThread *sth, SettingsDialog *set, SearchDialog *sDlg, std::function<bool()> modifiable);

    void setTabTitle(QString title, bool add);

    void setSniffing(bool sniff) {sniffing = sniff; setToCurrentTab(currentIndex());}

    void update(const Settings &set);

private:
    std::function<bool()> canBeModified;

    int displayCount, myIndex;

    bool sniffing;

    QToolButton *toolButton;
    SettingsDialog *settingSubject;

    ViewXmlReader reader;
    XmlFilter filter;
    SniffThread *sniff;
    SearchDialog *searchDlg;
    Controller *controller;

    XmlServer *currentModel;
    TablePacketInfoPresenter *currentModelPresenter;

    void updateData(int index);

private slots:
    void add();
    void closeTab(int index);
    void setToCurrentTab(int index);

};

#endif // DISPLAYTAB_H
