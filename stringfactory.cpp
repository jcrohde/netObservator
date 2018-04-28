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

#include "stringfactory.h"
#include "settings.h"

StringFactory::StringFactory()
{

}

QString StringFactory::getStringFromKey(stringKey key) {
    switch (key) {
    case stringKey::PROGRAMNAME:
        return getProgramName();
        break;
    case stringKey::PROGRAMNAMELONG:
        return getProgramNameLong();
        break;
    case stringKey::VERSION:
        return getVersion();
        break;
    case stringKey::HELP:
        return getHelp();
        break;
    case stringKey::LICENSE:
        return getLicense();
        break;
    case stringKey::ABOUT:
        return getAbout();
        break;
    default:
        return "";
        break;
    }
}

QString StringFactory::getTitleFromKey(stringKey key) {
    switch (key) {
    case stringKey::HELP:
        return "Help";
        break;
    case stringKey::LICENSE:
        return "License";
        break;
    case stringKey::ABOUT:
        return "About";
        break;
    default:
        return "";
        break;
    }
}

QString StringFactory::getProgramName() {
    return "netObservator";
}

QString StringFactory::getProgramNameLong() {
    QString longName = getProgramName();
    longName += " " + getVersion();
    return longName;
}

QString StringFactory::getVersion() {
    return "2.0";
}

QString StringFactory::getTitle() {
    QString title = "<font color=\"#0000FF\"><h1>";
    title += getProgramNameLong();
    title += "</h1></font>";
    return title;
}

QString StringFactory::getHelp() {
    QString content = getTitle();
    content += getProgramName() + " gives you an overview of the hosts your computer is connected with. Moreover " + getProgramName() + " provides support to analyse your connections. ";
    content += getProgramName() + " is a simple to use sniffer app. You can visualize the traffic by the plotting dialogs or the interactive statistics dialog.";

    content += "<h2>Quick Start</h2>";
    content += "For a quick start just select the device of your internet connection and click the \"Run\"-button. Then the program starts to sniff. ";
    content += "If you press this button a second time, the program will stop to sniff.<br><br>";

    content += "When you start to sniff with " + getProgramName() +  ", only information about the first packet of a connection with another host will be displayed. ";
    content += "This information will disappear some time after the last packet from or to this host has been sniffed. Thus you see the hosts you are currently connected with. After sniffing you get a chart of the sniffed packets by hostname and protocol.<br><br>";

    content += "The information about the sniffed packets are divided into different files (slices). The reason is that depending on your traffic and the memory of your system " + getProgramName();
    content += " might get problems to allocate enough memory to process files containing all information. If you have selected \"save\" in settings a combo Box will appear to support you to select and to display the slices of the recently sniffed packets.<br><br>";

    content += "Green background color means that the packet is incoming. ";
    content += "Red means just that the packet is outgoing, which is not necessarily dangerous. Blue means that the device you are sniffing on is neither the source nor the destination of the sniffed packet. ";
    content += "The saturation depends on the protocol: low (TCP, UDP), middle(IGMP, ICMP), high(other)";

    content += "<h2>File Menu</h2>";
    content += "By clicking on \"Save\" or \"SaveAs\" in the File menu, you can save the displayed information in a .pcap file. \"New\" adds a new tab. ";
    content += "Moreover by clicking on \"Open File\", you can open a .pcap file and information about all packets of this file will be displayed and you can search on them. ";
    content += "By \"Open Folder\", you can get the pcap files of an entire folder and visualize the statistics of all files with the statistics dialog. Moreover you can also plot the traffic over all files.<br>";
    content += "In addition you can also export your pcap files as .XML files or .json files, which contain the packet information you have selected in the view dialog.";

    content += "<h2>Packet Filter</h2>";

    content += "In the text editor you can insert a string with the filtering expression for the sniffing thread. The GUI supports you for generating some simple filters on protocols, ports and addresses. ";
    content += "However you can insert an arbitrary filtering expression for winpcap in the text editor by hand. You might look in the online documentation of winpcap for the filter syntax. By clicking on \"Test\", ";
    content += "you can test the filter inserted in the text editor. By clicking on \"Ok\", the filtering expression you see in your text editor will be used for sniffing, until it will be changed by you.";

    content += "<h2>View Menu</h2>";

    content += "The Statistics dialog orders the selected information by its incidence. Moreover it visualizes the incidence of the selected information on the time line. The Packet and Byte View plot the numbers of sniffed packets and bytes respectively. ";
    content += "If you call an arbitrary View dialog directly after sniffing, it displays information about all recently sniffed packets. ";
    content += "During Sniffing the Packet and the Byte View show the current traffic. The Statistics Dialog must be closed during sniffing.";

    content += "<h2>Settings</h2>";

    content += "The duration of the time packet information is shown during sniffing can be selected in the settings under \"duration\". If you have selected \"save\", ";
    content += "information about all packets will be shown and can be stored in .pcap files after sniffing. You may also selected the names of these files and the number of packets stored in them.<br><br>";

    content += "The following information about a packet can be displayed:<dl>";
    content += "<dt>"+LABEL[TIME] + "</dt><dd>The timestamp of a packet.</dd>";
    content += "<dt>"+LABEL[VERSIONANDINTERNETHEADERLENGHT] + "</dt><dd>The Internet Version and the Lenght of the IP Header.</dd>";
    content += "<dt>"+LABEL[TYPEOFSERVICE] + "</dt><dd>Priorization of Packets.</dd>";
    content += "<dt>"+LABEL[TOTALLENGTH] + "</dt><dd>The length of the packet as registered in the IP Header.</dd>";
    content += "<dt>"+LABEL[IDENTIFICATION] + "</dt><dd>Identification of a packet.</dd>";
    content += "<dt>"+LABEL[FLAGSANDFRAGMENTOFFSET] + "</dt><dd>The position of packet as a fragment of a larger packet.</dd>";
    content += "<dt>"+LABEL[TIMETOLIFE] + "</dt><dd>The Time To Life is reduced by each passed station.</dd>";
    content += "<dt>" + LABEL[PROTOCOL] + "</dt><dd>The name of the protocol is displayed in the case of TCP, UDP, IGMP, ICMP. Otherwise you get the protocol number.</dd>";
    content += "<dt>" + LABEL[HOSTPORT] + "</dt><dd>The port of the computer you are connected with.</dd>";
    content += "<dt>" + LABEL[HOSTADDRESS] + "</dt><dd>The IP address of the computer you a connected with or the address of the destination, if your device is neither the source nor the destination of the packet.</dd>";
    content += "<dt>" + LABEL[HOSTNAME] + "</dt><dd>The Hostname of the computer you a connected with, when " + getProgramName() + " gets it. This may fail sometimes. In this case the ip number is displayed.</dd>";
    content += "<dt>" + LABEL[DIRECTION] + "</dt><dd>Whether the packet is incoming (\"in\") or outgoing (\"out\") or it is from source to host, if your device is neither the source nor the destination of the packet.</dd>";
    content += "<dt>" + onLocalOrSrc(LABEL[LOCALSRCPORT]) + "</dt><dd>The port of your selected device or the port of the source, if your device is neither the source nor the destination of the packet.</dd>";
    content += "<dt>" + onLocalOrSrc(LABEL[LOCALSRCADDRESS]) + "</dt><dd>The IP address of your selected device or the address of the source, if your device is neither the source nor the destination of the packet.</dd>";
    content += "<dt>"+LABEL[OPTIONANDPADDING] + "</dt><dd>Optional information in the IP Header.</dd>";
    content += "<dt>"+LABEL[UDPLENGTH] + "</dt><dd>The length of the packet as registered in the UDP Header.</dd>";
    content += "<dt>" + LABEL[ENTIRE_PACKET] + "</dt><dd>The bytes of the packet are represented by integers from 0 to 255.</dd>";
    content += "<dt>" + LABEL[PAYLOAD] + "</dt><dd>The bytes of the packet are interpreted as characters.</dd>";
    content += "<dt>"+LABEL[IPHEADERCHECKSUM] + "</dt><dd>The Checksum of the IP Header.</dd>";
    content += "<dt>"+LABEL[UDPHEADERCHECKSUM] + "</dt><dd>The Checksum of the UDP Header.</dd>";
    content += "</dl>";

    content += "<h2>Search</h2>";

    content += "Insert a text and select a column, in which this text should occur. If you like to have an invert match check \"invert match\". Then click on the\"Find\"-button. ";
    content += "Only Information about the packets satisfying your condition will remain. You can also select \"regular expressions\" and search for entries containing your ";
    content += "inserted regular expression. There are two Search Dialogs: One for searching on the current tab and one to search in files. With the second one you may search in (different) ";
    content += "files you have generated with netObservator. ";
    content += "By clicking at the arrow buttons on the Search on Tab Dialog, you can browse \"&lt;-\" backward and \"-&gt;\" forward through the history of your searches on the current tab.";

    content += "<h2>Libraries</h2>";
    content += getProgramName() + " uses WinPcap (includes WpdPack 4.1.2), which is a common program for network sniffing. For the GUI Qt 5.10.0 is used.";

    return content;
}

QString StringFactory::getLicense(){
    QString res = getProgramName();
    res += " is free software; you can redistribute it and/or modify ";
    res = res.append("it under the terms of the GNU General Public License as published by ");
    res = res.append(" the Free Software Foundation; either version 3 of the License, or ");
    res = res.append("(at your option) any later version. ");
    res = res.append("<br><br>");
    res += getProgramName();
    res += " is distributed in the hope that it will be useful, ";
    res = res.append("but WITHOUT ANY WARRANTY; without even the implied warranty of ");
    res = res.append("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ");
    res = res.append("GNU General Public License for more details. ");
    res = res.append("<br><br>");
    res = res.append("You should have received a copy of the GNU General Public License ");
    res = res.append("along with ");
    res += getProgramName();
    res += "; if not, see http://www.gnu.org/licenses.";

    return res;
}

QString StringFactory::getAbout() {
    QString content = getTitle();
    content = content.append("");
    content = content.append("<br>");
    content = content.append("&copy; 2015-2018 Jan Christian Rohde<br>jan-christian.rohde@gmx.de");
    content = content.append("<br><br>");
    content = content.append("License: GNU General Public License 3");
    return content;
}
