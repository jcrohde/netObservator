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

#include <QLabel>
#include <QGridLayout>
#include "searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Filter");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *set = new QHBoxLayout();
    QHBoxLayout *flagLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QGridLayout *centralLayout = new QGridLayout();
    QLabel *columnLabel = new QLabel("in Column:");
    QLabel *findLabel = new QLabel("Text:");

    columnBox = new QComboBox();
    findEdit = new QLineEdit();

    invertBox = new QCheckBox();
    invertBox->setText("invert match");

    regexBox = new QCheckBox();
    regexBox->setText("regular expression");

    backBut = new QPushButton(QIcon("icons/back.png"),"");
    forwardBut = new QPushButton(QIcon("icons/forward.png"),"");

    findBut = new QPushButton("Find");
    closeBut = new QPushButton("Close");

    centralLayout->addWidget(findLabel,0,0,1,1);
    centralLayout->addWidget(findEdit,0,1,1,1);
    centralLayout->addWidget(columnLabel,1,0,1,1);
    centralLayout->addWidget(columnBox,1,1,1,1);

    enableBackForwardButtons(false,false);

    flagLayout->addWidget(invertBox);
    flagLayout->addWidget(regexBox);

    set->addWidget(backBut);
    set->addWidget(forwardBut);

    buttonLayout->addStretch();
    buttonLayout->addWidget(findBut);
    buttonLayout->addWidget(closeBut);

    mainLayout->addLayout(centralLayout);
    mainLayout->addLayout(flagLayout);
    mainLayout->addLayout(set);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(findBut,SIGNAL(clicked()),this,SLOT(change()));
    connect(closeBut,SIGNAL(clicked()),this,SLOT(close()));
    connect(backBut,SIGNAL(clicked()),this,SLOT(back()));
    connect(forwardBut,SIGNAL(clicked()),this,SLOT(forward()));
}

void SearchDialog::enableBackForwardButtons(bool backEnable, bool forwardEnable) {
    forwardBut->setEnabled(forwardEnable);
    backBut->setEnabled(backEnable);
}

void SearchDialog::enlistColumnBox() {
    columnBox->clear();

    int count = 0;
    for (int i = 0; i < COLUMNNUMBER; i++) {
        if (setting.showInfo[i]) {
            columnBox->addItem(LABEL[i]);
            count++;
        }
    }

    columnBox->addItem(ARBITRARY);
    columnBox->setCurrentIndex(count);
}

void SearchDialog::setCommand(SearchCommand &command) {
    command.settings = setting;
    command.columnName = columnBox->currentText();
    command.searchString = findEdit->text();
    command.invertMatch = invertBox->isChecked();
    command.regex = regexBox->isChecked();
}

void SearchDialog::back() {
    emit browse(false);
}

void SearchDialog::forward() {
    emit browse(true);
}

void SearchDialog::change() {
    emit sig();
}

