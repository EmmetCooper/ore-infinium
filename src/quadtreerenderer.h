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

#ifndef QUADTREERENDERER_H
#define QUADTREERENDERER_H

#include "src/camera.h"
#include "src/shader.h"

#include <GL/glew.h>
#include <Box2D/Box2D.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>

class QuadTreeRenderer
{
public:
    QuadTreeRenderer(Camera* camera);
    ~QuadTreeRenderer();

    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    void setCamera(Camera* camera);

    void render();

private:
    void initGL();

    void renderSolidPolygons();
    void renderPolygons();

    /* Each vertex is:
     * two floats for the 2d coordinate
     * four uint8_t's for the color (packed in 1 uint32_t)
     * */
    struct Vertex {
        float x, y;
        uint32_t color; // packed with 4 u8s (unsigned chars) for color
    };

private:

    ////////////////////////////////////////////////////////////////////// NON SOLID POLYGONS
    GLuint m_iboPolygons; // index buffer object
    GLuint m_vaoPolygons; // vertex array object
    GLuint m_vboPolygons; // vertex buffer object

    std::vector<Vertex> m_verticesPolygons;
    std::vector<uint16_t> m_indicesPolygons;
    size_t m_maxVBOSizePolygons = 0;
    size_t m_highestIBOSizePolygons = 0;
    /////////////////////////////////////////////////////////////////////

    glm::mat4 m_ortho;

    Camera* m_camera = nullptr;
    Shader* m_shader = nullptr;
};

#endif
