#ifndef FBOINSGRENDERER_H
#define FBOINSGRENDERER_H

#include <QQuickItem>
#include <QTimer>

class FboInSGRenderer : public QQuickItem
{
    Q_OBJECT

public:
    FboInSGRenderer();

signals:
    void renderCalled();

protected:
    QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
    QTimer m_timer;
};

#endif
