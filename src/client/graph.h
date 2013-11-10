#ifndef GRAPH_H
#define GRAPH_H

#include <QQuickPaintedItem>
#include <QTimer>

class Graph : public QQuickPaintedItem
{
    Q_OBJECT

public:
    Graph();

public slots:
    void addSample(double value);

protected:
    void paint(QPainter* painter);

private slots:
    void timeout();

private:
    void shiftLeft();

private:
    QTimer m_timer;

    double m_leftMovement = 2.0;

    QVector<QPointF> m_points;
};

#endif
