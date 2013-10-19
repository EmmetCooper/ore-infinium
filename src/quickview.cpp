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

#include "quickview.h"

#include "src/client/client.h"
#include "debug.h"

#include <QtCore/QCoreApplication>

QuickView::QuickView(Client* client, QWindow* parent)
: QQuickView(parent),
  m_client(client)
{
//    setSurfaceType(QSurface::OpenGLSurface);
//
//    QSurfaceFormat format;
//    format.setVersion(3, 1);
//    format.setOption(QSurfaceFormat::DeprecatedFunctions);
//
//    //TODO: qquick2 supports compatibility only..
////    format.setProfile(QSurfaceFormat::CompatibilityProfile);
//
//    format.setDepthBufferSize(24);
//    format.setStencilBufferSize(8);
//
//    format.setSamples(0);
//    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
//
//    format.setAlphaBufferSize(8);
//    format.setRedBufferSize(8);
//    format.setGreenBufferSize(8);
//    format.setBlueBufferSize(8);
//
//    setFormat(format);
    qRegisterMetaType<QKeyEvent*>("QKeyEvent");
    create();
}

void QuickView::keyPressEvent(QKeyEvent* event)
{
    m_client->viewKeyPressed(event);

    QQuickView::keyPressEvent(event);
}
