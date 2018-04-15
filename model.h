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

#ifndef MODEL_H
#define MODEL_H


#include <QFile>
#include "sniffthread.h"
#include "xmlserver.h"
#include "view.h"
#include "packetparser.h"
#include "observers.h"


class ViewModel : public serverObserver {
public:
    ViewModel() : view(nullptr), statisticsAttached(false) {}

    void setBytePlot(bool b);
    void setPacketPlot(bool b);
    void update(const viewSettings &set);
    void update(const serverState &state) {
        view->update(state);
        if (statisticsAttached)
            statisticsView->update(state);
    }

    void set(DatabaseView *v);
    void setStatistics(StatisticsView *sv) {statisticsView = sv;}

    void attachStatistics(bool attached) {statisticsAttached = attached;}

    DatabaseView *view;
    void setChartVisible(bool v) {if (view != nullptr) view->setChartVisible(v);}

private:
    bool statisticsAttached;
    parseInstruction instruction;

    StatisticsView *statisticsView;
};

class ServerModel : public SniffObserver {
public:
    ServerModel() : blockedBySniffThread(false), hasToClean(false) {}
    ~ServerModel();

    void set(XmlServer *s);

    void registerObserver(serverObserver *observer) {observers.push_back(observer);}
    void unregisterObserver(serverObserver *observer);

    bool search(SearchCommand &command);
    bool changeText(bool forward);

    void clear(QString title = "New");
    bool load(QString filename);
    bool loadSlice(QString slicename);
    bool loadFolder(QString folderName);
    bool save(QString fileName);

    void exportToXml(const QString &destination);
    void exportToJson(const QString &destination);

    bool isEmpty() {return server->isEmpty();}

    void update(const sniffState &state);

    const QStringList &getSliceNames() {return server->getSliceNames();}
    void getState(serverState & state) {server->getState(state);}

    PacketParser *parser;
private:
    XmlServer *server;

    bool blockedBySniffThread;
    bool hasToClean;
    QString foldername;
    std::vector<serverObserver*> observers;

    void notifyObservers(bool force = false);

    void load(QString xmlContent, std::set<ipAddress> addr);

    bool searchInFiles(SearchCommand &command);
    bool getSearchResults(SearchCommand &command, QStringList &results);
    void joinXml(QStringList &xmlData, QString &joined);
    bool loadXml(const QString &xmlContent);
};

class Model {
public:
    Model() {
        server.registerObserver(&view);
        sniff.registerObserver(&server);
        sniff.parser = &parser;

        std::vector<ipAddress> localAddresses;
        sniff.devs.getLocalIpAddresses(0,localAddresses);
        parser.update(localAddresses);

        server.parser = &parser;
    }

    ViewModel view;
    ServerModel server;
    SniffThread sniff;
    PacketParser parser;
};

#endif // MODEL_H
