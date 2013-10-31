#include "FboInSGRenderer.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>

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

public slots:
    void renderFBO()
    {
        QSize size = rect().size().toSize();

        if (!m_fbo) {

            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            m_fbo = new QOpenGLFramebufferObject(size, format);
            m_texture = m_window->createTextureFromId(m_fbo->texture(), size);
            //m_logoRenderer = new LogoRenderer();
            //m_logoRenderer->initialize();
            setTexture(m_texture);
        }

        m_fbo->bind();

        glViewport(0, 0, size.width(), size.height());

 //       m_logoRenderer->render();

        m_fbo->bindDefault();

        m_window->update();
    }

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture *m_texture;
    QQuickWindow *m_window;

//    LogoRenderer *m_logoRenderer;
};

FboInSGRenderer::FboInSGRenderer()
{
    setFlag(ItemHasContents, true);
}

QSGNode *FboInSGRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    // Don't bother with resize and such, just recreate the node from scratch
    // when geometry changes.
    if (oldNode)
        delete oldNode;

    TextureNode *node = new TextureNode(window());
    node->setRect(boundingRect());

    return node;
}

#include "FboInSGRenderer.moc"
