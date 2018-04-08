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

#ifndef HOSTCHART_H
#define HOSTCHART_H

#include <QtCharts/QChartView>
#include "util.h"

QT_CHARTS_USE_NAMESPACE

class HostChart : public QChart
{
public:
    HostChart();

    void set(QPixmap &map, const QHash<QString, Protocol> &hostProtocols);
};

#endif // HOSTCHART_H
