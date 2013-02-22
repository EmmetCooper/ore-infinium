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

#include "tilerenderer.h"

#include "debug.h"
#include "game.h"
#include "camera.h"
#include "shader.h"
#include "image.h"

#include "src/world.h"
#include "src/player.h"

#include "settings/settings.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

TileRenderer::TileRenderer(World* world, Camera* camera, Player* mainPlayer)
    :   m_world(world),
        m_mainPlayer(mainPlayer)
{
    m_shader = new Shader("tilerenderer.vert", "tilerenderer.frag");
    setCamera(camera);

    initGL();

    float scale = 1.0f;
    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    m_projectionMatrix = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);
    loadTileSheets();
}

TileRenderer::~TileRenderer()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);

    glDeleteVertexArrays(1, &m_vao);
}

void TileRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->setShader(m_shader);
}

void TileRenderer::loadTileSheets()
{
    glGenTextures(1, &m_tileMapTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_tileMapTexture);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const GLint level = 0;
    glTexImage3D(GL_TEXTURE_2D_ARRAY, level, GL_RGBA, TILESHEET_WIDTH, TILESHEET_HEIGHT, Block::blockTypeMap.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 /* if it's null it tells GL we will send in 2D images as elements one by one, later */);

    for (auto& tile : Block::blockTypeMap) {
       loadTileSheet(tile.second.texture, tile.first);
    }
}

void TileRenderer::loadTileSheet(const std::string& fileName, Block::BlockType type)
{
    Image* image = new Image(Block::blockTypeMap.at(type).texture);

    const GLint level = 0;
    const GLint xoffset = 0;
    const GLint yoffset = 0;
    const GLint zoffset = m_tileSheetCount;
    const GLsizei depth = 1;

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xoffset, yoffset, zoffset, TILESHEET_WIDTH, TILESHEET_HEIGHT, depth, GL_BGRA, GL_UNSIGNED_BYTE, image->bytes());

    m_tileSheets[type] = image;

    ++m_tileSheetCount;
}

void TileRenderer::render()
{
    m_shader->bindProgram();


    Debug::checkGLError();

    glm::vec2 playerPosition = glm::vec2(1800/2, 1200/2);

    //consider block map as starting at player pos == 0,0 and going down and to the right-ward
    //tilesBefore{X,Y} is only at the center of the view though..find the whole screen real estate
    //column
    int tilesBeforeX = playerPosition.x / Block::blockSize;
    //row
    int tilesBeforeY = playerPosition.y / Block::blockSize;

    //FIXME: only calculate this crap when we move/change tiles
    // -1 so that we render an additional row and column..to smoothly scroll
    const int startRow = tilesBeforeY - ((Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize) - 1;
    const int endRow = tilesBeforeY + ((Settings::instance()->screenResolutionHeight * 0.5) / Block::blockSize);

    //columns are our X value, rows the Y
    const int startColumn = tilesBeforeX - ((Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize) - 1;
    const int endColumn = tilesBeforeX + ((Settings::instance()->screenResolutionWidth * 0.5) / Block::blockSize);

    if (std::abs(startColumn) != startColumn) {
        std::cout << "FIXME, WENT INTO NEGATIVE COLUMN!!";
        assert(0);
    } else if (std::abs(startRow) != startRow) {
        std::cout << "FIXME, WENT INTO NEGATIVE ROW!!";
        assert(0);
    }

    int x = 0;
    int y = 0;

    int index = 0;

    Debug::checkGLError();
    // [y*rowlength + x]
    for (int currentRow = startRow; currentRow < endRow; ++currentRow) {
        for (int currentColumn = startColumn; currentColumn < endColumn; ++currentColumn) {

            // vertices that will be uploaded.
            Vertex vertices[4];

            // vertices[n][0] -> X, and [1] -> Y
            // vertices[0] -> top left
            // vertices[1] -> bottom left
            // vertices[2] -> bottom right
            // vertices[3] -> top right

            float positionX = Block::blockSize * currentColumn;
            float positionY = Block::blockSize * currentRow;

            glm::vec4 rect = glm::vec4(positionX, positionY, positionX + Block::blockSize, positionY + Block::blockSize);

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
                uint8_t blue = 255;
                uint8_t green = 255;
                uint8_t alpha = 255;
                int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
                vertices[i].color = color;
            }

            // copy texcoords to the buffer
            vertices[0].u = vertices[1].u = 0.0f;
            vertices[0].v = vertices[3].v = 1.0f;
            vertices[1].v = vertices[2].v = 0.0f;
            vertices[2].u = vertices[3].u = 1.0f;

            //FIXME: use tile type index
//            vertices[0].w = vertices[1].w = vertices[2].w = vertices[3].w = 1.0;

            Debug::checkGLError();
            // finally upload everything to the actual vbo
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(
                GL_ARRAY_BUFFER,
                sizeof(vertices) * index,
                            sizeof(vertices),
                            vertices);
            Debug::checkGLError();

            ++index;
        }
    }

    Debug::checkGLError();
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    Debug::checkGLError();

    m_shader->bindProgram();

    Debug::checkGLError();
//    Debug::log() << "RENDERING TILECOUNT: " << m_tileCount;

    glDrawElements(
        GL_TRIANGLES,
        6 * (index), // 6 indices per 2 triangles
                   GL_UNSIGNED_INT,
                   (const GLvoid*)0);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}

void TileRenderer::initGL()
{
    Debug::checkGLError();

    //////////////////////

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<u32> indicesv;

    // prepare and upload indices as a one time deal
    const u32 indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (size_t j = 0; j < m_maxSpriteCount; j++) {
        for (size_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(u32),
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
    buffer_offset += sizeof(f32) * 2;

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
    buffer_offset += sizeof(u32);

    Debug::checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        //2 + 1 because we need a depth the array of 2d textures
        2, //FIXME HACK
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();
}
