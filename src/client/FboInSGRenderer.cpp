#include "FboInSGRenderer.h"

#include <QtGui/QOpenGLFramebufferObject>

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include <src/debug.h>

class TextureNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT

public:
    TextureNode(QQuickWindow *window)
        : m_fbo(0)
        , m_texture(0)
        , m_window(window)
    {
        Debug::log(Debug::ImportantArea) << "TextureNode ctor..";
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
        Debug::log(Debug::ImportantArea) << "renderFBO called..";

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
    Debug::log(Debug::ImportantArea) << "FboInSGRenderer ctor..";

    setProperty("id", QVariant("renderer"));
    setObjectName("renderer");

    setFlag(ItemHasContents, true);
}

QSGNode *FboInSGRenderer::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Debug::log(Debug::ImportantArea) << "updatePaintNode called..";

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
