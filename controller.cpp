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

#include "controller.h"

#include "appdata.h"


Controller::Controller(Model *m)
    : model(m),
      packetTrafficView(TrafficPacketInfoPresenter::infoType::PACKETS),
      byteTrafficView(TrafficPacketInfoPresenter::infoType::BYTES)
{
    Settings set;
    getSetting(set);

    settings = new SettingsDialog(set.sniff);
    viewSettingsDialog = new ViewSettingsDialog(set.viewSettings);
    statisticsDialog = new StatisticsDialog(&statisticsView);
    packetTrafficDialog = new TrafficDialog(&packetTrafficView);
    byteTrafficDialog = new TrafficDialog(&byteTrafficView);
    searchTabDialog = new SearchDialog(&tabStrategy);
    searchFilesDialog = new SearchDialog(&filesStrategy);
    filterEditor = new PacketFilterEditor;

    model->view.setStatistics(&statisticsView);

    model->server.registerObserver(&tabStrategy);
    model->sniff.registerObserver(&filesStrategy);
    model->sniff.registerObserver(packetTrafficDialog);
    model->sniff.registerObserver(byteTrafficDialog);
    model->sniff.update(set.sniff);

    packetTrafficView.parser = &model->parser;
    byteTrafficView.parser = &model->parser;
    statisticsView.parser = &model->parser;

    QObject::connect(searchTabDialog,SIGNAL(sig(SearchCommand&)),this,SLOT(search(SearchCommand&)));
    QObject::connect(&tabStrategy,SIGNAL(browse(bool)),this,SLOT(changeText(bool)));
    QObject::connect(searchFilesDialog,SIGNAL(sig(SearchCommand&)),this,SLOT(search(SearchCommand&)));
    QObject::connect(viewSettingsDialog,SIGNAL(change(viewSettings&)),this,SLOT(getSettings(viewSettings&)));
    QObject::connect(settings,SIGNAL(change(sniffSettings&)),this,SLOT(getSniffSettings(sniffSettings&)));
    QObject::connect(filterEditor,SIGNAL(commit(QString)),this,SLOT(getFilter(QString)));

    QObject::connect(statisticsDialog,SIGNAL(closeSignal()),this,SLOT(removeStatisticsView()));
    QObject::connect(packetTrafficDialog,SIGNAL(closeSignal()),this,SLOT(removePacketView()));
    QObject::connect(byteTrafficDialog,SIGNAL(closeSignal()),this,SLOT(removeByteView()));

    QObject::connect(this,SIGNAL(packets(int)),packetTrafficDialog,SLOT(getForPlot(int)));
    QObject::connect(this,SIGNAL(bytes(int)),byteTrafficDialog,SLOT(getForPlot(int)));

    model->parser.firePacketNumber = [&](traffic &data) {emit packets(data.packetNumber);};
    model->parser.fireByteNumber = [&](traffic &data) {emit bytes(data.packetNumber);};
    viewSettingsDialog->updateObservers();
}

Controller::~Controller() {
    Settings set;
    settings->getSettings(set.sniff);
    set.viewSettings = viewSettingsDialog->getSettings();
    writeSettings(set);
}

void Controller::setModelView(modelView *mv) {
    model->view.set(&mv->view);
    statisticsView.getContent = mv->view.getContent;
    packetTrafficView.getContent = mv->view.getContent;
    byteTrafficView.getContent = mv->view.getContent;
    mv->view.parser = &model->parser;
    mv->model.parser = &model->parser;
    model->parser.setPacketInfoPresenter(&mv->view.tablePacketInfo);
    model->server.set(&mv->model); 
}

bool Controller::isSomeDialogOpened() {
    return fileDialog.isVisible()
        || (searchTabDialog && searchTabDialog->isVisible())
        || (searchFilesDialog && searchFilesDialog->isVisible())
        || (settings && settings->isVisible())
        || (filterEditor && filterEditor->isVisible());
}

void Controller::executeCommand(Command cmd) {
    CommandCode command = cmd.code;

    if (command == CommandCode::LOAD)
        load();
    else if (command == CommandCode::LOADFOLDER)
        loadFolder();
    else if (command == CommandCode::LOADFILE)
        model->server.load(cmd.arguments[0]);
    else if (command == CommandCode::ADDTAB)
        emit addTab();
    else if (command == CommandCode::CLEAR)
        model->server.clear();
    else if (command == CommandCode::SAVEFILE)
        saveFile();
    else if (command == CommandCode::SAVEFILEAS)
        saveFileAs();
    else if (command == CommandCode::EXPORTXML) {
        QString destination = enterDestination("Export to XML", "xml");
        if (destination.size() > 0) model->server.exportToXml(destination);
    }
    else if (command == CommandCode::EXPORTJSON) {
        QString destination = enterDestination("Export to json", "json");
        if (destination.size() > 0) model->server.exportToJson(destination);
    }
    else if (command == CommandCode::CHARTVISIBLE || command == CommandCode::CHARTINVISIBLE) {
        model->view.setChartVisible(command == CommandCode::CHARTVISIBLE);
    }
    else if (command == CommandCode::BACK)
        model->server.changeText(false);
    else if (command == CommandCode::FORWARD)
        model->server.changeText(true);
    else if (command == CommandCode::SHOWPACKETFILTEREDITOR)
        filterEditor->show();
    else if (command == CommandCode::SHOWSEARCHTABDIALOG)
        searchTabDialog->show();
    else if (command == CommandCode::SHOWSEARCHFILEDIALOG)
        searchFilesDialog->show();
    else if (command == CommandCode::SHOWSETTINGSDIALOG)
        settings->show();
    else if (command == CommandCode::SHOWCOLUMNSETTINGSDIALOG)
        viewSettingsDialog->show();
    else if (command == CommandCode::SHOWSTATISTICSDIALOG) {
        model->view.attachStatistics(true);
        statisticsView.setSliceNames(model->server.getSliceNames());
        statisticsDialog->show();
    }
    else if (command == CommandCode::LOADSLICE)
        model->server.loadSlice(cmd.arguments[0]);
    else if (command == CommandCode::SHOWTRAFFICDIALOG) {
        model->server.registerObserver(&packetTrafficView);
        packetTrafficView.setSliceNames(model->server.getSliceNames());
        packetTrafficDialog->show();
        model->view.setPacketPlot(true);
    }
    else if (command == CommandCode::SHOWBYTEDIALOG) {
        model->server.registerObserver(&byteTrafficView);
        byteTrafficView.setSliceNames(model->server.getSliceNames());
        byteTrafficDialog->show();
        model->view.setBytePlot(true);
    }
    else if (command == CommandCode::CHANGEDEVICE) {
        std::vector<ipAddress> localAddresses;
        model->sniff.devs.getLocalIpAddresses(cmd.arguments[0].toInt(),localAddresses);
        model->parser.update(localAddresses);
    }
    else if (command == CommandCode::SNIFF)
        handleSniffing(cmd);
}

void Controller::load() {
    QString filename = fileDialog.getOpenFileName(
        NULL,
        QObject::tr("Open Document"),
        QDir::currentPath(),
        QObject::tr("pcap (*.pcap)") );

    if (!filename.isEmpty() && !model->server.isEmpty())
        addTab();
    model->server.load(filename);
}

void Controller::loadFolder() {
    QString folderName = fileDialog.getExistingDirectory(
        NULL,
        QObject::tr("Open Document"),
        QDir::currentPath() );

    if (!folderName.isEmpty() && !model->server.isEmpty())
        addTab();
    model->server.loadFolder(folderName);
}

void Controller::saveFile() {
    if (lastSavedFileName.size() > 0)
        model->server.save(lastSavedFileName);
    else
        saveFileAs();
}

void Controller::saveFileAs() {
    QString fileName = fileDialog.getSaveFileName(NULL,QObject::tr("Save"),"Capture",QObject::tr("pcap (*.pcap)"));
    if(fileName.size() > 0) {
        model->server.save(fileName);
        lastSavedFileName = fileName;
    }
}

QString Controller::enterDestination(const QByteArray &title, const QByteArray &filetype) {
    QString destination = "";

    serverState state;
    model->server.getState(state);
    if (state.sliceNames.size() == 0)
        destination = fileDialog.getSaveFileName(NULL,QObject::tr(title), state.title.left(state.title.size() - 5), QObject::tr(filetype + " (*." + filetype + ")"));
    else
        destination = fileDialog.getSaveFileName(NULL, QObject::tr(title), state.title);
    return destination;
}

void Controller::handleSniffing(Command cmd) {
    if (model->sniff.isSniffing())
        model->sniff.stop();
    else {
        if (statisticsDialog && statisticsDialog->isVisible())
            setErrorMessage(QString("One cannot start to sniff while the Statistics Dialog is opened."),QMessageBox::Warning);
        else {
            model->parser.setPacketInfoPresenter(&model->view.view->tablePacketInfo);
            model->sniff.start(cmd.arguments[0].toInt());
        }
    }
}

void Controller::getCommand(Command command) {
    if (command.code != CommandCode::SNIFF && model->sniff.isSniffing()) {
        setErrorMessage(QString("The selected action cannot be performed while the program is sniffing."),QMessageBox::Warning);
    }
    else if (isSomeDialogOpened()) {
        setErrorMessage(QString("The selected action cannot be performed while some other sub dialog is opened."),QMessageBox::Warning);
    }
    else
        executeCommand(command);
}

void Controller::getComposition(ViewComposition &composition) {

}

void Controller::search(SearchCommand &command) {
    model->server.search(command);
}

void Controller::changeText(bool forward) {
    model->server.changeText(forward);
}

void Controller::getSettings(viewSettings &set) {
    model->view.update(set);
    searchTabDialog->update(set);
    searchFilesDialog->update(set);
}

void Controller::getSniffSettings(sniffSettings &settings) {
    model->sniff.update(settings);
    model->parser.setDurationTime(settings.duration);
}
