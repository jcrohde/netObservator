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

#ifndef VIEWSETTINGSDIALOG_H
#define VIEWSETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "settings.h"

class ViewSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ViewSettingsDialog(QWidget *parent = 0);

private:

    QCheckBox boxes[COLUMNNUMBER];
    QCheckBox *allBox;

    QPushButton *okBut, *cancelBut;

    viewSettings setting;

    void generatePresentationSection(QVBoxLayout *settingLayout);
    void generateButtonSection(QVBoxLayout *ordering);
    void connectSignalsAndSlots();

private slots:
    void selectAll(bool selection);
    void changeSettings();

signals:
    void change(viewSettings&);
};

#endif // VIEWSETTINGSDIALOG_H

