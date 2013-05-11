/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#include "skyrenderer.h"

#include "src/world.h"
#include "src/shader.h"
#include "src/time.h"
#include "src/camera.h"
#include "src/texture.h"
#include "src/debug.h"
#include "settings/settings.h"

#include <iostream>
#include <stdio.h>

#include <math.h>

SkyRenderer::SkyRenderer(World* world, Camera* camera, Time* time) :
m_camera(camera),
m_time(time)
{
    m_celestialBodiesShader = new Shader("skyrenderer.vert", "skyrenderer.frag");
    m_skyBackgroundDayShader = new Shader("skyrendererday.vert", "skyrendererday.frag");

    m_celestialBodiesTexture = new Texture("../textures/celestialBodies.png");
    m_celestialBodiesTexture->generate(Texture::TextureFilterNearest);

    m_skyBackgroundDayTexture = new Texture("../textures/skyBackgroundDay.png");
    m_skyBackgroundDayTexture->generate(Texture::TextureFilterNearest);

    m_skyBackgroundDuskTexture = new Texture("../textures/skyBackgroundDusk.png");
    m_skyBackgroundDuskTexture->generate(Texture::TextureFilterNearest);

    SpriteFrame sun;
    sun.texturePosition = glm::vec2(0, 0);
    sun.textureSize = glm::vec2(256, 256);
    sun.sizeMeters = glm::vec2(200, 200);
    sun.position = glm::vec2(500, 500);
    m_celestialBodiesSprites.push_back(sun);

    SpriteFrame moon;
    moon.texturePosition = glm::vec2(402, 54);
    moon.textureSize = glm::vec2(150, 160);
    moon.sizeMeters = glm::vec2(50, 50);
    moon.position = glm::vec2(500, 500);
    m_celestialBodiesSprites.push_back(moon);

    initGL();
}

SkyRenderer::~SkyRenderer()
{
    delete m_celestialBodiesShader;
    delete m_celestialBodiesTexture;
}

void SkyRenderer::initGL()
{
    initGLCelestialBodies();
    initGLSkyBackground();
}

void SkyRenderer::initGLCelestialBodies()
{
    glGenVertexArrays(1, &m_vaoCelestialBodies);
    glBindVertexArray(m_vaoCelestialBodies);

    glGenBuffers(1, &m_vboCelestialBodies);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboCelestialBodies);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxCelestialBodies * 4 * sizeof(Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < m_maxCelestialBodies; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_eboCelestialBodies);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboCelestialBodies);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(uint32_t),
                 indicesv.data(),
                 GL_STATIC_DRAW);

    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_celestialBodiesShader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(float) * 2;

    GLint color_attrib = glGetAttribLocation(m_celestialBodiesShader->shaderProgram(), "color");

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

    GLint texcoord_attrib = glGetAttribLocation(m_celestialBodiesShader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();

}

void SkyRenderer::initGLSkyBackground()
{
    glGenVertexArrays(1, &m_vaoSkyBackground);
    glBindVertexArray(m_vaoSkyBackground);

    glGenBuffers(1, &m_vboSkyBackground);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSkyBackground);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSkyBackgrounds * 4 * sizeof(Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < m_maxSkyBackgrounds; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_eboSkyBackground);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboSkyBackground);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(uint32_t),
                 indicesv.data(),
                 GL_STATIC_DRAW);

    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_skyBackgroundDayShader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(float) * 2;

    GLint color_attrib = glGetAttribLocation(m_skyBackgroundDayShader->shaderProgram(), "color");

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

    GLint texcoord_attrib = glGetAttribLocation(m_skyBackgroundDayShader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();
}

void SkyRenderer::renderSkyBackgroundDay()
{
    m_skyBackgroundDayShader->bindProgram();

    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));
    Debug::log(Debug::Area::ClientRendererArea) << "sky renderer init, screen at width: " << Settings::instance()->screenResolutionWidth << " height: " << Settings::instance()->screenResolutionHeight;
    //    m_orthoMatrix = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);

    m_orthoMatrix = glm::ortho(0.0f, float(1600.0f), float(900.0f), 0.0f, -1.0f, 1.0f);

    glm::mat4 mvp =  m_orthoMatrix * m_viewMatrix;

    int mvpLoc = glGetUniformLocation(m_skyBackgroundDayShader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    ////////////////////////////////////////////////// render... ////////////////////////////////////////////////

    glActiveTexture(GL_TEXTURE0);
    m_skyBackgroundDayTexture->bind();
    glActiveTexture(GL_TEXTURE1);
    m_skyBackgroundDuskTexture->bind();

    GLint dayTextureLoc = glGetUniformLocation(m_skyBackgroundDayShader->shaderProgram(), "dayTexture");
    glUniform1i(dayTextureLoc, 0);

    GLint duskTextureLoc = glGetUniformLocation(m_skyBackgroundDayShader->shaderProgram(), "duskTexture");
    glUniform1i(duskTextureLoc, 1);

    GLint timeLoc = glGetUniformLocation(m_skyBackgroundDayShader->shaderProgram(), "time");
    float time = std::max(float(m_time->currentHour() - 12) / 12.0f, 0.0f);
    glUniform1f(timeLoc, time);

    GLint heightLoc = glGetUniformLocation(m_skyBackgroundDayShader->shaderProgram(), "height");
    glUniform1f(heightLoc, 1024.0f);
    Debug::checkGLError();


    int index = 0;

    // vertices that will be uploaded.
    Vertex vertices[4];

    // vertices[n][0] -> X, and [1] -> Y
    // vertices[0] -> top left
    // vertices[1] -> bottom left
    // vertices[2] -> bottom right
    // vertices[3] -> top right

    glm::vec2 spritePosition = glm::vec2(Settings::instance()->screenResolutionWidth * 0.5f, Settings::instance()->screenResolutionWidth * 0.5f);

    glm::vec2 spriteSize = glm::vec2(SKY_TEXTURE_WIDTH, SKY_TEXTURE_HEIGHT);

    glm::vec4 rect = glm::vec4(spritePosition.x - (spriteSize.x * 0.5f), spritePosition.y - (spriteSize.x * 0.5f), spritePosition.x + (spriteSize.x * 0.5f), spritePosition.y + (spriteSize.y * 0.5f));

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
        uint8_t green = 255;
        uint8_t blue = 255;
        uint8_t alpha = 255;
        int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
        vertices[i].color = color;
    }

    const float spriteLeft = 0.0f;
    const float spriteRight = 1.0f;
    const float spriteTop = 0.0f;
    const float spriteBottom = 1.0f;

    // copy texcoords to the buffer
    vertices[0].u = vertices[1].u = spriteLeft;
    vertices[0].v = vertices[3].v = spriteTop;
    vertices[1].v = vertices[2].v = spriteBottom;
    vertices[2].u = vertices[3].u = spriteRight;

    // finally upload everything to the actual vbo
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSkyBackground);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        sizeof(vertices) * index,
                    sizeof(vertices),
                    vertices);

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboSkyBackground);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSkyBackground);
    glBindVertexArray(m_vaoSkyBackground);

    Debug::checkGLError();

    glDrawElements(
        GL_TRIANGLES,
        6 * (m_maxSkyBackgrounds), // 6 indices per 2 triangles
                GL_UNSIGNED_INT,
                (const GLvoid*)0);

    m_skyBackgroundDayShader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    Debug::checkGLError();
}

void SkyRenderer::renderCelestialBodies()
{
    m_celestialBodiesTexture->bind();
    m_celestialBodiesShader->bindProgram();

    float x = 0.0f;
    float y = 0.0f;
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
    Debug::log(Debug::Area::ClientRendererArea) << "sky renderer init, screen at width: " << Settings::instance()->screenResolutionWidth << " height: " << Settings::instance()->screenResolutionHeight;
    //    m_orthoMatrix = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);

    m_orthoMatrix = glm::ortho(0.0f, float(1600.0f), float(900.0f), 0.0f, -1.0f, 1.0f);

    glm::mat4 mvp =  m_orthoMatrix * m_viewMatrix;

    int mvpLoc = glGetUniformLocation(m_celestialBodiesShader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    ////////////////////////////////////////////////// render... ////////////////////////////////////////////////

    Debug::checkGLError();

    int index = 0;

    for (SpriteFrame frame : m_celestialBodiesSprites) {
        // vertices that will be uploaded.
        Vertex vertices[4];

        // vertices[n][0] -> X, and [1] -> Y
        // vertices[0] -> top left
        // vertices[1] -> bottom left
        // vertices[2] -> bottom right
        // vertices[3] -> top right

        glm::vec2 spritePosition = frame.position;

        glm::vec2 spriteSize = frame.sizeMeters;

        glm::vec4 rect = glm::vec4(spritePosition.x - (spriteSize.x * 0.5f), spritePosition.y - (spriteSize.x * 0.5f), spritePosition.x + (spriteSize.x * 0.5f), spritePosition.y + (spriteSize.y * 0.5f));

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
            uint8_t green = 255;
            uint8_t blue = 255;
            uint8_t alpha = 255;
            int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
            vertices[i].color = color;
        }

        // copy texcoords to the buffer
        const float textureWidth = float(frame.textureSize.x) / float(SPRITESHEET_WIDTH);
        const float textureHeight = float(frame.textureSize.y) / float(SPRITESHEET_HEIGHT);
        const float textureX = float(frame.texturePosition.x) / float(SPRITESHEET_WIDTH);
        const float textureY = float(frame.texturePosition.y) / float(SPRITESHEET_HEIGHT);

        const float spriteLeft = textureX;
        const float spriteRight = spriteLeft + textureWidth;
        const float spriteTop = 1.0f - (textureY);
        const float spriteBottom = spriteTop - textureHeight;

        // copy texcoords to the buffer
        vertices[0].u = vertices[1].u = spriteLeft;
        vertices[0].v = vertices[3].v = spriteTop;
        vertices[1].v = vertices[2].v = spriteBottom;
        vertices[2].u = vertices[3].u = spriteRight;

        // finally upload everything to the actual vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vboCelestialBodies);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            sizeof(vertices) * index,
                        sizeof(vertices),
                        vertices);

        ++index;
    }

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboCelestialBodies);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboCelestialBodies);
    glBindVertexArray(m_vaoCelestialBodies);

    Debug::checkGLError();

    glDrawElements(
        GL_TRIANGLES,
        6 * (m_celestialBodiesSprites.size()), // 6 indices per 2 triangles
                   GL_UNSIGNED_INT,
                   (const GLvoid*)0);

    m_celestialBodiesShader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}

void SkyRenderer::update(const float elapsedTime)
{
    assert(m_time);


    glm::vec2 viewportCenter = glm::vec2(1600/2, 900/2);
    const double hour = m_time->currentHour();
    const double minute = m_time->currentMinute();
    const double second = m_time->currentSecond();

    ///// SUN
    double timeAngle = 360.0 * (double(hour) * 60.0 + double(minute) + double(second) / 60.0) / (24.0 * 60.0);
    double angle = (timeAngle + 30.0) * (M_PI / 180.0);
    float newX = viewportCenter.x + cos(angle) * 400.0f;
    float newY = viewportCenter.y + sin(angle) * 400.0f;

    Debug::log(Debug::StartupArea) << "m_time: " << m_time->toString();
    // sun position
    m_celestialBodiesSprites.at(0).position = glm::vec2(newX, newY);
    ////////////

    //////////// MOON
    angle = (timeAngle + 90 - 180) * (M_PI / 180);
    newX = viewportCenter.x + cos(angle) * 400;
    newY = viewportCenter.y + sin(angle) * 400;

    // moon position
    m_celestialBodiesSprites.at(1).position = glm::vec2(newX, newY);
    /////////////////
}

void SkyRenderer::render()
{
    renderSkyBackgroundDay();
    renderCelestialBodies();

 //   sf::VertexArray line(sf::Lines, 2);
 //   sf::Vector2f screen = sf::Vector2f(SCREEN_W/2, SCREEN_H/2);
 //   line.append(sf::Vertex(screen));
 //   line.append(sf::Vertex(m_sunPosition));
 //   m_window->draw(line);
 //
 //   // sunrise at 7 am, sunset at 7am..
 //   const unsigned char hour = Time::instance()->currentHour();
 //
 //   // 7am, 7pm
 //   if (hour >= 7 && hour < 19) {
 //       m_window->draw(m_sunSprite);
 //       m_sunSprite.render(m_window);
 //   } else {
 //       m_window->draw(m_moonSprite);
 //       m_moonSprite.render(m_window);
 //   }
}
