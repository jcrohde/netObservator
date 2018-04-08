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

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QFileDialog>
#include <QHBoxLayout>
#include "searchstrategy.h"
#include "util.h"


class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchDialog(SearchStrategy *strtgy, QWidget *parent = 0);

    void update(const viewSettings &set) {setting = set;}

    SearchStrategy *strategy;

    QStandardItemModel *tableModel;

private:
    QComboBox *columnBox, *modeBox;
    QPushButton *findBut;
    QLineEdit *findEdit;
    QCheckBox *invertBox;
    QPushButton *closeBut, *fileBut;

    QTableView *display;

    int addressIndex, nameIndex;

    viewSettings setting;

    void setCommand(SearchCommand &command);

    void updateTableModel(const std::set<ipAddress> &addresses);

    void enlistColumnBox();

protected:
    void showEvent(QShowEvent *e) {
        enlistColumnBox();

        QDialog::showEvent(e);
    }

private slots:
    void change();
    void setHost(const QModelIndex &index);

signals:
    void sig(SearchCommand &command);
};

#endif // SEARCHDIALOG_H
