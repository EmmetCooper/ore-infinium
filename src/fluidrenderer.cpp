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

#include "fluidrenderer.h"

#include "src/debug.h"
#include "src/camera.h"
#include "src/shader.h"
#include "src/texture.h"
#include "src/player.h"
#include "src/object.h"

#include "settings/settings.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

FluidRenderer::FluidRenderer(Camera* camera, Entities::Player* mainPlayer)
: m_camera(camera),
  m_mainPlayer(mainPlayer)
{
    return;
    Debug::assertf(false, "");
    m_shaderWater = new Shader("fluidrenderer_water.vert", "fluidrenderer_water.frag");
    setCamera(camera);

    initGL();
}

FluidRenderer::~FluidRenderer()
{
    glDeleteBuffers(1, &m_vboWater);
    glDeleteBuffers(1, &m_eboWater);
    glDeleteBuffers(1, &m_vboWater);
    glDeleteBuffers(1, &m_eboWater);

    glDeleteVertexArrays(1, &m_vaoWater);
    glDeleteVertexArrays(1, &m_vaoWater);

    delete m_shaderWater;
}

void FluidRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->addShader(m_shaderWater);
}

void FluidRenderer::renderWater()
{
    m_shaderWater->bindProgram();

    std::vector<Object*> results;

    double left = m_mainPlayer->position().x - Settings::instance()->screenResolutionWidth * 0.5;
    double top = m_mainPlayer->position().y - Settings::instance()->screenResolutionHeight * 0.5;
    double right = m_mainPlayer->position().x + Settings::instance()->screenResolutionWidth;
    double bottom = m_mainPlayer->position().x + Settings::instance()->screenResolutionHeight;

    m_spatialHashWater->queryRangeNonOverlapping(&results, left, top, right, bottom);

    Debug::checkGLError();
//
//    int index = 0;
//    for (Object* sprite : m_entitySprites) {
//        // vertices that will be uploaded.
//        Vertex vertices[4];
//
//        // vertices[n][0] -> X, and [1] -> Y
//        // vertices[0] -> top left
//        // vertices[1] -> bottom left
//        // vertices[2] -> bottom right
//        // vertices[3] -> top right
//
//        glm::vec2 spritePosition = sprite->position();
//
//        glm::vec2 spriteSize = sprite->sizeMeters();
//
//        glm::vec4 rect = glm::vec4(spritePosition.x - (spriteSize.x * 0.5f), spritePosition.y - (spriteSize.x * 0.5f), spritePosition.x + (spriteSize.x * 0.5f), spritePosition.y + (spriteSize.y * 0.5f));
//
//        float x = rect.x;
//        float width = rect.z;
//
//        float y = rect.y;
//        float height = rect.w;
//
//        vertices[0].position.x = x; // top left X
//        vertices[0].position.y = y; //top left Y
//
//        vertices[1].position.x = x; // bottom left X
//        vertices[1].position.y = height; // bottom left Y
//
//        vertices[2].position.x = width; // bottom right X
//        vertices[2].position.y = height; //bottom right Y
//
//        vertices[3].position.x = width; // top right X
//        vertices[3].position.y = y; // top right Y
//
//        Debug::checkGLError();
//
//        // copy color to the buffer
//        for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
//            //        *colorp = color.bgra;
//            uint8_t red = 255;
//            uint8_t green = 255;
//            uint8_t blue = 255;
//            uint8_t alpha = 255;
//            int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
//            vertices[i].color = color;
//        }
//        /*
//        const float tileWidth = 1.0f / TILESHEET_WIDTH * 16.0f;
//        const float tileHeight = 1.0f / TILESHEET_HEIGHT * 16.0f;
//
//        float xPadding = 1.0f / TILESHEET_WIDTH * 1.0f * (column + 1);
//        float yPadding = 1.0f / TILESHEET_HEIGHT * 1.0f * (row + 1);
//
//        const float tileLeft = (column *  tileWidth) + xPadding;
//        const float tileRight = tileLeft + tileWidth;
//        const float tileTop = 1.0f - ((row * tileHeight)) - yPadding;
//        const float tileBottom = tileTop - tileHeight;
//
//        // copy texcoords to the buffer
//        vertices[0].u = vertices[1].u = tileLeft;
//        vertices[0].v = vertices[3].v = tileTop;
//        vertices[1].v = vertices[2].v = tileBottom;
//        vertices[2].u = vertices[3].u = tileRight;
//        */
//
//        // copy texcoords to the buffer
//        const float textureWidth = float(frame.width) / float(SPRITESHEET_WIDTH);
//        const float textureHeight = float(frame.height) / float(SPRITESHEET_HEIGHT);
//        const float textureX = float(frame.x) / float(SPRITESHEET_WIDTH);
//        const float textureY = float(frame.y) / float(SPRITESHEET_HEIGHT);
//
//        const float spriteLeft = textureX;
//        const float spriteRight = spriteLeft + textureWidth;
//        const float spriteTop = 1.0f - (textureY);
//        const float spriteBottom = spriteTop - textureHeight;
//
//        // copy texcoords to the buffer
//        vertices[0].uv.x = vertices[1].uv.x = spriteLeft;
//        vertices[0].uv.y = vertices[3].uv.y = spriteTop;
//        vertices[1].uv.y = vertices[2].uv.y = spriteBottom;
//        vertices[2].uv.x = vertices[3].uv.x = spriteRight;
//
//        // finally upload everything to the actual vbo
//        glBindBuffer(GL_ARRAY_BUFFER, m_vboWater);
//        glBufferSubData(
//            GL_ARRAY_BUFFER,
//            sizeof(vertices) * index,
//            sizeof(vertices),
//            vertices);
//
//        ++index;
//    }
//
//    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboWater);
//    glBindBuffer(GL_ARRAY_BUFFER, m_vboWater);
//    glBindVertexArray(m_vaoWater);
//
//    Debug::checkGLError();
//
//    m_shaderWater->bindProgram();
//
//    Debug::checkGLError();
//
//    glDrawElements(
//        GL_TRIANGLES,
//        6 * (m_entitySprites.size()), // 6 indices per 2 triangles
//        GL_UNSIGNED_INT,
//        (const GLvoid*)0);
//
//    m_shaderWater->unbindProgram();
//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//    glDisable(GL_BLEND);
//
//    Debug::checkGLError();
}

void FluidRenderer::initGL()
{
    Debug::checkGLError();
    initGLWater();
    Debug::checkGLError();
}

void FluidRenderer::initGLWater()
{
    /////////////////////////////// ENTITIES
    glGenVertexArrays(1, &m_vaoWater);
    glBindVertexArray(m_vaoWater);

    glGenBuffers(1, &m_vboWater);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboWater);
    glBufferData(GL_ARRAY_BUFFER, m_maxWaterCount * 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < m_maxWaterCount; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_eboWater);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboWater);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesv.size() * sizeof(uint32_t), indicesv.data(), GL_STATIC_DRAW);

    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shaderWater->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)buffer_offset);

    buffer_offset += sizeof(glm::vec2);

    GLint color_attrib = glGetAttribLocation(m_shaderWater->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(color_attrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    Debug::checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shaderWater->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
