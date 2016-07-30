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

modelView::modelView(ViewXmlReader *reader, XmlFilter *filter)
    : view(reader), model(filter) {
    model.registerObserver(&view);
}

DisplayTab::DisplayTab(Controller *c, SniffThread *sth, SettingsDialog *set, SearchDialog *sDlg, std::function<bool()> modifiable)
    : controller(c), sniff(sth), searchDlg(sDlg), settingSubject(set), canBeModified(modifiable)
{
    displayCount = 0;

    sniffing = false;

    std::function<void(QString,bool)> tabLambda = [&](QString title,bool add) {setTabTitle(title,add);};
    controller->setTabTitle = tabLambda;

    controller->getTabTitle = [&]()->QString {return tabText(myIndex);};

    add();
    setToCurrentTab(0);

    toolButton = new QToolButton(this);
    toolButton->setText("+");
    toolButton->setCursor(Qt::ArrowCursor);
    toolButton->setAutoRaise(true);
    setCornerWidget(toolButton,Qt::TopLeftCorner);

    setTabsClosable(true);

    connect(toolButton,SIGNAL(clicked(bool)),this,SLOT(add()));
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(setToCurrentTab(int)));
}

void DisplayTab::updateData(int index) {
    currentModel = &dynamic_cast<Page*>(widget(index))->mv.model;
    currentModelPresenter = &dynamic_cast<Page*>(widget(index))->mv.view.tablePacketInfo;
    controller->setModel(currentModel);
}

void DisplayTab::setTabTitle(QString title, bool add) {
    setTabText(myIndex,(add ? tabText(myIndex) : "") + title);
}

void DisplayTab::update(const Settings &set) {
    SettingObserver::update(set);
}

void DisplayTab::add() {
    if (canBeModified()) {
        Page *page = new Page(&reader,&filter);
        page->mv.view.update(setting);
        page->mv.model.registerObserver(searchDlg);

        if (settingSubject != 0) {
            settingSubject->registerObserver(&page->mv.view);
            page->settings = settingSubject;
        }

        displayCount++;
        addTab(page, "display " + QString::number(displayCount));
        updateData(0);
    }
}

void DisplayTab::closeTab(int index) {
    if (canBeModified() && count() > 1) {
        QWidget *tab = widget(index);
        removeTab(index);
        delete tab;
    }
}

void DisplayTab::setToCurrentTab(int index) {
    if (!sniffing) {
        myIndex = index;
        updateData(index);
        sniff->setPacketInfoPresenter(currentModelPresenter);
        currentModel->notifyObservers(true);
    }
}
