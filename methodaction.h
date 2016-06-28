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

#ifndef METHODACTION_H
#define METHODACTION_H

#include <QAction>
#include "util.h"

// get problems, when i emit std::function by a Qt signal directly
struct methodWrapper {
    std::function<bool()> method;
};

class MethodAction : public QAction
{
    Q_OBJECT
public:
    explicit MethodAction(QString str, QObject *base);
    ~MethodAction();

    void setMethod(std::function<bool()> func) {method.method = func;}

private:
    methodWrapper method;

private slots:
    void codeSignal();

signals:
    void sigMethod(methodWrapper);
};

#endif // METHODACTION_H
