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

TileRenderer::TileRenderer(World* world, Camera* camera, Entities::Player* mainPlayer)
    :   m_mainPlayer(mainPlayer),
        m_world(world)
{
    m_shader = new Shader("tilerenderer.vert", "tilerenderer.frag");
    setCamera(camera);

    initGL();

    glGenTextures(1, &m_tileMapTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_tileMapTexture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);

    const GLint level = 0;
    glTexImage3D(GL_TEXTURE_2D_ARRAY, level, GL_RGBA, Block::BLOCK_SIZE_PIXELS, Block::BLOCK_SIZE_PIXELS, Block::blockTypes.size() + Block::wallTypes.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr /* if it's null it tells GL we will send in 2D images as elements one by one, later */);

    const GLint xoffset = 0;
    const GLint yoffset = 0;
    const GLsizei depth = 1;

    for (size_t i = 0; i < Block::blockTypes.size(); ++i) {
        Image image(Block::blockTypes.at(i).texture);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xoffset, yoffset, i, Block::BLOCK_SIZE_PIXELS, Block::BLOCK_SIZE_PIXELS, depth, GL_BGRA, GL_UNSIGNED_BYTE, image.bytes());
    }

    for (size_t i = 0; i < Block::wallTypes.size(); ++i) {
        Image image(Block::wallTypes.at(i).texture);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, level, xoffset, yoffset, (Block::blockTypes.size() + i), Block::BLOCK_SIZE_PIXELS, Block::BLOCK_SIZE_PIXELS, depth, GL_BGRA, GL_UNSIGNED_BYTE, image.bytes());
    }
}

TileRenderer::~TileRenderer()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);

    glDeleteVertexArrays(1, &m_vao);

    glDeleteFramebuffers(1, &m_fbo);
    glDeleteRenderbuffers(1, &m_rb);

    glDeleteTextures(1, &m_fboTexture);
    glDeleteTextures(1, &m_tileMapTexture);

    delete m_shader;
}

void TileRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->addShader(m_shader);
}

void TileRenderer::setRenderingEnabled(bool enabled)
{
    m_renderingEnabled = enabled;
}

GLuint TileRenderer::fboTexture()
{
    return m_fboTexture;
}

void TileRenderer::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
    glClearColor(0.f, 0.f, 0.f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_renderingEnabled) {
        return;
    }

    //Debug::log() << "OFFSET: " << offset.x << " Y : " << offset.y;
    Debug::checkGLError();
    glm::vec2 playerPosition = m_mainPlayer->position();

    float tilesBeforeX = playerPosition.x / Block::BLOCK_SIZE;
    float tilesBeforeY = playerPosition.y / Block::BLOCK_SIZE;

    // determine what the size of the tiles are but convert that to our zoom level
    const float blockSizePixels = Block::BLOCK_SIZE;
    glm::vec4 tileSize = glm::vec4(blockSizePixels, 0, 0, 0);
    const float transformedTileSize = glm::vec4(m_camera->view() * tileSize).x;

    float halfScreenMetersHeight = (Settings::instance()->screenResolutionHeight * 0.5) / PIXELS_PER_METER;
    float halfScreenMetersWidth = (Settings::instance()->screenResolutionWidth * 0.5) / PIXELS_PER_METER;

    // -1 so that we render an additional row and column..to smoothly scroll
    const int startRow = std::max(static_cast<int>(tilesBeforeY - (halfScreenMetersHeight / transformedTileSize)) - 2, 0);
    const int endRow = std::min(static_cast<int>(tilesBeforeY + (halfScreenMetersHeight / transformedTileSize) + 2), static_cast<int>(WORLD_ROWCOUNT));

    //columns are our X value, rows the Y
    const int startColumn = std::max(static_cast<int>(tilesBeforeX - (halfScreenMetersWidth / transformedTileSize)) - 2, 0);
    const int endColumn = std::min(static_cast<int>(tilesBeforeX + (halfScreenMetersWidth / transformedTileSize) + 2), static_cast<int>(WORLD_COLUMNCOUNT));

    //Debug:: log(Debug::TileRendererArea) << "tilesBeforeX: " << tilesBeforeX << " tilebeforeY: " << tilesBeforeY;
    //Debug:: log(Debug::TileRendererArea) << "halfScreenMetersHeight: " << halfScreenMetersHeight << " Width: " << halfScreenMetersWidth;

    //Debug:: log(Debug::TileRendererArea) << "starRow: " << startRow << "endrow: " << endRow << "startcol: " << startColumn << " endcol: " << endColumn;

    if (std::abs(startColumn) != startColumn) {
        Debug::log(Debug::TileRendererArea) << "FIXME, WENT INTO NEGATIVE COLUMN!!";
        assert(0);
    } else if (std::abs(startRow) != startRow) {
        Debug::log(Debug::TileRendererArea) << "FIXME, WENT INTO NEGATIVE ROW!!";
        assert(0);
    }

    //TODO: needs improvement, it's a bit of a hack to buffer blank data then subdata
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    const uint32_t totalTiles = (endRow - startRow) * (endColumn - startColumn);
    if (totalTiles > m_highestTileCount || m_firstRun) {
        Debug::log(Debug::TileRendererArea) << "reallocating buffer!";

        m_firstRun = false;
        m_highestTileCount = totalTiles * 2;

        std::vector<uint32_t> indicesv;

        // prepare and upload indices as a one time deal
        const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
        // for each possible sprite, add the 6 index pattern
        for (uint32_t j = 0; j < m_highestTileCount; j++) {
            for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
                indicesv.push_back(4 * j + indices[i]);
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBindVertexArray(m_vao);

        glDeleteBuffers(1, &m_ebo);
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indicesv.size() * sizeof(uint32_t),
            indicesv.data(),
            GL_STATIC_DRAW);

        glBufferData(
            GL_ARRAY_BUFFER,
            m_highestTileCount * 4 * sizeof(Vertex),
            NULL,
            GL_STREAM_DRAW);
    }

    int drawingRow = 0;
    int index = 0;

    struct Quad {
        Vertex vertices[4];
    };

    std::vector<Quad> quads;
    Debug::checkGLError();
    // [y*rowlength + x]
    for (int currentRow = startRow; currentRow < endRow; ++currentRow) {
        int drawingColumn = 0;
        for (int currentColumn = startColumn; currentColumn < endColumn; ++currentColumn) {
            Quad quad;

            // vertices that will be uploaded.

            // vertices[n][0] -> X, and [1] -> Y
            // vertices[0] -> top left
            // vertices[1] -> bottom left
            // vertices[2] -> bottom right
            // vertices[3] -> top right

            float positionX = Block::BLOCK_SIZE * currentColumn;
            float positionY = Block::BLOCK_SIZE * currentRow;

            float x = positionX;
            float width = x +  Block::BLOCK_SIZE;

            float y = positionY;
            float height = y  +  Block::BLOCK_SIZE;

            quad.vertices[0].x = x; // top left X
            quad.vertices[0].y = y; //top left Y

            quad.vertices[1].x = x; // bottom left X
            quad.vertices[1].y = height; // bottom left Y

            quad.vertices[2].x = width; // bottom right X
            quad.vertices[2].y = height; //bottom right Y

            quad.vertices[3].x = width; // top right X
            quad.vertices[3].y = y; // top right Y

            Debug::checkGLError();

            //tilesheet index/row, column
            int row = 0;
            int column = 0;

            int blockIndex = currentColumn * WORLD_ROWCOUNT + currentRow;
            assert(blockIndex >= 0);
            assert(blockIndex < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
            Block& block = m_world->m_blocks[blockIndex];

            const float tileWidth = 1.0f / float(Block::BLOCK_SIZE_PIXELS) * Block::BLOCK_SIZE_PIXELS;
            const float tileHeight = 1.0f / float(Block::BLOCK_SIZE_PIXELS) * Block::BLOCK_SIZE_PIXELS;

            float xPadding = 1.0f / float(Block::BLOCK_SIZE_PIXELS) * 1.0f * (float(column) + 1.0);
            float yPadding = 1.0f / float(Block::BLOCK_SIZE_PIXELS) * 1.0f * (float(row) + 1.0);

            const float tileLeft = (column *  tileWidth) + xPadding;
            const float tileRight = tileLeft + tileWidth;
            const float tileTop = 1.0f - ((row * tileHeight)) - yPadding;
            const float tileBottom = tileTop - tileHeight;

            // copy texcoords to the buffer
            quad.vertices[0].u = quad.vertices[1].u = tileLeft;
            quad.vertices[0].v = quad.vertices[3].v = tileTop;
            quad.vertices[1].v = quad.vertices[2].v = tileBottom;
            quad.vertices[2].u = quad.vertices[3].u = tileRight;

            // NOTE: block primitive type is in the perfect order that we need, so the first one (0) is at the top, and goes down and increases.
            float z = block.primitiveType;

            uint8_t red = 255;
            uint8_t blue = 255;
            uint8_t green = 255;
            uint8_t alpha = 255;

            if (block.wallType != Block::WallType::NullWallType && block.primitiveType == Block::BlockType::NullBlockType) {
                z = Block::blockTypes.size() + block.wallType;

                red = 120;
                blue = 120;
                green = 120;
            }

            int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);

            // copy color to the buffer
            for (size_t i = 0; i < sizeof(quad.vertices) / sizeof(*quad.vertices); i++) {
                //        *colorp = color.bgra;
                quad.vertices[i].color = color;
            }

            quad.vertices[0].w = quad.vertices[1].w = quad.vertices[2].w = quad.vertices[3].w = z;

            quads.push_back(quad);

            Debug::checkGLError();

            ++index;
            ++drawingColumn;
        }
        ++drawingRow;
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        quads.size() * sizeof(Quad),
        &quads[0],
        GL_STREAM_DRAW);

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);


    m_shader->bindProgram();

    Debug::checkGLError();
    //Debug::log() << "RENDERING TILECOUNT: " << m_tileCount;

    glDrawElements(
        GL_TRIANGLES,
        6 * (quads.size()), // 6 indices per 2 triangles
        GL_UNSIGNED_INT,
        (const GLvoid*)0);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    Debug::checkGLError();
}

void TileRenderer::initGL()
{
    Debug::checkGLError();

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenRenderbuffers(1, &m_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, Settings::instance()->windowWidth, Settings::instance()->windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_rb);

    GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Settings::instance()->screenResolutionWidth, Settings::instance()->screenResolutionHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Attach the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    //////////////////////

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

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

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        //2 + 1 because we need a depth the array of 2d textures
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    Debug::checkGLError();
}
