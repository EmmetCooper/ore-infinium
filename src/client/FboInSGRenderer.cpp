#include "FboInSGRenderer.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include <src/debug.h>
#include <src/game.h>

class TextureNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:

    TextureNode(QQuickWindow *window)
        : m_fbo(0)
        , m_texture(0)
        , m_window(window)
    {
        connect(m_window, SIGNAL(beforeRendering()), this, SLOT(renderFBO()));
    }

    ~TextureNode()
    {
        delete m_texture;
        delete m_fbo;
    }

signals:
    void renderCalled();

public slots:
    void renderFBO()
    {
        QSize size = rect().size().toSize();

        if (!m_fbo) {

            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            m_fbo = new QOpenGLFramebufferObject(size, format);

            m_fbo->bind();

            int res = -1;

            glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &res);
            Game::setGLFBO(res);

            m_texture = m_window->createTextureFromId(m_fbo->texture(), size);
            setTexture(m_texture);
        }

        m_fbo->bind();

//        glViewport(0, 0, size.width(), size.height());
        glViewport(0, 0, size.width(), size.height());

        emit renderCalled();

        m_fbo->bindDefault();

        m_window->update();
    }

private:
    QOpenGLFramebufferObject* m_fbo;
    QSGTexture* m_texture;
    QQuickWindow* m_window;
};

FboInSGRenderer::FboInSGRenderer()
{
    setProperty("id", QVariant("renderer"));
    setObjectName("renderer");

    setFlag(ItemHasContents, true);

    m_timer.setInterval(1.0/60.0);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

QSGNode *FboInSGRenderer::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    // Don't bother with resize and such, just recreate the node from scratch
    // when geometry changes.
    if (oldNode) {
        delete oldNode;
    }

    TextureNode *node = new TextureNode(window());
    node->setRect(boundingRect());

    connect(node, SIGNAL(renderCalled()), this, SIGNAL(renderCalled()), Qt::ConnectionType::DirectConnection);

    return node;
}

#include "FboInSGRenderer.moc"
