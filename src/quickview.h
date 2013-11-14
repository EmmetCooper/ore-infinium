/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#ifndef QUICKVIEW_H
#define QUICKVIEW_H

#include "glew.h"

#include <QQuickView>

class Client;

class QuickView : public QQuickView
{
    Q_OBJECT

public:
    QuickView(Client* client, QWindow* parent = 0);

signals:
    void keyPressed(QKeyEvent* event);

private:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

    Client* m_client = nullptr;
};
Q_DECLARE_METATYPE(QKeyEvent*);

#endif
