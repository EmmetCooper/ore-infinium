#ifndef SQUIRCLE_H
#define SQUIRCLE_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QTime>

//#include "client/client.h"

#include "src/game.h"

class Squircle : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Squircle();
    ~Squircle();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void paintUnder();
    void cleanup();
    void sync();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    QOpenGLShaderProgram *m_program;

//    Client* m_client = nullptr;

    QTime m_time;
    int m_frameCount = 0;

    Game *m_game = nullptr;

    qreal m_t;
    qreal m_thread_t;
};

#endif // SQUIRCLE_H
