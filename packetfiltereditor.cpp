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

#include "packetfiltereditor.h"

#include <QLabel>
#include <QList>
#include <QGridLayout>
#include "dnssingleton.h"
#include <QMenu>

static const int minButtomButtonHeight = 24;

TypeSelectionStrategy::TypeSelectionStrategy(QWidget *parent) : QHBoxLayout(parent) {}

ProtocolSelectionStrategy::ProtocolSelectionStrategy(QWidget *parent) : TypeSelectionStrategy(parent) {
    icmpBox = new QCheckBox("ICMP");
    igmpBox = new QCheckBox("IGMP");
    tcpBox = new QCheckBox("TCP");
    udpBox = new QCheckBox("UDP");

    addWidget(icmpBox);
    addWidget(igmpBox);
    addWidget(tcpBox);
    addWidget(udpBox);
}

void ProtocolSelectionStrategy::getTextList(QList<QString> &list) {
    list.clear();
    if (icmpBox->isChecked()) list.append("icmp");
    if (igmpBox->isChecked()) list.append("igmp");
    if (tcpBox->isChecked()) list.append("tcp");
    if (udpBox->isChecked()) list.append("udp");
}

PortSelectionStrategy::PortSelectionStrategy(QWidget *parent) : TypeSelectionStrategy(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *selectLayout = new QHBoxLayout();

    selectLayout->addWidget(new QLabel("portnumber:"));

    edit = new QLineEdit();
    edit->setMaximumWidth(50);
    selectLayout->addWidget(edit);

    okBut = new QPushButton("Ok");
    selectLayout->addWidget(okBut);

    mainLayout->addLayout(selectLayout);

    listWidget = new QListWidget();
    listWidget->setMaximumHeight(75);
    listWidget->setMaximumWidth(200);
    mainLayout->addWidget(listWidget);

    addLayout(mainLayout);

    setSizeConstraint(QLayout::SetFixedSize);

    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));
    connect(okBut,SIGNAL(clicked()),this,SLOT(addPort()));
}

void PortSelectionStrategy::getTextList(QList<QString> &list) {
    list.clear();
    for (int i = 0; i < listWidget->count(); i++)
        list.append("port " + listWidget->item(i)->text());
}

void PortSelectionStrategy::addPort() {
    QListWidgetItem *item = new QListWidgetItem(edit->text());
    listWidget->addItem(item);
}

void PortSelectionStrategy::showContextMenu(const QPoint& point) {
    QPoint globalPoint = listWidget->mapToGlobal(point);

    QMenu menu;
    menu.addAction("delete",  this, SLOT(eraseItem()));

    menu.exec(globalPoint);
}

void PortSelectionStrategy::eraseItem()
{
    for (int i = 0; i < listWidget->selectedItems().size(); ++i) {
        QListWidgetItem *item = listWidget->takeItem(listWidget->currentRow());
        delete item;
    }
}

AddressSelectionStrategy::AddressSelectionStrategy(QWidget *parent) : TypeSelectionStrategy(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *selectLayout = new QHBoxLayout();

    selectLayout->addWidget(new QLabel("ip address:"));

    edit = new QLineEdit();
    edit->setMaximumWidth(50);
    selectLayout->addWidget(edit);

    okBut = new QPushButton("Ok");
    selectLayout->addWidget(okBut);

    mainLayout->addLayout(selectLayout);

    listWidget = new QListWidget();
    listWidget->setMaximumHeight(75);
    listWidget->setMaximumWidth(200);
    mainLayout->addWidget(listWidget);

    addLayout(mainLayout);

    setSizeConstraint(QLayout::SetFixedSize);

    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(listWidget,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(showContextMenu(const QPoint&)));
    connect(okBut,SIGNAL(clicked()),this,SLOT(addAddress()));
}

void AddressSelectionStrategy::getTextList(QList<QString> &list) {
    list.clear();
    for (int i = 0; i < listWidget->count(); i++)
        list.append("host " + listWidget->item(i)->text());
}

void AddressSelectionStrategy::addAddress() {
    QListWidgetItem *item = new QListWidgetItem(edit->text());
    listWidget->addItem(item);
}

void AddressSelectionStrategy::showContextMenu(const QPoint& point) {
    QPoint globalPoint = listWidget->mapToGlobal(point);

    QMenu menu;
    menu.addAction("delete",  this, SLOT(eraseItem()));

    menu.exec(globalPoint);
}

void AddressSelectionStrategy::eraseItem()
{
    for (int i = 0; i < listWidget->selectedItems().size(); ++i) {
        QListWidgetItem *item = listWidget->takeItem(listWidget->currentRow());
        delete item;
    }
}

TypeSelectionFrame::TypeSelectionFrame(TypeSelectionStrategy *str, QWidget *parent) : QGroupBox(parent), strategy(str) {
    setTitle(strategy->getTitle());
    setFlat(true);
    setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    blackWhiteBox = new QComboBox();
    blackWhiteBox->addItem("black list");
    blackWhiteBox->addItem("white list");

    QVBoxLayout *frameLayout = new QVBoxLayout();
    QHBoxLayout *blackWhiteLayout = new QHBoxLayout();

    blackWhiteLayout->addWidget(blackWhiteBox);
    blackWhiteLayout->addStretch();
    frameLayout->addLayout(blackWhiteLayout);

    frameLayout->addStretch();

    frameLayout->addLayout(strategy);

    frameLayout->addStretch();

    setLayout(frameLayout);
}

void TypeSelectionFrame::getText(QString &text) {
    QList<QString> list;
    strategy->getTextList(list);
    text.clear();

    int size = list.size();
    for (int i = 0; i <size; i++) {
        if (i > 0) {
            if (blackWhiteBox->currentIndex()) text += " or ";
            else text += " and ";
        }
        if (!blackWhiteBox->currentIndex() && size > 1)
            text += "(";
        if (!blackWhiteBox->currentIndex())
            text += "not ";
        text += list[i];
        if (!blackWhiteBox->currentIndex() && size > 1)
            text += ")";
    }
}

PacketFilterEditor::PacketFilterEditor(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Packet Filter");
    setWindowIcon(QIcon("icons/observerLogo.png"));

    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    tested = true;
    testButton = new QPushButton("Test");
    testButton->setMinimumHeight(minButtomButtonHeight);
    QPushButton *insertButton = new QPushButton("Insert");

    okButton = new QPushButton("Ok");
    okButton->setMinimumHeight(minButtomButtonHeight);
    closeButton = new QPushButton("Close");
    closeButton->setMinimumHeight(minButtomButtonHeight);

    filterEdit = new QTextEdit();

    QVBoxLayout *mainLayout = new QVBoxLayout();

    TypeSelectionStrategy *protocolStrategy = new ProtocolSelectionStrategy();
    TypeSelectionStrategy *portStrategy = new PortSelectionStrategy();
    TypeSelectionStrategy *addressStrategy = new AddressSelectionStrategy();

    typeSelection[0] = new TypeSelectionFrame(protocolStrategy,this);
    typeSelection[1] = new TypeSelectionFrame(portStrategy,this);
    typeSelection[2] = new TypeSelectionFrame(addressStrategy,this);

    QGridLayout *selectionLayout = new QGridLayout();

    for (int i = 0; i < 3; i++)
        selectionLayout->addWidget(typeSelection[i],0,i);

    mainLayout->addLayout(selectionLayout);

    QHBoxLayout *insertLayout = new QHBoxLayout();
    insertLayout->addWidget(insertButton);
    insertLayout->addStretch();

    mainLayout->addLayout(insertLayout);
    mainLayout->addWidget(filterEdit);

    QHBoxLayout *butLayout = new QHBoxLayout();
    butLayout->addWidget(testButton);
    butLayout->addStretch();
    butLayout->addWidget(okButton);
    butLayout->addWidget(closeButton);
    mainLayout->addLayout(butLayout);

    setLayout(mainLayout);

    setFixedWidth(width()+30);

    connect(testButton,SIGNAL(clicked(bool)),this,SLOT(test()));
    connect(insertButton,SIGNAL(clicked(bool)),this,SLOT(insert()));
    connect(filterEdit,SIGNAL(textChanged()),this,SLOT(changeTestIcon()));
    connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(okButton,SIGNAL(clicked()),this,SLOT(onOk()));
}

void PacketFilterEditor::changeTestIcon() {
    if (filterEdit->toPlainText().isEmpty()) {
        testButton->setIcon(QIcon());
        tested = true;
    }
    else if (tested) {
        testButton->setIcon(QIcon("icons/testQuestion.png"));
        tested = false;
    }
}

void PacketFilterEditor::test() {
    QString text = filterEdit->toPlainText();
    std::string str = text.toStdString();
    char *filter = new char [str.size()+1];
    strcpy( filter, str.c_str() );
    char error[PCAP_ERRBUF_SIZE];

    pcap_if_t *alldevs;
    if(pcap_findalldevs(&alldevs,error)==-1) {
        setErrorMessage("Error in finding devices: " + QString(error),QMessageBox::Critical);
    }

    pcap_t *tester = pcap_open_dead(1,BUFSIZ);
    if (tester != NULL) {
        bpf_u_int32 net = 0xffffff;
        bpf_program prog;
        int i = pcap_compile(tester,&prog,filter,0,net);

        tested = true;
        if (i == -1) {
            char *err = pcap_geterr(tester);
            testButton->setIcon(QIcon("icons/testFail.png"));
            setErrorMessage(QString(err), QMessageBox::Critical);
        }
        else
            testButton->setIcon(QIcon("icons/testSuccess.png"));
    pcap_close(tester);
    }
}

void PacketFilterEditor::insert() {
    QString text;
    QList<QString> texts;
    for (int i = 0; i < 3; i++) {
        QString help;
        typeSelection[i]->getText(help);
        if (help.size() > 0)
            texts.append(help);
    }
    if (texts.size()>1) {
        for (int i = 0; i < texts.size(); i++) {
            if (i>0) text += " and ";
            text += "(";
            text += texts[i];
            text += ")";
        }
    }
    else if (texts.size() == 1)
        text = texts[0];

    filterEdit->setText(text);
}

void PacketFilterEditor::onOk() {
    emit commit(filterEdit->toPlainText());
    close();
}


