#include "graph.h"
#include <qt/QtCore/QEvent>
#include <qt/QtCore/qcoreevent.h>
#include <qt/QtGui/qevent.h>

#include <QPainter>
#include <QEvent>

#include <QDebug>

Graph::Graph(): QQuickPaintedItem()
{
    setAntialiasing(true);
    setRenderTarget(FramebufferObject);
    setOpaquePainting(true);

    m_points.reserve(1000);
    setAcceptHoverEvents(true);
}

void Graph::setMax(double max)
{
    m_max = max;
    updateScalar();
}

void Graph::setMin(double min)
{
    m_min = min;
    updateScalar();
}

void Graph::updateScalar()
{
    m_scalar = boundingRect().height() / m_max;
}

void Graph::paint(QPainter* painter)
{
    QRectF rect = boundingRect();
    painter->fillRect(0, 0, rect.width(), rect.height(), Qt::black);

    if (m_points.size() == 0) {
        return;
    }

    ///// draw grid lines
    painter->setPen(Qt::white);

    QVector<QLine> gridLines;
    gridLines << QLine(0, 0, rect.width(), 0);

    painter->drawLines(gridLines);
    /////////

    painter->setPen(Qt::blue);

    QBrush b;
    b.setStyle(Qt::SolidPattern);
    b.setColor(Qt::blue);

    painter->setBrush(b);

    // draw filled graph
    QPolygonF poly;
    for (int i = 0; i < m_points.size(); ++i) {
        Sample sample = m_points.at(i);

        QPointF point(sample.x, rect.height() - sample.scaledValue);
        poly << point;
    }

    //finalize the shape so we can fill it
    Sample lastSample = m_points.at(m_points.size() - 1);
    QPointF lastPoint(lastSample.x, lastSample.scaledValue);

    lastPoint.setY(rect.height());
    poly << lastPoint;


    Sample firstSample = m_points.at(0);
    QPointF firstPoint(firstSample.x, firstSample.scaledValue);

    firstPoint.setY(rect.height());
    poly << firstPoint;

    painter->drawPolygon(poly, Qt::FillRule::WindingFill);

    painter->setPen(Qt::red);

    QString text = QString::number((rect.height() - m_tooltipPoint.y()) / m_scalar);
    painter->drawText(m_tooltipPoint, text);
}

void Graph::hoverMoveEvent(QHoverEvent * event)
{
    m_tooltipPoint = event->oldPosF();
}

void Graph::addSample(double value)
{
    Sample sample(boundingRect().width(), value * m_scalar, value);
    m_points.append(sample);

    shiftLeft();
    this->update();
}

void Graph::shiftLeft()
{
    for (int i = 0; i < m_points.size(); ++i) {
        m_points[i].x -= m_leftMovement;

        if (m_points[i].x < 0.0) {
            m_points.remove(i);
        }
    }
    qDebug() << "COUNT:" << m_points.size();
}
