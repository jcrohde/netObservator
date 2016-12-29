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

#include "viewsettingsdialog.h"
#include <QGroupBox>
#include <QFrame>


ViewSettingsDialog::ViewSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Packet Information");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    QVBoxLayout *ordering = new QVBoxLayout;

    generatePresentationSection(ordering);

    generateButtonSection(ordering);

    setLayout(ordering);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connectSignalsAndSlots();
}

void ViewSettingsDialog::generatePresentationSection(QVBoxLayout *settingLayout) {

    QGridLayout *centralLayout = new QGridLayout();

    for (int i = 0; i< COLUMNNUMBER; i++) {
        centralLayout->addWidget(&boxes[i],i,1,1,1);
        boxes[i].setText(onLocalOrSrc(LABEL[i]));
        boxes[i].setChecked(setting.showInfo[i]);
    }

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);

    centralLayout->addWidget(line,COLUMNNUMBER,0,1,2);

    allBox = new QCheckBox();
    centralLayout->addWidget(allBox,COLUMNNUMBER + 1,1,1,1);
    allBox->setText("show all");

    settingLayout->addLayout(centralLayout);
}

void ViewSettingsDialog::generateButtonSection(QVBoxLayout *ordering) {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addStretch();

    okBut = new QPushButton("Ok");
    buttonLayout->addWidget(okBut);

    cancelBut = new QPushButton("Cancel");
    buttonLayout->addWidget(cancelBut);

    ordering->addLayout(buttonLayout);
}

void ViewSettingsDialog::connectSignalsAndSlots() {
    connect(allBox,SIGNAL(toggled(bool)),this,SLOT(selectAll(bool)));
    connect(okBut,SIGNAL(clicked()),this,SLOT(changeSettings()));
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(close()));
}

void ViewSettingsDialog::selectAll(bool selection) {
    for (int i = 0; i< COLUMNNUMBER; i++) {
        if (selection) boxes[i].setChecked(true);
        boxes[i].setDisabled(selection);
    }
}

void ViewSettingsDialog::changeSettings() {
    setting.shownColumns = 0;

    for (int i = 0; i < COLUMNNUMBER; i++) {
        setting.showInfo[i] = boxes[i].isChecked();
        if (boxes[i].isChecked())
            setting.shownColumns++;
    }

    emit change(setting);

    close();
}
