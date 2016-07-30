#include "controller.h"


Controller::Controller()
{

}

bool Controller::search(SearchCommand &command) {
    if (command.inFiles) {
        searchInFiles(command);
        setTabTitle("searched in files",false);
    }
    else {
        model->search(command);
        setTabTitle("->search",true);
    }
    model->notifyObservers();
    return true;
}

bool Controller::changeText(bool forward) {
    model->changeText(forward);
    if (forward)
        setTabTitle("->search",true);
    else
        setTabTitle(getTabTitle().left(getTabTitle().lastIndexOf("->search")),false);
    model->notifyObservers();
    return true;
}

bool Controller::load(QString xmlContent, std::set<ipAddress> addr) {
    bool valid = true;
    valid = valid && model->load(xmlContent,addr);
    if (valid) model->notifyObservers();
    return valid;
}

void Controller::clear() {
    model->clear();
    model->notifyObservers();
}

bool Controller::load() {
    QString filename = fileDialog.getOpenFileName(
        NULL,
        QObject::tr("Open Document"),
        QDir::currentPath(),
        QObject::tr("XML (*.xml)") );

    return loadFile(filename);
}

bool Controller::loadFile(QString filename) {
    bool valid = true;

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString content = QString::fromUtf8(file.readAll());
        if (loadXml(content)) {
            setTabTitle(filename,false);
            model->notifyObservers();
        }
        else
            valid = false;
        file.close();
    }
    else valid = (filename.size() == 0);

    return valid;
}

bool Controller::saveFileAs() {
    fileName = fileDialog.getSaveFileName(NULL,QObject::tr("Save"),"Capture",QObject::tr("XML (*.xml)"));
    if(fileName == "") return true;
    else return save();
}

bool Controller::saveFile() {
    if (fileName == "") return saveFileAs();
    else return save();
}

bool Controller::save() {
    QFile file(fileName);

    QString content = model->getContent();
    if (content.size() == 0) {
        setErrorMessage(QString("No sniffed information to save."),QMessageBox::Critical);
        return true;
    }

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(content.toUtf8());
        file.close();
        return true;
    }

    return false;
}

bool Controller::searchInFiles(SearchCommand &command) {
    QString finalResult;
    QStringList results;
    bool valid = true;

    valid = valid && getSearchResults(command,results);
    if (valid) joinXml(results, finalResult);
    return valid && loadXml(finalResult);
}

bool Controller::getSearchResults(SearchCommand &command, QStringList &results) {
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
        else
            valid = false;
    }
    return valid;
}

void Controller::joinXml(QStringList &xmlData, QString &joined) {
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

bool Controller::loadXml(QString &xmlContent) {
    std::set<ipAddress> addr;
    bool valid = true;

    valid = valid && extractor.extract(xmlContent,addr);
    return valid && model->load(xmlContent,addr);
}
