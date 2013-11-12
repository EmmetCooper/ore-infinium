#ifndef GRAPH_H
#define GRAPH_H

#include <QQuickPaintedItem>
#include <QTimer>

class Graph : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(double min READ min WRITE setMin NOTIFY minChanged);

public:
    Graph();

    void setMin(double min);
    void setMax(double max);

    double min() {
        return m_min;
    }

    double max() {
        return m_max;
    }

public slots:
    void addSample(double value);

protected:
    void paint(QPainter* painter);

signals:
    void minChanged(int min);

private:
    void shiftLeft();

private:
    double m_min = 0.0;
    double m_max = 0.0;

    double m_leftMovement = 2.0;

    QVector<QPointF> m_points;
};

#endif
