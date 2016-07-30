#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QFileDialog>
#include "model.h"


class Controller
{
public:
    Controller();

    bool search(SearchCommand &command);
    bool changeText(bool forward);
    bool load(QString xmlContent, std::set<ipAddress> addr);

    void clear();
    bool load();
    bool loadFile(QString filename);
    bool saveFileAs();
    bool saveFile();

    QFileDialog fileDialog;

    void setModel(XmlServer *server) {model = server;}

    std::function<void(QString,bool)> setTabTitle;
    std::function<QString()> getTabTitle;

private:
    XmlServer *model;

    QString fileName;

    XmlFilter filter;
    AddressExtractor extractor;

    bool save();

    bool searchInFiles(SearchCommand &command);
    bool getSearchResults(SearchCommand &command, QStringList &results);
    void joinXml(QStringList &xmlData, QString &joined);
    bool loadXml(QString &xmlContent);
};

#endif // CONTROLLER_H
