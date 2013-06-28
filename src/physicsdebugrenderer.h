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

#ifndef PHYSICSDEBUGRENDERER_H
#define PHYSICSDEBUGRENDERER_H

#include "src/camera.h"
#include "src/shader.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glew.h"

#include <mutex>

struct cpConstraint;
struct cpDampedSpring;
struct cpSpace;
struct cpBody;
struct cpShape;
struct cpVect;
struct cpBB;

class PhysicsDebugRenderer
{
public:
    typedef struct Color {
        float r, g, b, a;
    } Color;

    PhysicsDebugRenderer(Camera* camera);
    virtual ~PhysicsDebugRenderer();

    virtual void DrawPolygon(const cpVect* vertices, int32_t vertexCount, PhysicsDebugRenderer::Color color);
    virtual void DrawSolidPolygon(const cpVect* vertices, int32_t vertexCount, PhysicsDebugRenderer::Color color);

    virtual void DrawSolidCircle(cpVect center, cpFloat radius, cpFloat rotation, Color color);

    virtual void DrawSegment(const cpVect& p1, const cpVect& p2, PhysicsDebugRenderer::Color color);

    void setCamera(Camera* camera);

    void render();

    uint32_t shapeCount() {
        return m_shapeCount;
    }

    const Color LINE_COLOR = {200.0 / 255.0, 210.0 / 255.0, 230.0 / 255.0, 1.0};
    const Color CONSTRAINT_COLOR = {0.0, 0.75, 0.0, 1.0};
    const float SHAPE_ALPHA = 1.0;

    float ChipmunkDebugDrawPointLineScale = 1.0;

    static inline Color RGBAColor(float r, float g, float b, float a) {
        Color color = {r, g, b, a};
        return color;
    }

    static inline Color LAColor(float l, float a) {
        Color color = {l, l, l, a};
        return color;
    }

    static void staticDrawShape(cpShape *shape, void *unused);
    static void staticDrawConstraint(cpConstraint *constraint, void *unused);
    static void staticDrawSpring(cpDampedSpring *spring, cpBody *body_a, cpBody *body_b);

    Color ColorForShape(cpShape *shape);
    void glColor_from_color(Color color);
    Color ColorFromHash(cpHashValue hash, float alpha);

    void ChipmunkDebugDrawShape(cpShape *shape);

    /**
     * Should be called ONLY by the server thread, iterates over the space and finds what it has to draw, switches a mutex, adds it to a list
     * of objects the client has to draw.
     * NOTE: this function is indeed threadsafe, but only should be called from the server thread after cpStepSpace is called.
     *
     * Client will trigger a mutex as well when it comes to iterating over said list and drawing it.
     */
    void iterateShapesInSpace(cpSpace *space);

private:
    void initGL();
    void initGLSolidPolygons();
    void initGLSolidCircles();
    void initGLPolygons();
    void initGLSegments();

    void renderSolidPolygons();
    void renderSolidCircles();
    void renderPolygons();
    void renderSegments();

    /* Each vertex is:
     * two floats for the 2d coordinate
     * four uint8_t's for the color (packed in 1 uint32_t)
     * */
    struct Vertex {
        float x, y;
        uint32_t color; // packed with 4 u8s (unsigned chars) for color
    };

private:
    /////////////////////////////////////////////////////////////// SOLID POLYGONS
    GLuint m_iboSolidPolygons; // index buffer object
    GLuint m_vaoSolidPolygons; // vertex array object
    GLuint m_vboSolidPolygons; // vertex buffer object

    std::vector<Vertex> m_verticesSolidPolygons;
    std::vector<uint32_t> m_indicesSolidPolygons;
    size_t m_maxVBOSizeSolidPolygons = 0;
    size_t m_highestIBOSizeSolidPolygons = 0;
    //////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////// SOLID CIRCLES
    GLuint m_iboSolidCircles; // index buffer object
    GLuint m_vaoSolidCircles; // vertex array object
    GLuint m_vboSolidCircles; // vertex buffer object

    std::vector<Vertex> m_verticesSolidCircles;
    std::vector<uint32_t> m_indicesSolidCircles;
    size_t m_maxVBOSizeSolidCircles = 0;
    size_t m_highestIBOSizeSolidCircles = 0;
    //////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////// NON SOLID POLYGONS
    GLuint m_iboPolygons; // index buffer object
    GLuint m_vaoPolygons; // vertex array object
    GLuint m_vboPolygons; // vertex buffer object

    std::vector<Vertex> m_verticesPolygons;
    std::vector<uint32_t> m_indicesPolygons;
    size_t m_maxVBOSizePolygons = 0;
    size_t m_highestIBOSizePolygons = 0;
    /////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////// SEGMENTS
    GLuint m_iboSegments; // index buffer object
    GLuint m_vaoSegments; // vertex array object
    GLuint m_vboSegments; // vertex buffer object

    std::vector<Vertex> m_verticesSegments;
    std::vector<uint32_t> m_indicesSegments;
    size_t m_maxVBOSizeSegments = 0;
    size_t m_highestIBOSizeSegments = 0;
    //////////////////////////////////////////////////////////////

    glm::mat4 m_ortho;

    Camera* m_camera = nullptr;
    Shader* m_shader = nullptr;

    std::vector<cpShape*> m_shapes;

    std::mutex m_mutex;

    uint32_t m_shapeCount = 0;
};

#endif
