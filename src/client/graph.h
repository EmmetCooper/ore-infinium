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

    struct Sample {
        Sample()
        {

        }

        Sample(double _x, double _scaledValue, double _originalValue) :
        x(_x),
        scaledValue(_scaledValue),
        originalValue(_originalValue)
        {

        }

        double x = 0.0;
        double scaledValue = 0.0;
        double originalValue = 0.0;
    };

private:
    double m_min = 0.0;
    double m_max = 0.0;

    // how much to move for left shift operation
    double m_leftMovement = 2.0;
    double m_scalar = 1.0;

    QVector<Sample> m_points;
};

#endif
