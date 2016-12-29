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
#include <QDir>
#include <QFileInfoList>

std::mutex myCellMutex;

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

void getPcapFileNames(const QString &folderName, QStringList &fileNames) {
    fileNames.clear();

    QDir dir(folderName);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); i++) {
        if (list.at(i).fileName().contains(".pcap"))
            fileNames.append(list.at(i).absoluteFilePath());
    }
}
