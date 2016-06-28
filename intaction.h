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

#ifndef INTACTION_H
#define INTACTION_H

#include <QAction>

enum class stringKey;

class intAction : public QAction
{
    Q_OBJECT

private:
    stringKey code;

public:
    explicit intAction(stringKey i, QString str, QObject *base);
    ~intAction();

private slots:
    void codeSignal();

signals:
    void sigCode(stringKey);
};

#endif // INTACTION_H
