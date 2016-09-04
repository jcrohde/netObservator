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


#include "model.h"
#include <QBuffer>

void ViewModel::update(const Settings &set) {
    setting = set;
    view->update(set);
}

void ViewModel::set(DatabaseView *v) {
    view = v;
    view->getSettings(setting);
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
    if (server->change() || force) {
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
        server->setTitle("searched in files");
    }
    else {
        server->search(command);
    }
    notifyObservers();
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
        server->clear();
        server->setSniffed(state.sliceNames);
        server->setTitle("Sniffed");
        blockedBySniffThread = false;
        if (state.sliceNames.size() > 0)
            loadSlice(state.sliceNames[0]);
        else
            clear("Sniffed");
    }
}

bool ServerModel::load(QString filename) {
    bool valid = true;

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());
        server->clear();
        if (loadXml(content)) {
            server->setTitle(filename);
            notifyObservers();
        }
        else {
            setErrorMessage("Can not open the file " + filename, QMessageBox::Critical);
            valid = false;
        }
        file.close();
    }
    else valid = (filename.size() == 0);

    return valid;
}

bool ServerModel::loadSlice(QString slicename) {
    bool valid = true;

    QFile file(slicename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());
        if (loadXml(content)) {
            notifyObservers();
        }
        else {
            setErrorMessage("Can not open the file " + slicename, QMessageBox::Critical);
            valid = false;
        }
        file.close();
    }
    else valid = (slicename.size() == 0);

    return valid;
}

bool ServerModel::save(QString fileName) {
    QFile file(fileName);

    QString content = server->getContent();
    if (content.size() == 0) {
        setErrorMessage(QString("No sniffed information to save."),QMessageBox::Warning);
        return true;
    }

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(content.toUtf8());
        file.close();
        return true;
    }

    setErrorMessage("Can not write on the file " + fileName, QMessageBox::Critical);
    return false;
}

bool ServerModel::searchInFiles(SearchCommand &command) {
    QString finalResult;
    QStringList results;
    bool valid = true;

    valid = valid && getSearchResults(command,results);
    if (valid) joinXml(results, finalResult);
    if (valid) server->clear();
    return valid && loadXml(finalResult);
}

bool ServerModel::getSearchResults(SearchCommand &command, QStringList &results) {
    results.clear();
    bool valid = true;

    for (int i = 0; i < command.filenames.size() && valid; i++) {
        QFile file(command.filenames.at(i));
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString content = QString::fromUtf8(file.readAll());
            QString result;
            valid = valid && filter.generateFilteredXmlDocument(command,content,result);
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

bool ServerModel::loadXml(QString &xmlContent) {
    std::set<ipAddress> addr;
    bool valid =  extractor.extract(xmlContent,addr);
    if (valid)
        server->load(xmlContent,addr);
    return valid;
}



