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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QFileDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "settings.h"
#include "stringfactory.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(const sniffSettings &set, QWidget *parent = 0);
    void getSettings(sniffSettings& s) {s = setting;}

private:
    QSpinBox *durationSpinBox;
    QSlider *durationSlider;

    QComboBox *sniffModeBox;

    QPushButton *okBut, *cancelBut;

    QFileDialog *fileDialog;
    QLineEdit *sliceNameEdit, *folderNameEdit;
    QPushButton *sliceNameBut, *folderNameBut;
    QSpinBox *sliceSizeBox;

    sniffSettings setting;

    void generateSniffingSection(QVBoxLayout *settingLayout);
    void generateButtonSection(QVBoxLayout *ordering);
    void connectSignalsAndSlots();

private slots:
    void selectSaveFolderName();
    void selectSliceFileName();
    void activateSlices(int selection);
    void changeSettings();

signals:
    void change(sniffSettings&);
};

#endif // SETTINGSDIALOG_H
