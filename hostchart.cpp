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

#include "hostchart.h"

#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>

HostChart::HostChart() //: QChart(Q_NULLPTR, Qt::)
{
}

void HostChart::set(QPixmap &map, const QHash<QString, Protocol> &hostProtocols) {
    QStringList protocols;
    protocols << "TCP" << "UDP" << "ICMP" << "IGMP" << "others";

    struct HostProtocol {
        HostProtocol(const QString &n, const Protocol &p) : name(n), protocol(p) {}

        QString name;
        Protocol protocol;

        bool operator<(const HostProtocol & rhs) const {
            int s = protocol.sum() - rhs.protocol.sum();
            if (s > 0) return true;
            else if (s == 0) {
                return name > rhs.name;
            }
            else return false;
        }
    };

    QList<HostProtocol> proto;

    int maxNameSize = 0;
    for (const QString &host : hostProtocols.keys()) {
        proto.append(HostProtocol(host, hostProtocols[host]));
        if (host.size() > maxNameSize) maxNameSize = host.size();
    }

    qSort(proto.begin(), proto.end());

    QList<int> set[5];

    int max = 0;
    for (const HostProtocol & protocol : proto) {
        set[0] << protocol.protocol.TCP;
        if (protocol.protocol.TCP > max) max = protocol.protocol.TCP;
    }
    for (const HostProtocol & protocol : proto) {
        set[1] << protocol.protocol.UDP;
        if (protocol.protocol.UDP > max) max = protocol.protocol.UDP;
    }
    for (const HostProtocol & protocol : proto) {
        set[2] << protocol.protocol.ICMP;
        if (protocol.protocol.ICMP > max) max = protocol.protocol.ICMP;
    }
    for (const HostProtocol & protocol : proto) {
        set[3] << protocol.protocol.IGMP;
        if (protocol.protocol.IGMP > max) max = protocol.protocol.IGMP;
    }
    for (const HostProtocol & protocol : proto) {
        set[4] << protocol.protocol.other;
        if (protocol.protocol.other > max) max = protocol.protocol.other;
    }

    double d = log10((double)max);
    int pow10 = (int) d;

    int tens = 1;
    for (int i = 0; i < pow10; i++) {
        tens *= 10;
    }
    int quot = max / tens;
    int maxR = (quot + 1) * tens;

    int dist = 10, legendH = 100, textw = 8 * maxNameSize, plotw = 300, w = 2*dist + textw + plotw, h = 2*dist + 45*hostProtocols.size() + legendH;
    map = QPixmap(w, h);

    QPainter paint;
    paint.begin(&map);
    paint.fillRect(map.rect(),Qt::white);

    QList<QColor> color;
    color.append(Qt::green);
    color.append(Qt::blue);
    color.append(Qt::red);
    color.append(Qt::magenta);
    color.append(Qt::darkGray);

    paint.setPen(Qt::lightGray);
    paint.drawLine(dist, dist, dist, dist + 45 * hostProtocols.size());
    paint.drawLine(dist + textw, dist, dist + textw, dist + 45 * hostProtocols.size());
    int colCount = maxR / tens;
    for (int i = 1; i < colCount ; i++) {
        paint.drawLine(dist + textw + plotw * i / colCount, dist, dist + textw + plotw * i / colCount, dist + 45 * hostProtocols.size());
    }
    paint.drawLine(dist + textw + plotw, dist, dist + textw + plotw, dist + 45 * hostProtocols.size());

    paint.drawLine(dist, dist, dist + textw + plotw, dist);
    int lineH = 10;
    for (int host = 0; host < hostProtocols.size(); host++) {
        lineH += 45;
        paint.drawLine(dist, lineH, dist + textw + plotw, lineH);
    }

    paint.setPen(Qt::black);
    paint.setBrush(Qt::black);

    QString resStr = "maximal number of sniffed packets of a host and protocols: ";
    resStr += QString::number(max);
    int legendLineH = 2 * dist + 45 * hostProtocols.size() + 10;
    paint.drawText(dist, legendLineH, resStr);

    for (int i = 0; i < 5; i++) {
        legendLineH += 15;
        paint.setBrush(color[i]);
        paint.drawRect(dist, legendLineH, 4, 4);
        paint.drawText(dist+9, legendLineH + 6, protocols[i]);
    }

    int hostH = dist + 17;
    for (int host = 0; host < hostProtocols.size(); host++) {
        paint.drawText(dist+5, hostH, 7 * maxNameSize, 13, Qt::AlignLeft | Qt::AlignVCenter, proto[host].name);
        hostH += 45;
    }

    for (int i = 0; i < 5; i++) {
        paint.setBrush(color[i]);
        int begw = dist + textw;
        int h = dist+ (i+1)*8-3;
        for (int host = 0; host < hostProtocols.size(); host++) {
            int endw = plotw * set[i][host] / maxR;
            paint.drawRect(begw, h, endw, 4);
            h += 45;
        }
    }
}
