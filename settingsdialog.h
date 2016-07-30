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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QFileDialog>
#include "util.h"
#include "stringfactory.h"
#include "packetfiltereditor.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

    void registerObserver(SettingObserver *observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(SettingObserver *observer) {
        std::vector<SettingObserver*>::iterator pos = std::find(observers.begin(),observers.end(),observer);
        if (pos != observers.end())
            observers.erase(pos);
    }

private:
    QSpinBox *durationSpinBox;
    QSlider *durationSlider;

    QCheckBox boxes[COLUMNNUMBER];
    QCheckBox *allBox, *saveBox;

    QPushButton *okBut, *cancelBut;

    QFileDialog *fileDialog;
    QLineEdit *sliceNameEdit;
    QPushButton *sliceNameBut;
    QSpinBox *sliceSizeBox;

    Settings setting;

    std::vector<SettingObserver*> observers;

private slots:
    void selectSliceFileName();
    void activateSlices(bool selection);
    void selectAll(bool selection);
    void changeSettings();
};

#endif // SETTINGSDIALOG_H
