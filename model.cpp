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


#include "model.h"
#include <QBuffer>
#include <QTextStream>

void ViewModel::setBytePlot(bool b) {
    instruction.plotByteNumber = b;
}

void ViewModel::setPacketPlot(bool b) {
    instruction.plotPacketNumber = b;
}

void ViewModel::update(const viewSettings &set) {
    instruction.settings = set;
    if (view != nullptr) view->update(instruction);
}

void ViewModel::set(DatabaseView *v) {
    view = v;
    view->getSettings(instruction);
}

void ServerModel::set(XmlServer *s) {
    server = s;
    notifyObservers(true);
}

void ServerModel::unregisterObserver(serverObserver *observer) {
    std::vector<serverObserver*>::iterator pos = std::find(observers.begin(),observers.end(),observer);
    if (pos != observers.end())
        observers.erase(pos);
}

void ServerModel::notifyObservers(bool force) {
    if (server->isChanged() || force) {
        serverState state;
        state.blockedBySniffThread = blockedBySniffThread;
        server->getState(state);

        for (auto observer: observers) {
            observer->update(state);
        }
    }
}

bool ServerModel::search(SearchCommand &command) {
    if (command.inFiles) {
        searchInFiles(command);
        //server->setTitle("searched in files");
    }
    else {
        server->search(command);
        notifyObservers();
    } 
    return true;
}

bool ServerModel::changeText(bool forward) {
    server->changeText(forward);
    notifyObservers();
    return true;
}

void ServerModel::load(QString xmlContent, std::set<ipAddress> addr) {
    server->clear();
    server->load(xmlContent,addr);
    notifyObservers();
}

bool ServerModel::loadFolder(QString folderName) {

    QStringList names;
    getPcapFileNames(folderName,names);

    if (names.size() > 0) {
        server->setTitle(folderName);
        server->loadFolder(folderName, names);
        loadXml(names.at(0));
        notifyObservers();
        return true;
    }

    return false;
}

void ServerModel::clear(QString title) {
    server->clear();
    server->setTitle(title);
    notifyObservers();
}

void ServerModel::update(const sniffState &state) {
    if (state.valid && state.sniffing) {
        blockedBySniffThread = true;
        clear("Sniffing");
    }
    else if (!state.sniffing) {
        blockedBySniffThread = false;
        loadFolder(state.folderName);
    }
}

bool ServerModel::load(QString filename) {
    bool valid = true;

    server->clear();
    if (loadXml(filename)) {
        server->setTitle(filename);
        notifyObservers();

    }
    valid = (filename.size() == 0);

    return valid;
}

bool ServerModel::loadSlice(QString slicename) {
    bool valid = true;

    if (loadXml(slicename)) {
        notifyObservers();
    }
    else {
        setErrorMessage("Can not open the file " + slicename, QMessageBox::Critical);
        valid = false;
    }

    return valid;
}

bool ServerModel::save(QString fileName) {
    bool valid = server->copy(fileName);
    /*QFile file(server->getContent());

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QFile destination(fileName);
        if (destination.open(QIODevice::WriteOnly|QIODevice::Text)) {
            destination.write(stream.readAll().toUtf8());
            destination.close();
            valid = true;
        }
        file.close();
    }*/
    /*if (content.size() == 0) {
        setErrorMessage(QString("No sniffed information to save."),QMessageBox::Warning);
        return true;
    }

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(content.toUtf8());
        file.close();
        return true;
    }*/

    if (!valid)
        setErrorMessage("Can not write on the file " + fileName, QMessageBox::Critical);
    return valid;
}

void ServerModel::exportToXml(const QString & destination) {
    server->exportFormat(destination, Mode::TOXML);
}

void ServerModel::exportToJson(const QString & destination) {
    server->exportFormat(destination, Mode::TOJSON);
}

bool ServerModel::searchInFiles(SearchCommand &command) {
    parser->search(command);

    QString str = command.filenames.at(command.filenames.size()-1);
    str = str.left(str.lastIndexOf("/")) + "/Search";
    return loadFolder(str);
}

bool ServerModel::getSearchResults(SearchCommand &command, QStringList &results) {
    results.clear();
    bool valid = true;

    for (int i = 0; i < command.filenames.size() && valid; i++) {
        QFile file(command.filenames.at(i));
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString result;
            if (valid) results.append(result);
            file.close();
        }
        else {
            setErrorMessage("Can not open the file " + command.filenames.at(i), QMessageBox::Critical);
            valid = false;
        }
    }
    return valid;
}

void ServerModel::joinXml(QStringList &xmlData, QString &joined) {
    int index;
    joined.clear();
    for (int i = 0; i < xmlData.size(); i++) {
        i > 0 ? index = joined.lastIndexOf("</" + SNIFFED + ">") : index = -1;
        if (index > 0) {
            int index2 = xmlData.at(i).indexOf("<" + SNIFFED + ">");
            if (index2 > 0) {
                joined = joined.left(index);
                joined.append(xmlData.at(i).right(xmlData.at(i).size()-xmlData.at(i).indexOf("<" + SNIFFED + ">")-SNIFFED.size()-2));
            }
        }
        else joined = xmlData.at(i);
    }
}

bool ServerModel::loadXml(const QString &xmlContent) {
    std::set<ipAddress> addr;

    parser->extract(xmlContent,addr);
    server->load(xmlContent,addr);
    return true;
}



