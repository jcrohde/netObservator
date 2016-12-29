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
#include <QGroupBox>
#include <QFrame>


SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    fileDialog = new QFileDialog();

    QVBoxLayout *ordering = new QVBoxLayout();

    generateSniffingSection(ordering);

    generateButtonSection(ordering);

    setLayout(ordering);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connectSignalsAndSlots();
}

void SettingsDialog::generateSniffingSection(QVBoxLayout *settingLayout) {
    QHBoxLayout *durationLayout = new QHBoxLayout();
    durationLayout->addWidget(new QLabel("duration in sec"));
    durationSpinBox = new QSpinBox;
    durationSpinBox->setValue(20);
    durationLayout->addWidget(durationSpinBox);
    settingLayout->addLayout(durationLayout);

    durationSlider = new QSlider(Qt::Horizontal);
    durationSlider->setRange(10,60);
    durationSlider->setValue(20);
    settingLayout->addWidget(durationSlider);

    settingLayout->addStretch();

    sniffModeBox = new QComboBox();
    sniffModeBox->addItem("save and show connections");
    sniffModeBox->addItem("just save");
    sniffModeBox->addItem("just show connections");
    settingLayout->addWidget(sniffModeBox);

    settingLayout->addStretch();

    QHBoxLayout *folderNameLayout = new QHBoxLayout();
    folderNameLayout->addWidget(new QLabel("Save in Folder:"));
    folderNameEdit = new QLineEdit();
    folderNameEdit->insert("Sniffed");
    folderNameBut = new QPushButton();
    folderNameBut->setIcon(QIcon("icons/open.png"));
    folderNameLayout->addWidget(folderNameEdit);
    folderNameLayout->addWidget(folderNameBut);
    settingLayout->addLayout(folderNameLayout);

    settingLayout->addStretch();

    QHBoxLayout *sliceNameLayout = new QHBoxLayout();
    sliceNameLayout->addWidget(new QLabel("Slice file name:"));
    sliceNameEdit = new QLineEdit();
    sliceNameEdit->insert("Slice");
    sliceNameBut = new QPushButton();
    sliceNameBut->setIcon(QIcon("icons/open.png"));
    sliceNameLayout->addWidget(sliceNameEdit);
    sliceNameLayout->addWidget(sliceNameBut);
    settingLayout->addLayout(sliceNameLayout);

    settingLayout->addStretch();

    QHBoxLayout *packetLayout = new QHBoxLayout();
    packetLayout->addWidget(new QLabel("Packets per Slice:"));
    sliceSizeBox = new QSpinBox();
    sliceSizeBox->setRange(100,100000);
    sliceSizeBox->setValue(1000);
    packetLayout->addWidget(sliceSizeBox);
    settingLayout->addLayout(packetLayout);
}

void SettingsDialog::generateButtonSection(QVBoxLayout *ordering) {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addStretch();

    okBut = new QPushButton("Ok");
    buttonLayout->addWidget(okBut);

    cancelBut = new QPushButton("Cancel");
    buttonLayout->addWidget(cancelBut);

    ordering->addLayout(buttonLayout);
}

void SettingsDialog::connectSignalsAndSlots() {
    connect(durationSlider, SIGNAL(valueChanged(int)),durationSpinBox, SLOT(setValue(int)));
    connect(durationSpinBox, SIGNAL(valueChanged(int)),durationSlider, SLOT(setValue(int)));
    connect(sniffModeBox,SIGNAL(currentIndexChanged(int)),this,SLOT(activateSlices(int)));
    connect(sliceNameBut,SIGNAL(clicked()),this,SLOT(selectSliceFileName()));
    connect(folderNameBut,SIGNAL(clicked()),this,SLOT(selectSaveFolderName()));
    connect(okBut,SIGNAL(clicked()),this,SLOT(changeSettings()));
    connect(cancelBut,SIGNAL(clicked()),this,SLOT(close()));
}

void SettingsDialog::selectSliceFileName() {
    QString filename = fileDialog->getSaveFileName(
        this,
        tr("Select Slice File Name"),
        QDir::currentPath(),
        tr("")
    );

    sliceNameEdit->setText(filename);
}


void SettingsDialog::selectSaveFolderName() {
    QString filename = fileDialog->getExistingDirectory(
        this,
        tr("Select Slice File Name"),
        QDir::currentPath());

    if (filename.size()>0) {
        QString text = folderNameEdit->text();
        text = text.right(text.size()-text.lastIndexOf("/")-1);
        text = filename + "/" + text;
        folderNameEdit->setText(text);
    }
}

void SettingsDialog::activateSlices(int selection) {
    bool save = (selection == (int)sniffSettings::Mode::JUSTSAVE || selection == (int)sniffSettings::Mode::SAVEANDSHOW);
    sliceNameBut->setEnabled(save);
    sliceNameEdit->setEnabled(save);
    sliceSizeBox->setEnabled(save);
}

void SettingsDialog::changeSettings() {

    setting.duration = durationSpinBox->value();
    setting.mode = (sniffSettings::Mode)sniffModeBox->currentIndex();
    setting.storeFolderName = folderNameEdit->text();
    setting.sliceFileName = sliceNameEdit->text();
    setting.sliceSize = sliceSizeBox->value();

    emit change(setting);

    close();
}
