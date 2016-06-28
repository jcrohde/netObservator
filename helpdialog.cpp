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

#include <QHBoxLayout>
#include "helpdialog.h"

HelpDialog::HelpDialog() {
    setWindowIcon(QIcon("icons/observerLogo.png"));

    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    closeBut = new QPushButton("Close");
    browser = new QTextBrowser;

    vbox->addWidget(browser);

    this->browser->setOpenLinks(false);

    hbox->addStretch();
    hbox->addWidget(closeBut);

    vbox->addLayout(hbox);

    setLayout(vbox);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QObject::connect(this->closeBut,SIGNAL(clicked()),this,SLOT(close()));
}

HelpDialog::~HelpDialog(){

}

void HelpDialog::load(QString str) {
    this->browser->setHtml(str);
}
