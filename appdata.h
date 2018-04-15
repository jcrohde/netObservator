#ifndef APPDATA_H
#define APPDATA_H

#include "ippacket.h"
#include "settings.h"

#include <unordered_set>

struct Settings {
    int viewSettings;
    sniffSettings sniff;
};

void getSetting(Settings &settings);
void getDNS(std::unordered_set<addressItem> &addresses, std::vector<addressItem> &oldHosts);

QString generateAppDataFolder(const QString &foldername);

void writeDNS(const std::unordered_set<addressItem> &addresses);
void writeSettings(const Settings &settings);

#endif // APPDATA_H
