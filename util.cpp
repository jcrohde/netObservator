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

#include <QIcon>
#include "util.h"

Settings::Settings() {
    for (int i = 0; i < COLUMNNUMBER; i++) {
        showInfo[i] = i == PORT || i == ADDRESS || i == HOSTNAME;
    }

    save = true;
    duration = 20;
}

ipAddress::ipAddress(QString str) {
    getByteFromString(byte1, str);
    getByteFromString(byte2, str);
    getByteFromString(byte3, str);

    byte4 = str.toInt();
}

void ipAddress::getByteFromString(u_char &byte, QString &str) {
    byte = str.left(str.indexOf(".")).toInt();
    str = str.right(str.size()-str.indexOf(".")-1);
}

unsigned int ipAddress::length(u_char byte) const {
    if (byte < 10) return 1;
    else if (byte <100) return 2;
    else return 3;
}

void ipAddress::print(char *&str) const {
    int size = length(byte1)+length(byte2)+length(byte3)+length(byte4)+4;
    str = (char*) malloc(size);
    sprintf(str,"%d.%d.%d.%d",byte1,byte2,byte3,byte4);
}

QString ipAddress::toQString() const {
    char *c;
    print(c);
    QString res(c);
    free(c);
    return res;
}

QString escape(QString content) {
    content.replace(QString("<"),QString("&lt;"));
    content.replace(QString(">"),QString("&gt;"));
    content.replace(QString("&"),QString("&amp;"));
    content.replace(QString("'"),QString("&apos;"));
    content.replace(QString("\""),QString("&quot;"));
    return content;
}

QString unEscape(QString content) {
    content.replace(QString("&lt;"),QString("<"));
    content.replace(QString("&gt;"),QString(">"));
    content.replace(QString("&amp;"),QString("&"));
    content.replace(QString("&apos;"),QString("'"));
    content.replace(QString("&quot;"),QString("\""));
    return content;
}

void setErrorMessage(QString message,QMessageBox::Icon icon) {
    QMessageBox box;
    if (icon == QMessageBox::Critical) box.setWindowTitle("Error");
    else box.setWindowTitle("Warning");
    box.setWindowIcon(QIcon("icons/observerLogo.png"));
    box.setIcon(icon);
    box.setText(message);
    box.setWindowFlags(box.windowFlags() | Qt::WindowStaysOnTopHint);
    box.exec();
}
