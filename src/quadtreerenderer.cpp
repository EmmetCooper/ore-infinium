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

#include "quadtreerenderer.h"

#include "src/quadtree.h"

#include "debug.h"
#include "world.h"

#include "settings/settings.h"

QuadTreeRenderer::QuadTreeRenderer(Camera* camera)
{
    return;
    m_shader = new Shader("quadtreerenderer.vert", "quadtreerenderer.frag");
    m_shader->bindProgram();
    setCamera(camera);

    Debug::checkGLError();
    initGL();
    Debug::checkGLError();
}

QuadTreeRenderer::~QuadTreeRenderer()
{

    // NOTE: cam is not ours to delete.
    delete m_shader;
}

void QuadTreeRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->addShader(m_shader);
}

void QuadTreeRenderer::initGL()
{
    ///////////CHARACTERS////////////////
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxVBOSize * 4 * sizeof(Vertex),
        NULL,
        GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (size_t j = 0; j < m_maxVBOSize; j++) {
        for (size_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(uint32_t),
        indicesv.data(),
        GL_STATIC_DRAW);

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
        (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(float) * 2;

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

    Debug::checkGLError();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();
}

void QuadTreeRenderer::addQuadTree(QuadTree* tree)
{
    m_trees.push_back(tree);
}

void QuadTreeRenderer::uploadVertices(QuadTree *tree)
{
    if (tree == nullptr) {
        return;
    }

    // vertices that will be uploaded.
    Vertex vertices[4];

    // vertices[n][0] -> X, and [1] -> Y
    // vertices[0] -> top left
    // vertices[1] -> bottom left
    // vertices[2] -> bottom right
    // vertices[3] -> top right
//BIG HACK TIME
    glm::vec2 spritePosition;//FIXME:(tree->m_boundary.center.x - tree->m_boundary.halfDimension.x, tree->m_boundary.center.y - tree->m_boundary.halfDimension.y);

    glm::vec4 rect;//FIXME: = glm::vec4(spritePosition.x - (tree->m_boundary.halfDimension.x * 2.0f), spritePosition.y - (tree->m_boundary.halfDimension.x * 2.0f), spritePosition.x + (tree->m_boundary.halfDimension.x * 2.0f), spritePosition.y + (tree->m_boundary.halfDimension.y * 2.0f));

    float x = rect.x;
    float width = rect.z;
    float y = rect.y;
    float height = rect.w;

    vertices[0].x = x; // top left X
    vertices[0].y = y; //top left Y

    vertices[1].x = x; // bottom left X
    vertices[1].y = height; // bottom left Y

    vertices[2].x = width; // bottom right X
    vertices[2].y = height; //bottom right Y

    vertices[3].x = width; // top right X
    vertices[3].y = y; // top right Y

    Debug::checkGLError();
    // copy color to the buffer
    for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
        //        *colorp = color.bgra;
        uint8_t red = 255;
        uint8_t green = 0;
        uint8_t blue = 0;
        uint8_t alpha = 10;
        int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertices[i].color = color;
    }

    // finally upload everything to the actual vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices) * m_index,
        sizeof(vertices),
        vertices);

    ++m_index;

    uploadVertices(tree->NE);
    uploadVertices(tree->NW);
    uploadVertices(tree->SE);
    uploadVertices(tree->SW);
}

void QuadTreeRenderer::render()
{
    m_index = 0;
    for (QuadTree * tree : m_trees) {
        uploadVertices(tree);
    }

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    Debug::checkGLError();

    m_shader->bindProgram();

    Debug::checkGLError();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(
        GL_TRIANGLES,
        6 * (m_index + 1), // 6 indices per 2 triangles
        GL_UNSIGNED_INT,
        (const GLvoid*)0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
}
