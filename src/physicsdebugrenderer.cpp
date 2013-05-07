/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#include "physicsdebugrenderer.h"

#include "debug.h"
#include "world.h"
#include "settings/settings.h"

static PhysicsDebugRenderer *s_instance = nullptr;

const cpFloat PI = 3.14159;

PhysicsDebugRenderer::PhysicsDebugRenderer(Camera* camera)
{
    s_instance = this;
    m_shader = new Shader("physicsdebugrenderer.vert", "physicsdebugrenderer.frag");
    m_shader->bindProgram();
    setCamera(camera);

    Debug::checkGLError();
    initGL();
    Debug::checkGLError();
}

PhysicsDebugRenderer::~PhysicsDebugRenderer()
{
    glDeleteBuffers(1, &m_vboSolidPolygons);
    glDeleteBuffers(1, &m_iboSolidPolygons);
    glDeleteVertexArrays(1, &m_vaoSolidPolygons);

    glDeleteBuffers(1, &m_vboSolidCircles);
    glDeleteBuffers(1, &m_iboSolidCircles);
    glDeleteVertexArrays(1, &m_vaoSolidCircles);


    glDeleteBuffers(1, &m_vboPolygons);
    glDeleteBuffers(1, &m_iboPolygons);
    glDeleteVertexArrays(1, &m_vaoPolygons);

    glDeleteBuffers(1, &m_vboSegments);
    glDeleteBuffers(1, &m_iboSegments);
    glDeleteVertexArrays(1, &m_vaoSegments);

    // NOTE: cam is not ours to delete.
    delete m_shader;
}

void PhysicsDebugRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->addShader(m_shader);
}

void PhysicsDebugRenderer::initGL()
{
    initGLSolidPolygons();
    initGLSolidCircles();
    initGLPolygons();
    initGLSegments();
}

void PhysicsDebugRenderer::initGLPolygons()
{
    glGenVertexArrays(1, &m_vaoPolygons);
    glBindVertexArray(m_vaoPolygons);

    glGenBuffers(1, &m_vboPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPolygons);
    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboPolygons);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::initGLSolidPolygons()
{
    glGenVertexArrays(1, &m_vaoSolidPolygons);
    glBindVertexArray(m_vaoSolidPolygons);

    glGenBuffers(1, &m_vboSolidPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSolidPolygons);
    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboSolidPolygons);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::initGLSolidCircles()
{
    glGenVertexArrays(1, &m_vaoSolidCircles);
    glBindVertexArray(m_vaoSolidCircles);

    glGenBuffers(1, &m_vboSolidCircles);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSolidCircles);
    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboSolidCircles);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::initGLSegments()
{
    glGenVertexArrays(1, &m_vaoSegments);
    glBindVertexArray(m_vaoSegments);

    glGenBuffers(1, &m_vboSegments);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSegments);
    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)0
    );
    buffer_offset += sizeof(float) * 2;
    Debug::checkGLError();

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    glGenBuffers(1, &m_iboSegments);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();
}

void PhysicsDebugRenderer::DrawPolygon(const cpVect* vertices, int32_t vertexCount, PhysicsDebugRenderer::Color color)
    //const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
//    const size_t iboOffset = m_verticesPolygons.size();
//
//    for (int32_t i = 0; i < vertexCount; ++i) {
//        Vertex vertex;
//        vertex.x = vertices[i].x;
//        vertex.y = vertices[i].y;
//
//        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
//        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
//        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));
//        uint8_t alpha = 80;
//        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
//        vertex.color = colorPacked;
//
//        m_verticesPolygons.push_back(vertex);
//    }
//
//    for (int i = 0; i < vertexCount; i++) {
//        m_indicesPolygons.push_back(iboOffset + (i % vertexCount));
//        m_indicesPolygons.push_back(iboOffset + ((i + 1) % vertexCount));
//    }
}

void PhysicsDebugRenderer::DrawSolidPolygon(const cpVect* vertices, int32_t vertexCount, PhysicsDebugRenderer::Color color)
{
    const size_t iboOffset = m_verticesSolidPolygons.size();

    for (int32_t i = 0; i < vertexCount; ++i) {
        Vertex vertex;
        vertex.x = vertices[i].x;
        vertex.y = vertices[i].y;

        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));

        uint8_t alpha = 80;
        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertex.color = colorPacked;

        m_verticesSolidPolygons.push_back(vertex);
    }

    for (int i = 1; i < vertexCount - 1; i++) {
        m_indicesSolidPolygons.push_back(iboOffset);
        m_indicesSolidPolygons.push_back(iboOffset + i);
        m_indicesSolidPolygons.push_back(iboOffset + i + 1);
    }
}

//FIXME: the circle calls need to be fixed to not run a draw call each polygon count
void PhysicsDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    assert(0);
    /*
    const float32 k_segments = 16.0f;
    int vertexCount=16;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;

    GLdouble                         glVertices[vertexCount*2];
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertices[i*2]=v.x;
        glVertices[i*2+1]=v.y;
        theta += k_increment;
    }

    glColor4f(color.r, color.g, color.b,1);
    glVertexPointer(2, GL_DOUBLE, 0, glVertices);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
    */
}

void PhysicsDebugRenderer::DrawSolidCircle(cpVect center, cpFloat radius, cpFloat rotation, PhysicsDebugRenderer::Color color)
{
    const float32 k_segments = 16.0f;
    int vertexCount = 16;
    const float32 k_increment = 2.0f * PI / k_segments;
    cpFloat theta = 0.0f;

    std::vector<Vertex> vertices;
    for (int32 i = 0; i < k_segments; ++i)
    {
        cpVect v = cpv(radius * cosf(theta) + center.x, radius * sinf(theta) + center.y);
        Vertex vert;
        vert.x = v.x;
        vert.y = v.y;
        vertices.push_back(vert);
        theta += k_increment;
    }

    const size_t iboOffset = m_verticesSolidCircles.size();

    for (size_t i = 0; i < vertices.size(); ++i) {
        Vertex vertex;
        vertex.x = vertices[i].x;
        vertex.y = vertices[i].y;

        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));
        uint8_t alpha = 80;
        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertex.color = colorPacked;

        m_verticesSolidCircles.push_back(vertex);
    }

    for (size_t i = 1; i < vertices.size() - 1; i++) {
        m_indicesSolidCircles.push_back(iboOffset);
        m_indicesSolidCircles.push_back(iboOffset + i);
        m_indicesSolidCircles.push_back(iboOffset + i + 1);
    }

    // Draw the axis line
    //FIXME: i don't think this works? but i don't see why it wouldn't..
//    DrawSegment(center, center + radius * axis, color);
}

void PhysicsDebugRenderer::DrawSegment(const cpVect& p1, const cpVect& p2, PhysicsDebugRenderer::Color color)
{
//
//    std::vector<b2Vec2> vertices;
//    vertices.push_back(p1);
//    vertices.push_back(p2);
//
//    size_t vertexCount = vertices.size();
//
//    for (size_t i = 0; i < vertexCount; ++i) {
//        Vertex vertex;
//        vertex.x = vertices[i].x;
//        vertex.y = vertices[i].y;
//
//        uint8_t red = static_cast<uint8_t>(ceil(color.r * 255));
//        uint8_t green = static_cast<uint8_t>(ceil(color.g * 255));
//        uint8_t blue = static_cast<uint8_t>(ceil(color.b * 255));
//        uint8_t alpha = 255;
//        int32_t colorPacked = red | (green << 8) | (blue << 16) | (alpha << 24);
//        vertex.color = colorPacked;
//
//        m_verticesSegments.push_back(vertex);
//    }
//
//    const size_t iboOffset = m_verticesSegments.size();
//
//    m_indicesSegments.push_back(iboOffset);
//    m_indicesSegments.push_back(iboOffset + 1);
}

void PhysicsDebugRenderer::DrawTransform(const b2Transform& xf)
{
//    b2Vec2 p1 = xf.p, p2;
//    const float32 k_axisScale = 0.4f;
//    p2 = p1 + k_axisScale * xf.q.GetXAxis();
//    DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));
//
//    p2 = p1 + k_axisScale * xf.q.GetYAxis();
//    DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
}

void PhysicsDebugRenderer::render()
{
    m_mutex.lock();

    renderSolidPolygons();
    renderSolidCircles();
 //   renderPolygons();
  //  renderSegments();


    m_mutex.unlock();
}

void PhysicsDebugRenderer::renderPolygons()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboPolygons);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboPolygons);

    if (m_verticesPolygons.size() > m_maxVBOSizePolygons) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesPolygons.size(), m_verticesPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesPolygons.size(), m_verticesPolygons.data());
    }

    if (m_indicesPolygons.size() > m_highestIBOSizePolygons) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indicesPolygons.size(), m_indicesPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * m_indicesPolygons.size(), m_indicesPolygons.data());
    }

    glDrawElements(GL_LINES, m_indicesPolygons.size(), GL_UNSIGNED_INT, (GLvoid*)0);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_maxVBOSizePolygons = m_verticesPolygons.size();
    m_highestIBOSizePolygons = m_indicesPolygons.size();
}

void PhysicsDebugRenderer::renderSolidPolygons()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoSolidPolygons);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSolidPolygons);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSolidPolygons);

    if (m_verticesSolidPolygons.size() > m_maxVBOSizeSolidPolygons) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesSolidPolygons.size(), m_verticesSolidPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesSolidPolygons.size(), m_verticesSolidPolygons.data());
    }

    if (m_indicesSolidPolygons.size() > m_highestIBOSizeSolidPolygons) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indicesSolidPolygons.size(), m_indicesSolidPolygons.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * m_indicesSolidPolygons.size(), m_indicesSolidPolygons.data());
    }

    glDrawElements(GL_TRIANGLES, m_indicesSolidPolygons.size(), GL_UNSIGNED_INT, (GLvoid*)0);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_maxVBOSizeSolidPolygons = m_verticesSolidPolygons.size();
    m_highestIBOSizeSolidPolygons = m_indicesSolidPolygons.size();
}

void PhysicsDebugRenderer::renderSolidCircles()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoSolidCircles);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSolidCircles);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSolidCircles);

    if (m_verticesSolidCircles.size() > m_maxVBOSizeSolidCircles) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesSolidCircles.size(), m_verticesSolidCircles.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesSolidCircles.size(), m_verticesSolidCircles.data());
    }

    if (m_indicesSolidCircles.size() > m_highestIBOSizeSolidCircles) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indicesSolidCircles.size(), m_indicesSolidCircles.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * m_indicesSolidCircles.size(), m_indicesSolidCircles.data());
    }

    glDrawElements(GL_TRIANGLES, m_indicesSolidCircles.size(), GL_UNSIGNED_INT, (GLvoid*)0);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_maxVBOSizeSolidCircles = m_verticesSolidCircles.size();
    m_highestIBOSizeSolidCircles = m_indicesSolidCircles.size();
}

void PhysicsDebugRenderer::renderSegments()
{
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bindProgram();
    Debug::checkGLError();

    glBindVertexArray(m_vaoSegments);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSegments);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSegments);

    if (m_verticesSegments.size() > m_maxVBOSizeSegments) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verticesSegments.size(), m_verticesSegments.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verticesSegments.size(), m_verticesSegments.data());
    }

    if (m_indicesSegments.size() > m_highestIBOSizeSegments) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indicesSegments.size(), m_indicesSegments.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * m_indicesSegments.size(), m_indicesSegments.data());
    }

    glDrawElements(GL_LINES, m_indicesSegments.size(), GL_UNSIGNED_INT, (GLvoid*)0);

    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_maxVBOSizeSegments = m_verticesSegments.size();
    m_highestIBOSizeSegments = m_indicesSegments.size();
}


PhysicsDebugRenderer::Color PhysicsDebugRenderer::ColorFromHash(cpHashValue hash, float alpha)
{
    unsigned long val = (unsigned long)hash;

    // scramble the bits up using Robert Jenkins' 32 bit integer hash function
    val = (val+0x7ed55d16) + (val<<12);
    val = (val^0xc761c23c) ^ (val>>19);
    val = (val+0x165667b1) + (val<<5);
    val = (val+0xd3a2646c) ^ (val<<9);
    val = (val+0xfd7046c5) + (val<<3);
    val = (val^0xb55a4f09) ^ (val>>16);

    GLdouble r = (val>>0) & 0xFF;
    GLdouble g = (val>>8) & 0xFF;
    GLdouble b = (val>>16) & 0xFF;

    GLdouble max = cpfmax(cpfmax(r, g), b);
    GLdouble min = cpfmin(cpfmin(r, g), b);
    GLdouble intensity = 0.75;

    // Saturate and scale the color
    if(min == max){
        return RGBAColor(intensity, 0.0, 0.0, alpha);
    } else {
        GLdouble coef = alpha*intensity/(max - min);
        return RGBAColor(
            (r - min)*coef,
                         (g - min)*coef,
                         (b - min)*coef,
                         alpha
        );
    }
}

inline void
PhysicsDebugRenderer::glColor_from_color(Color color){
  //FIXME: HACK:  glColor4fv((GLdouble *)&color);
}

PhysicsDebugRenderer::Color PhysicsDebugRenderer::ColorForShape(cpShape *shape)
{
    if(cpShapeGetSensor(shape)){
        return LAColor(1, 0);
    } else {
        cpBody *body = shape->body;

        if(cpBodyIsSleeping(body)){
            return LAColor(0.2, 1);
        } else if(body->node.idleTime > shape->space->sleepTimeThreshold) {
            return LAColor(0.66, 1);
        } else {
            return ColorFromHash(shape->hashid, SHAPE_ALPHA);
        }
    }
}

static const GLdouble circleVAR[] = {
    0.0000f,  1.0000f,
    0.2588f,  0.9659f,
    0.5000f,  0.8660f,
    0.7071f,  0.7071f,
    0.8660f,  0.5000f,
    0.9659f,  0.2588f,
    1.0000f,  0.0000f,
    0.9659f, -0.2588f,
    0.8660f, -0.5000f,
    0.7071f, -0.7071f,
    0.5000f, -0.8660f,
    0.2588f, -0.9659f,
    0.0000f, -1.0000f,
    -0.2588f, -0.9659f,
    -0.5000f, -0.8660f,
    -0.7071f, -0.7071f,
    -0.8660f, -0.5000f,
    -0.9659f, -0.2588f,
    -1.0000f, -0.0000f,
    -0.9659f,  0.2588f,
    -0.8660f,  0.5000f,
    -0.7071f,  0.7071f,
    -0.5000f,  0.8660f,
    -0.2588f,  0.9659f,
    0.0000f,  1.0000f,
    0.0f, 0.0f, // For an extra line to see the rotation.
};
static const int circleVAR_count = sizeof(circleVAR)/sizeof(GLdouble)/2;

void PhysicsDebugRenderer::ChipmunkDebugDrawCircle(cpVect center, cpFloat angle, cpFloat radius, Color lineColor, Color fillColor)
{
    glVertexPointer(2, GL_DOUBLE, 0, circleVAR);

    glPushMatrix(); {
        glTranslatef(center.x, center.y, 0.0f);
        glRotatef(angle*180.0f/M_PI, 0.0f, 0.0f, 1.0f);
        glScalef(radius, radius, 1.0f);

        if(fillColor.a > 0){
            glColor_from_color(fillColor);
            glDrawArrays(GL_TRIANGLE_FAN, 0, circleVAR_count - 1);
        }

        if(lineColor.a > 0){
            glColor_from_color(lineColor);
            glDrawArrays(GL_LINE_STRIP, 0, circleVAR_count);
        }
    } glPopMatrix();
}

static const GLdouble pillVAR[] = {
    0.0000f,  1.0000f, 1.0f,
    0.2588f,  0.9659f, 1.0f,
    0.5000f,  0.8660f, 1.0f,
    0.7071f,  0.7071f, 1.0f,
    0.8660f,  0.5000f, 1.0f,
    0.9659f,  0.2588f, 1.0f,
    1.0000f,  0.0000f, 1.0f,
    0.9659f, -0.2588f, 1.0f,
    0.8660f, -0.5000f, 1.0f,
    0.7071f, -0.7071f, 1.0f,
    0.5000f, -0.8660f, 1.0f,
    0.2588f, -0.9659f, 1.0f,
    0.0000f, -1.0000f, 1.0f,

    0.0000f, -1.0000f, 0.0f,
    -0.2588f, -0.9659f, 0.0f,
    -0.5000f, -0.8660f, 0.0f,
    -0.7071f, -0.7071f, 0.0f,
    -0.8660f, -0.5000f, 0.0f,
    -0.9659f, -0.2588f, 0.0f,
    -1.0000f, -0.0000f, 0.0f,
    -0.9659f,  0.2588f, 0.0f,
    -0.8660f,  0.5000f, 0.0f,
    -0.7071f,  0.7071f, 0.0f,
    -0.5000f,  0.8660f, 0.0f,
    -0.2588f,  0.9659f, 0.0f,
    0.0000f,  1.0000f, 0.0f,
};
static const int pillVAR_count = sizeof(pillVAR)/sizeof(GLdouble)/3;

void PhysicsDebugRenderer::ChipmunkDebugDrawSegment(cpVect a, cpVect b, Color color)
{
    GLdouble verts[] = {
        a.x, a.y,
        b.x, b.y,
    };

    glVertexPointer(2, GL_DOUBLE, 0, verts);
    glColor_from_color(color);
    glDrawArrays(GL_LINES, 0, 2);
}

void PhysicsDebugRenderer::ChipmunkDebugDrawFatSegment(cpVect a, cpVect b, cpFloat radius, Color lineColor, Color fillColor)
{
    if(radius){
        glVertexPointer(3, GL_DOUBLE, 0, pillVAR);
        glPushMatrix(); {
            cpVect d = cpvsub(b, a);
            cpVect r = cpvmult(d, radius/cpvlength(d));

            const GLfloat matrix[] = {
                float(r.x), float(r.y), 0.0f, 0.0f,
                float(-r.y), float(r.x), 0.0f, 0.0f,
                float(d.x), float(d.y), 0.0f, 0.0f,
                float(a.x), float(a.y), 0.0f, 1.0f,
            };
            glMultMatrixf(matrix);

            if(fillColor.a > 0){
                glColor_from_color(fillColor);
                glDrawArrays(GL_TRIANGLE_FAN, 0, pillVAR_count);
            }

            if(lineColor.a > 0){
                glColor_from_color(lineColor);
                glDrawArrays(GL_LINE_LOOP, 0, pillVAR_count);
            }
        } glPopMatrix();
    } else {
        ChipmunkDebugDrawSegment(a, b, lineColor);
    }
}

void PhysicsDebugRenderer::ChipmunkDebugDrawPolygon(int count, cpVect *verts, Color lineColor, Color fillColor)
{
    #if CP_USE_DOUBLES
    glVertexPointer(2, GL_DOUBLE, 0, verts);
    #else
    glVertexPointer(2, GL_DOUBLE, 0, verts);
    #endif

    if(fillColor.a > 0){
        glColor_from_color(fillColor);
        glDrawArrays(GL_TRIANGLE_FAN, 0, count);
    }

    if(lineColor.a > 0){
        glColor_from_color(lineColor);
        glDrawArrays(GL_LINE_LOOP, 0, count);
    }
}

void PhysicsDebugRenderer::ChipmunkDebugDrawPoints(cpFloat size, int count, cpVect *verts, Color color)
{
    #if CP_USE_DOUBLES
    glVertexPointer(2, GL_DOUBLE, 0, verts);
    #else
    glVertexPointer(2, GL_DOUBLE, 0, verts);
    #endif

    glPointSize(size*ChipmunkDebugDrawPointLineScale);
    glColor_from_color(color);
    glDrawArrays(GL_POINTS, 0, count);
}

void PhysicsDebugRenderer::ChipmunkDebugDrawBB(cpBB bb, Color color)
{
    cpVect verts[] = {
        cpv(bb.l, bb.b),
        cpv(bb.l, bb.t),
        cpv(bb.r, bb.t),
        cpv(bb.r, bb.b),
    };
    ChipmunkDebugDrawPolygon(4, verts, color, LAColor(0, 0));
}

void PhysicsDebugRenderer::staticDrawConstraint(cpConstraint *constraint, void *unused)
{

}

void PhysicsDebugRenderer::staticDrawShape(cpShape *shape, void *unused)
{
    s_instance->ChipmunkDebugDrawShape(shape);
}

void PhysicsDebugRenderer::staticDrawSpring(cpDampedSpring *spring, cpBody *body_a, cpBody *body_b)
{
//    cpVect a = cpvadd(body_a->p, cpvrotate(spring->anchr1, body_a->rot));
//    cpVect b = cpvadd(body_b->p, cpvrotate(spring->anchr2, body_b->rot));
//
//    cpVect points[] = {a, b};
//    ChipmunkDebugDrawPoints(5, 2, points, CONSTRAINT_COLOR);
//
//    cpVect delta = cpvsub(b, a);
//
//    glVertexPointer(2, GL_DOUBLE, 0, springVAR);
//    glPushMatrix(); {
//        GLdouble x = a.x;
//        GLdouble y = a.y;
//        GLdouble cos = delta.x;
//        GLdouble sin = delta.y;
//        GLdouble s = 1.0f/cpvlength(delta);
//
//        const GLdouble matrix[] = {
//            cos,    sin, 0.0f, 0.0f,
//            -sin*s,  cos*s, 0.0f, 0.0f,
//            0.0f,   0.0f, 1.0f, 0.0f,
//            x,      y, 0.0f, 1.0f,
//        };
//
//        glMultMatrixf(matrix);
//        glDrawArrays(GL_LINE_STRIP, 0, springVAR_count);
//    } glPopMatrix();
}


//shape
//FIXME HACK

void PhysicsDebugRenderer::ChipmunkDebugDrawShape(cpShape *shape)
{
    assert(shape);

    cpBody *body = shape->body;
    Color color = ColorForShape(shape);

    switch(shape->klass->type){
        case CP_CIRCLE_SHAPE: {
            cpCircleShape *circle = (cpCircleShape *)shape;
            DrawSolidCircle(circle->tc, circle->r, body->a, color);
            break;
        }
        case CP_SEGMENT_SHAPE: {
            cpSegmentShape *seg = (cpSegmentShape *)shape;
//                ChipmunkDebugDrawFatSegment(seg->ta, seg->tb, seg->r, LINE_COLOR, color);
            break;
        }
        case CP_POLY_SHAPE: {
            cpPolyShape *poly = (cpPolyShape *)shape;
            //ChipmunkDebugDrawPolygon(poly->numVerts, poly->tVerts, LINE_COLOR, color);
            DrawSolidPolygon(poly->tVerts, poly->numVerts, color);
            break;
        }
        default: break;
    }
}

void PhysicsDebugRenderer::iterateShapesInSpace(cpSpace *space)
{
    assert(space);

    m_mutex.lock();

    m_verticesSolidCircles.clear();
    m_indicesSolidCircles.clear();
    m_verticesSolidPolygons.clear();
    m_indicesSolidPolygons.clear();
    m_verticesPolygons.clear();
    m_indicesPolygons.clear();
    m_verticesSegments.clear();
    m_indicesSegments.clear();

    cpSpaceEachShape(space, staticDrawShape, NULL);

//    Debug::log(Debug::StartupArea) << "ITERATING SHAPES IN SPACE COUNT: " << m_shapes.size();

    m_mutex.unlock();
}

static const GLdouble springVAR[] = {
    0.00f, 0.0f,
    0.20f, 0.0f,
    0.25f, 3.0f,
    0.30f,-6.0f,
    0.35f, 6.0f,
    0.40f,-6.0f,
    0.45f, 6.0f,
    0.50f,-6.0f,
    0.55f, 6.0f,
    0.60f,-6.0f,
    0.65f, 6.0f,
    0.70f,-3.0f,
    0.75f, 6.0f,
    0.80f, 0.0f,
    1.00f, 0.0f,
};
static const int springVAR_count = sizeof(springVAR)/sizeof(GLdouble)/2;

void PhysicsDebugRenderer::drawConstraint(cpConstraint *constraint, void *unused)
{
//    cpBody *body_a = constraint->a;
//    cpBody *body_b = constraint->b;
//
//    const cpConstraintClass *klass = constraint->klass;
//    if(klass == cpPinJointGetClass()){
//        cpPinJoint *joint = (cpPinJoint *)constraint;
//
//        cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
//        cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
//
//        cpVect points[] = {a, b};
//        ChipmunkDebugDrawPoints(5, 2, points, CONSTRAINT_COLOR);
//        ChipmunkDebugDrawSegment(a, b, CONSTRAINT_COLOR);
//    } else if(klass == cpSlideJointGetClass()){
//        cpSlideJoint *joint = (cpSlideJoint *)constraint;
//
//        cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
//        cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
//
//        cpVect points[] = {a, b};
//        ChipmunkDebugDrawPoints(5, 2, points, CONSTRAINT_COLOR);
//        ChipmunkDebugDrawSegment(a, b, CONSTRAINT_COLOR);
//    } else if(klass == cpPivotJointGetClass()){
//        cpPivotJoint *joint = (cpPivotJoint *)constraint;
//
//        cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
//        cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
//
//        cpVect points[] = {a, b};
//        ChipmunkDebugDrawPoints(10, 2, points, CONSTRAINT_COLOR);
//    } else if(klass == cpGrooveJointGetClass()){
//        cpGrooveJoint *joint = (cpGrooveJoint *)constraint;
//
//        cpVect a = cpvadd(body_a->p, cpvrotate(joint->grv_a, body_a->rot));
//        cpVect b = cpvadd(body_a->p, cpvrotate(joint->grv_b, body_a->rot));
//        cpVect c = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));
//
//        ChipmunkDebugDrawPoints(5, 1, &c, CONSTRAINT_COLOR);
//        ChipmunkDebugDrawSegment(a, b, CONSTRAINT_COLOR);
//    } else if(klass == cpDampedSpringGetClass()){
//        staticDrawSpring((cpDampedSpring *)constraint, body_a, body_b);
//    }
}

void PhysicsDebugRenderer::ChipmunkDebugDrawConstraint(cpConstraint *constraint)
{
//    staticDrawConstraint(constraint, NULL);
}

void PhysicsDebugRenderer::ChipmunkDebugDrawConstraints(cpSpace *space)
{
//    cpSpaceEachConstraint(space, staticDrawConstraint, NULL);
}

void PhysicsDebugRenderer::ChipmunkDebugDrawCollisionPoints(cpSpace *space)
{
//    cpArray *arbiters = space->arbiters;
//
//    glColor3f(1.0f, 0.0f, 0.0f);
//    glPointSize(4.0f*ChipmunkDebugDrawPointLineScale);
//
//    glBegin(GL_POINTS); {
//        for(int i=0; i<arbiters->num; i++){
//            cpArbiter *arb = (cpArbiter*)arbiters->arr[i];
//
//            for(int j=0; j<arb->numContacts; j++){
//                cpVect v = arb->contacts[j].p;
//                glVertex2f(v.x, v.y);
//            }
//        }
//    } glEnd();
}
