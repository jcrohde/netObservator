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

#ifndef MODEL_H
#define MODEL_H


#include <QFile>
#include "sniffthread.h"
#include "xmlserver.h"
#include "view.h"


class ViewModel : public serverObserver {
public:
    ViewModel() {
        statisticsAttached = false;
    }

    void update(const Settings &set);
    void update(const serverState &state) {
        view->update(state);
        if (statisticsAttached)
            statisticsView->update(state);
    }

    void set(DatabaseView *v);
    void setStatistics(StatisticsView *sv) {statisticsView = sv;}

    void attachStatistics(bool attached) {statisticsAttached = attached;}

private:
    bool statisticsAttached;
    ViewComposition composition;
    Settings setting;
    DatabaseView *view;
    StatisticsView *statisticsView;
};

class ServerModel : public SniffObserver {
public:
    ServerModel() {blockedBySniffThread = false;}

    void set(XmlServer *s);

    void registerObserver(serverObserver *observer) {observers.push_back(observer);}
    void unregisterObserver(serverObserver *observer);

    bool search(SearchCommand &command);
    bool changeText(bool forward);

    void clear(QString title = "New");
    bool load(QString filename);
    bool loadSlice(QString slicename);
    bool save(QString fileName);

    void update(const sniffState &state);

    const QStringList &getSliceNames() {return server->getSliceNames();}

private:
    XmlServer *server;

    bool blockedBySniffThread;
    std::vector<serverObserver*> observers;

    XmlFilter filter;
    AddressExtractor extractor;

    void notifyObservers(bool force = false);

    void load(QString xmlContent, std::set<ipAddress> addr);

    bool searchInFiles(SearchCommand &command);
    bool getSearchResults(SearchCommand &command, QStringList &results);
    void joinXml(QStringList &xmlData, QString &joined);
    bool loadXml(QString &xmlContent);
};

class Model {
public:
    Model() {
        server.registerObserver(&view);
        sniff.registerObserver(&server);
    }

    ViewModel view;
    ServerModel server;
    SniffThread sniff;
};

#endif // MODEL_H
