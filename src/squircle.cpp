
#include "squircle.h"
#include "debug.h"
#include "debug.h"

#include <QtQuick/QQuickWindow>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

#include "assert.h"

Squircle::Squircle()
    : m_program(0)
    , m_t(0)
    , m_thread_t(0)
{
    connect(this, &Squircle::windowChanged, this, &Squircle::handleWindowChanged);


//    m_client = new Client();
}

Squircle::~Squircle()
{
    m_game->shutdown();
    delete m_game;
}

void Squircle::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void Squircle::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
        connect(win, SIGNAL(beforeRendering()), this, SLOT(paintUnder()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}

void Squircle::paintUnder()
{
    assert(window());
    assert(window()->openglContext());

    glViewport(0, 0, window()->width(), window()->height());
//
 //   glDisable(GL_DEPTH_TEST);
//
    glClearColor(0, 0, 0, 1);
//    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_game) {
        Debug::log(Debug::StartupArea) << "m_game not yet created, creating new!";

        m_game =  new Game();
        m_game->init();
    } else {
        m_game->tick();
    }

//    if (!m_program) {
//        m_program = new QOpenGLShaderProgram();
//        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
//                                           "attribute highp vec4 vertices;"
//                                           "varying highp vec2 coords;"
//                                           "void main() {"
//                                           "    gl_Position = vertices;"
//                                           "    coords = vertices.xy;"
//                                           "}");
//        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
//                                           "uniform lowp float t;"
//                                           "varying highp vec2 coords;"
//                                           "void main() {"
//                                           "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
//                                           "    i = smoothstep(t - 0.8, t + 0.8, i);"
//                                           "    i = floor(i * 20.) / 20.;"
//                                           "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
//                                           "}");
//
//        m_program->bindAttributeLocation("vertices", 0);
//        m_program->link();
//
 //       connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
  //              this, SLOT(cleanup()), Qt::DirectConnection);
   // }
    assert(window()->openglContext());


    if (m_frameCount == 0) {
        m_time.start();
    } else {
        printf("FPS is %f ms\n", m_time.elapsed() / float(m_frameCount));
    }
    m_frameCount++;



//    m_program->bind();
//
//    m_program->enableAttributeArray(0);
//
//    float values[] = {
//        -1, -1,
//        1, -1,
//        -1, 1,
//        1, 1
//    };
//    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
//    m_program->setUniformValue("t", (float) m_thread_t);
//

//
//    glEnable(GL_BLEND);
 //   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//    m_program->disableAttributeArray(0);
//    m_program->release();
}

void Squircle::cleanup()
{
//    if (m_program) {
//        delete m_program;
//        m_program = 0;
//    }
}

void Squircle::sync()
{
    m_thread_t = m_t;
}
