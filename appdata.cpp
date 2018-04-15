#include "appdata.h"

#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QFile>
#include <QDir>

namespace {

const QString DNS = "dns";
const QString DOMAINNAME = "domainName";

const QString SETTINGS = "Settings";
const QString VIEW = "View";
const QString MODE = "Mode";
const QString STOREFOLDERNAME = "StoreFolderName";
const QString SLICEFILENAME = "SliceFileName";
const QString SLICESICE = "SliceSize";

void readSettings(const QString &content, Settings &settings) {
    QXmlStreamReader reader(content);

    if (reader.readNextStartElement()) {
        if (reader.name() == SETTINGS){
            reader.readNextStartElement();
            settings.viewSettings = reader.readElementText().toInt();
            reader.readNextStartElement();
            settings.sniff.mode = (sniffSettings::Mode)reader.readElementText().toInt();
            reader.readNextStartElement();
            settings.sniff.storeFolderName = reader.readElementText();
            reader.readNextStartElement();
            settings.sniff.sliceFileName = reader.readElementText();
            reader.readNextStartElement();
            settings.sniff.sliceSize = reader.readElementText().toInt();
        }
    }
}

void readDNS(QString content, std::unordered_set<addressItem> &addresses, std::vector<addressItem> &oldHosts) {
    QXmlStreamReader reader(content);

    int currentTime = static_cast<unsigned int>(std::time(nullptr));
    if (reader.readNextStartElement()) {
        if (reader.name() == DNS){
            while (reader.readNextStartElement()) {
                if (reader.name() == DOMAINNAME) {
                    QStringList entries;
                    while (reader.readNextStartElement()) {
                        entries.push_back(reader.readElementText());
                    }
                    if (entries.size() > 2) {
                        if (entries[2].toInt() > currentTime)
                            addresses.insert(addressItem(ipAddress(entries[0]),entries[1],entries[2].toInt()));
                        else {
                            addressItem item(entries[0],entries[1],0);
                            addresses.insert(item);
                            oldHosts.push_back(item);
                        }
                    }
                }
            }
        }
    }
}

}

void getSetting(Settings &settings) {
    QString filename = "/settings.xml";

    QStringList appDataFolders = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

    for (QString appDataFolder : appDataFolders) {
        QFile file(appDataFolder + filename);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString content = QString::fromUtf8(file.readAll());
            readSettings(content, settings);
            break;
        }
    }
}

void getDNS(std::unordered_set<addressItem> &addresses, std::vector<addressItem> &oldHosts) {
    QString filename = "/netObservator/DNS.xml";

    QStringList appDataFolders = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);

    for (QString appDataFolder : appDataFolders) {
        QFile file(appDataFolder + filename);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QString content = QString::fromUtf8(file.readAll());
            readDNS(content, addresses, oldHosts);
            break;
        }
    }
}

namespace {

void generateDNSDocument(QString &output, const std::unordered_set<addressItem> &addresses) {
    QXmlStreamWriter writer(&output);

    writer.writeStartDocument();
    writer.writeStartElement(DNS);

    for (std::unordered_set<addressItem>::const_iterator iter = addresses.begin(); iter != addresses.end(); ++iter) {
        if (iter->timeStamp > 0) {
            writer.writeStartElement(DOMAINNAME);
            writer.writeTextElement("IP",iter->address.toQString());
            writer.writeTextElement("Name",iter->hostname);
            writer.writeTextElement("ExpireTime",QString::number(iter->timeStamp));
            writer.writeEndElement();
        }
    }

    writer.writeEndElement();
}

void generateSettingsDocument(QString &output, const Settings &settings) {
    QXmlStreamWriter writer(&output);

    writer.writeStartDocument();
    writer.writeStartElement(SETTINGS);

    writer.writeTextElement(VIEW, QString::number(settings.viewSettings));
    writer.writeTextElement(MODE, QString::number((int)settings.sniff.mode));
    writer.writeTextElement(STOREFOLDERNAME, settings.sniff.storeFolderName);
    writer.writeTextElement(SLICEFILENAME, settings.sniff.sliceFileName);
    writer.writeTextElement(SLICESICE, QString::number(settings.sniff.sliceSize));

    writer.writeEndElement();
}

void store(const QString &filename, const QString &output) {
    QString appDataFolder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataFolder);
    if (!dir.exists())
        dir.mkpath(appDataFolder);
    QString path = dir.absoluteFilePath(filename);

    QFile file(path);

    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        file.write(output.toUtf8());
    }
}

}

QString generateAppDataFolder(const QString &foldername) {
    QString appDataFolder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataFolder);
    if (!dir.exists())
        dir.mkpath(appDataFolder);
    dir.mkdir(foldername);
    return dir.absoluteFilePath(foldername);
}

void writeDNS(const std::unordered_set<addressItem> &addresses) {
    QString output;

    generateDNSDocument(output, addresses);

    store("DNS.xml", output);
}

void writeSettings(const Settings &settings) {
    QString output;

    generateSettingsDocument(output, settings);

    store("settings.xml", output);
}
