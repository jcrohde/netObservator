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

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include "util.h"

class SearchDialog : public QDialog, public SettingObserver, public ModelObserver
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

    void setCommand(SearchCommand &command);

    void update(modelState state) {
        enableBackForwardButtons(!state.firstDocument,!state.lastDocument);
    }

private:

    QComboBox *columnBox;
    QPushButton *findBut;
    QLineEdit *findEdit;
    QCheckBox *invertBox, *regexBox;
    QPushButton *backBut, *forwardBut, *closeBut;

    void enableBackForwardButtons(bool backEnable, bool forwardEnable);
    void enlistColumnBox();

protected:
    void showEvent(QShowEvent *e) {
        enlistColumnBox();

        QDialog::showEvent(e);
    }

private slots:
    void change();
    void back();
    void forward();

signals:
    void browse(bool);
    void sig();
};

#endif // SEARCHDIALOG_H
