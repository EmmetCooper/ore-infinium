#include "graph.h"

#include <QPainter>

#include <QDebug>

Graph::Graph(): QQuickPaintedItem()
{
    setAntialiasing(true);
    setRenderTarget(FramebufferObject);
    setOpaquePainting(true);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.setInterval(16);
    m_timer.start();

    m_points.reserve(1000);
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
        poly << m_points.at(i);
    }

    //finalize the shape so we can fill it
    QPointF lastPoint = m_points.at(m_points.size() - 1);
    lastPoint.setY(rect.height());
    poly << lastPoint;


    QPointF firstPoint = m_points.at(0);
    firstPoint.setY(rect.height());
    poly << firstPoint;

    painter->drawPolygon(poly, Qt::FillRule::WindingFill);
}

void Graph::timeout()
{
    update();

    int rand = qrand() % 100;
    qDebug() << "RAND: " << rand;
    addSample(rand);

    shiftLeft();
}

void Graph::addSample(double value)
{
    m_points.append(QPointF(boundingRect().width(), value));
}

void Graph::shiftLeft()
{
    for (int i = 0; i < m_points.size(); ++i) {
        m_points[i].rx() -= m_leftMovement;

        if (m_points[i].x() < 0.0) {
            m_points.remove(i);
        }
    }
}
