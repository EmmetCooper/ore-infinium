#ifndef FBOINSGRENDERER_H
#define FBOINSGRENDERER_H

#include <QQuickItem>

class FboInSGRenderer : public QQuickItem
{
    Q_OBJECT

public:
    FboInSGRenderer();

signals:
    void renderCalled();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
};

#endif
