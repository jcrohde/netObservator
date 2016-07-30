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

    fileDialog = new QFileDialog();

    QVBoxLayout *ordering = new QVBoxLayout();
    QVBoxLayout *optionLayout = new QVBoxLayout();
    QHBoxLayout *settingLayout = new QHBoxLayout();
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QGroupBox *optionFrame = new QGroupBox(this);
    optionFrame->setTitle("Sniffing");
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

    optionLayout->addStretch();

    optionLayout->addWidget(new QLabel("Slice file name:"));
    QHBoxLayout *sliceNameLayout = new QHBoxLayout();
    sliceNameEdit = new QLineEdit();
    sliceNameEdit->insert("Slice");
    sliceNameBut = new QPushButton();
    sliceNameBut->setIcon(QIcon("icons/open.png"));
    sliceNameLayout->addWidget(sliceNameEdit);
    sliceNameLayout->addWidget(sliceNameBut);
    optionLayout->addLayout(sliceNameLayout);

    optionLayout->addStretch();

    optionLayout->addWidget(new QLabel("Packets per Slice:"));
    sliceSizeBox = new QSpinBox();
    sliceSizeBox->setRange(100,100000);
    sliceSizeBox->setValue(1000);
    optionLayout->addWidget(sliceSizeBox);

    optionFrame->setLayout(optionLayout);

    settingLayout->addWidget(optionFrame);

    QGroupBox *infoFrame = new QGroupBox(this);

    infoFrame->setTitle("Presentation");
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

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(durationSlider, SIGNAL(valueChanged(int)),durationSpinBox, SLOT(setValue(int)));
    connect(durationSpinBox, SIGNAL(valueChanged(int)),durationSlider, SLOT(setValue(int)));
    connect(allBox,SIGNAL(toggled(bool)),this,SLOT(selectAll(bool)));
    connect(saveBox,SIGNAL(toggled(bool)),this,SLOT(activateSlices(bool)));
    connect(sliceNameBut,SIGNAL(clicked()),this,SLOT(selectSliceFileName()));
    connect(okBut,SIGNAL(clicked()),this,SLOT(changeSettings()));
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(close()));
}

void SettingsDialog::selectSliceFileName() {
    QString filename = fileDialog->getSaveFileName(
        this,
        tr("Select Slice File Name"),
        QDir::currentPath(),
        tr("") );

    sliceNameEdit->setText(filename);
}

void SettingsDialog::activateSlices(bool selection) {
    sliceNameBut->setEnabled(selection);
    sliceNameEdit->setEnabled(selection);
    sliceSizeBox->setEnabled(selection);
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
    setting.sliceFileName = sliceNameEdit->text();
    setting.sliceSize = sliceSizeBox->value();

    for (auto observer: observers) {
        observer->update(setting);
    }

    close();
}
