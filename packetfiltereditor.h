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

#ifndef PACKETFILTEREDITOR_H
#define PACKETFILTEREDITOR_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include "util.h"
#include <QTextEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidget>

class TypeSelectionStrategy : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit TypeSelectionStrategy(QWidget *parent = 0);

    virtual QString getTitle() = 0;
    virtual void getTextList(QList<QString> &list) = 0;
};

class ProtocolSelectionStrategy : public TypeSelectionStrategy
{
    Q_OBJECT
public:
    explicit ProtocolSelectionStrategy(QWidget *parent = 0);

    QString getTitle() {return "Protocol";}
    void getTextList(QList<QString> &list);
private:
    QCheckBox *icmpBox, *igmpBox, *tcpBox, *udpBox;
};

class PortSelectionStrategy : public TypeSelectionStrategy
{
    Q_OBJECT
public:
    explicit PortSelectionStrategy(QWidget *parent = 0);

    QString getTitle() {return "Port";}
    void getTextList(QList<QString> &list);
private:
    QLineEdit *edit;
    QPushButton *okBut;
    QListWidget *listWidget;

private slots:
    void addPort();
    void showContextMenu(const QPoint& point);
    void eraseItem();
};

class AddressSelectionStrategy : public TypeSelectionStrategy
{
    Q_OBJECT
public:
    explicit AddressSelectionStrategy(QWidget *parent = 0);

    QString getTitle() {return "Address";}
    void getTextList(QList<QString> &list);
private:
    QLineEdit *edit;
    QPushButton *okBut;
    QListWidget *listWidget;

private slots:
    void addAddress();
    void showContextMenu(const QPoint& point);
    void eraseItem();
};

class TypeSelectionFrame : public QGroupBox
{
    Q_OBJECT
public:
    explicit TypeSelectionFrame(TypeSelectionStrategy *str, QWidget *parent = 0);

    void getText(QString &text);

private:
    QComboBox *blackWhiteBox;
    TypeSelectionStrategy *strategy;
};

class PacketFilterEditor : public QDialog
{
    Q_OBJECT
public:
    explicit PacketFilterEditor(QWidget *parent = 0);

private:
    bool tested;
    QPushButton *testButton;
    QTextEdit *filterEdit;
    TypeSelectionFrame *typeSelection[3];
    QPushButton *okButton, *closeButton;

private slots:
    void changeTestIcon();
    void test();
    void insert();
    void onOk();
signals:
    void commit(QString);
};

#endif // PACKETFILTEREDITOR_H
