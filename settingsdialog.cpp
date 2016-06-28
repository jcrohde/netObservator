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

#include "settingsdialog.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    QVBoxLayout *ordering = new QVBoxLayout();
    QVBoxLayout *optionLayout = new QVBoxLayout();
    QHBoxLayout *settingLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QGroupBox *optionFrame = new QGroupBox(this);
    optionFrame->setTitle("Options");
    optionFrame->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    optionLayout->addStretch();
    optionLayout->addWidget(new QLabel("duration in sec"));

    durationSpinBox = new QSpinBox;
    durationSpinBox->setValue(20);
    optionLayout->addWidget(durationSpinBox);

    durationSlider = new QSlider(Qt::Horizontal);
    durationSlider->setRange(10,60);
    durationSlider->setValue(20);
    optionLayout->addWidget(durationSlider);

    optionLayout->addStretch();

    saveBox = new QCheckBox();
    optionLayout->addWidget(saveBox);
    saveBox->setText("save");
    saveBox->setChecked(setting.save);

    optionFrame->setLayout(optionLayout);

    settingLayout->addWidget(optionFrame);

    QGroupBox *infoFrame = new QGroupBox(this);

    infoFrame->setTitle("Information");
    infoFrame->setFlat(true);
    infoFrame->setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    QGridLayout *centralLayout = new QGridLayout();

    for (int i = 0; i< COLUMNNUMBER; i++) {
        centralLayout->addWidget(&boxes[i],i,1,1,1);
        boxes[i].setText("show " + LABEL[i]);
        boxes[i].setChecked(setting.showInfo[i]);
    }

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);

    centralLayout->addWidget(line,10,0,1,2);

    allBox = new QCheckBox();
    centralLayout->addWidget(allBox,11,1,1,1);
    allBox->setText("show all");

    infoFrame->setLayout(centralLayout);

    settingLayout->addWidget(infoFrame);

    ordering->addLayout(settingLayout);

    buttonLayout->addStretch();

    okBut = new QPushButton("Ok");
    buttonLayout->addWidget(okBut);

    cancelBut = new QPushButton("Cancel");
    buttonLayout->addWidget(cancelBut);

    ordering->addLayout(buttonLayout);

    setLayout(ordering);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(durationSlider, SIGNAL(valueChanged(int)),
            durationSpinBox, SLOT(setValue(int)));
    connect(durationSpinBox, SIGNAL(valueChanged(int)),
            durationSlider, SLOT(setValue(int)));
    connect(allBox,SIGNAL(toggled(bool)),
            this,SLOT(selectAll(bool)));
    connect(okBut,SIGNAL(clicked()),this,SLOT(changeSettings()));
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(close()));
}

void SettingsDialog::selectAll(bool selection) {
    for (int i = 0; i< COLUMNNUMBER; i++) {
        if (selection) boxes[i].setChecked(true);
        boxes[i].setDisabled(selection);
    }
}

void SettingsDialog::changeSettings() {
    for (int i = 0; i < COLUMNNUMBER; i++) {
        setting.showInfo[i] = boxes[i].isChecked();
    }
    setting.duration = durationSpinBox->value();
    setting.save = saveBox->isChecked();

    for (auto observer: observers) {
        observer->update(setting);
    }

    close();
}
