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

#ifndef VIEWDIALOG_H
#define VIEWDIALOG_H


#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QScrollArea>
#include "util.h"
#include "view.h"

class StatisticsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StatisticsDialog(StatisticsView *statistics, QWidget *parent = 0);

private:
    QComboBox *selection;

    QPushButton *cancelBut;

    QPixmap *map;
    QLabel *label;
    QLabel *timeLineTitle;
    QScrollArea *scr;

    StatisticsView *statisticsView;

    void generatePresentationSection(QHBoxLayout *settingLayout);
    void generateButtonSection(QVBoxLayout *ordering);
    void connectSignalsAndSlots();

    void visualize(std::vector<sizedPair> &results, long int begin, long int end);
    QString evaluateTimeStamp(long int timeStamp);

protected:
    void showEvent(QShowEvent *e) {
        statisticsView->update(PROTOCOL);
        QDialog::showEvent(e);
    }

    void closeEvent(QCloseEvent *e) {
        emit closeSignal();
        QDialog::closeEvent(e);
    }

private slots:
    void changeView(int index);

signals:
    closeSignal();
    change(ViewComposition&);
};

#endif // VIEWDIALOG_H
