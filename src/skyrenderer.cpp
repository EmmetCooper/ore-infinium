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
#include "src/texture.h"
#include "src/debug.h"
#include "settings/settings.h"

#include <iostream>
#include <stdio.h>

#include <math.h>

SkyRenderer::SkyRenderer(World* world)
{
    m_sunMoonShader = new Shader("skyrenderer.vert", "skyrenderer.frag");

    m_sunTexture = new Texture("../textures/sun.png");
    m_sunTexture->generate(Texture::TextureFilterNearest);

    m_moonTexture = new Texture("../textures/moon.png");
    m_moonTexture->generate(Texture::TextureFilterNearest);

    float x = 0.0f;
    float y = 0.0f;
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
    Debug::log(Debug::Area::ClientRendererArea) << "sky renderer init, screen at width: " << Settings::instance()->screenResolutionWidth << " height: " << Settings::instance()->screenResolutionHeight;
    //    m_orthoMatrix = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);
    m_orthoMatrix = glm::ortho(0.0f, float(1600.0f/PIXELS_PER_METER), float(900.0f/PIXELS_PER_METER), 0.0f, -1.0f, 1.0f);

    m_sunMoonShader->bindProgram();

    glm::mat4 mvp =  m_orthoMatrix * m_viewMatrix;

    int mvpLoc = glGetUniformLocation(m_sunMoonShader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    m_sunMoonShader->unbindProgram();

    initGL();
}

SkyRenderer::~SkyRenderer()
{
    delete m_sunMoonShader;
    delete m_sunTexture;
    delete m_moonTexture;
}

void SkyRenderer::initGL()
{
    initGLSunMoon();
    //initGLSkyBackground();
}

void SkyRenderer::initGLSunMoon()
{
    glGenVertexArrays(1, &m_vaoSunMoon);
    glBindVertexArray(m_vaoSunMoon);

    const int sunMoonCount = 2;

    glGenBuffers(1, &m_vboSunMoon);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSunMoon);
    glBufferData(
        GL_ARRAY_BUFFER,
        sunMoonCount * 4 * sizeof(Vertex),
                 NULL,
                 GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < sunMoonCount; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_eboSunMoon);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboSunMoon);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(uint32_t),
                 indicesv.data(),
                 GL_STATIC_DRAW);

    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_sunMoonShader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(float) * 2;

    GLint color_attrib = glGetAttribLocation(m_sunMoonShader->shaderProgram(), "color");

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

    GLint texcoord_attrib = glGetAttribLocation(m_sunMoonShader->shaderProgram(), "texcoord");
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

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < m_maxSpriteCount; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
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

    GLint pos_attrib = glGetAttribLocation(m_sunMoonShader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
                          (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(float) * 2;

    GLint color_attrib = glGetAttribLocation(m_sunMoonShader->shaderProgram(), "color");

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

    GLint texcoord_attrib = glGetAttribLocation(m_sunMoonShader->shaderProgram(), "texcoord");
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


void SkyRenderer::renderSkyBackground()
{
    m_sunMoonShader->bindProgram();

    Debug::checkGLError();

    int index = 0;

    {
        // vertices that will be uploaded.
        Vertex vertices[4];

        // vertices[n][0] -> X, and [1] -> Y
        // vertices[0] -> top left
        // vertices[1] -> bottom left
        // vertices[2] -> bottom right
        // vertices[3] -> top right

        glm::vec2 spritePosition ;//HACK:= sprite->position();

        glm::vec2 spriteSize;//HACK: = sprite->sizeMeters();

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
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    Debug::checkGLError();

    m_sunMoonShader->bindProgram();

    Debug::checkGLError();

    glDrawElements(
        GL_TRIANGLES,
        6 * (m_maxSpriteCount), // 6 indices per 2 triangles
                GL_UNSIGNED_INT,
                (const GLvoid*)0);

    m_sunMoonShader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}

void SkyRenderer::renderSunAndMoon()
{
    m_sunMoonShader->bindProgram();

    Debug::checkGLError();

    SunOrMoon moon;
    moon.texture = m_moonTexture->textureHandle();
    moon.position = glm::vec2(200, 200);
    moon.size = glm::vec2(60, 60);

    SunOrMoon sun;
    sun.texture = m_sunTexture->textureHandle();
    sun.position = glm::vec2(500, 500);
    sun.size = glm::vec2(60, 60);

    std::vector<SunOrMoon> renderables;
    renderables.push_back(sun);
    renderables.push_back(moon);

    for (size_t index = 0; index < renderables.size(); ++index) {

        // vertices that will be uploaded.
        Vertex vertices[4];

        // vertices[n][0] -> X, and [1] -> Y
        // vertices[0] -> top left
        // vertices[1] -> bottom left
        // vertices[2] -> bottom right
        // vertices[3] -> top right

        glm::vec2 spritePosition = renderables.at(index).position;

        glm::vec2 spriteSize = renderables.at(index).size;

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
        glBindBuffer(GL_ARRAY_BUFFER, m_vboSunMoon);
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboSunMoon);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboSunMoon);
    glBindVertexArray(m_vaoSunMoon);

    Debug::checkGLError();

    m_sunMoonShader->bindProgram();

    Debug::checkGLError();

    glDrawElements(
        GL_TRIANGLES,
        6 * (renderables.size()), // 6 indices for 2 triangles
                GL_UNSIGNED_INT,
                (const GLvoid*)0);

    m_sunMoonShader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}

void SkyRenderer::update(const float elapsedTime)
{
//    sf::Vector2f _viewportCenter;
//    _viewportCenter.x = SCREEN_W / 2;
//    _viewportCenter.y = SCREEN_H / 2;
//
//    const unsigned char hour = Time::instance()->currentHour();
//    const unsigned char minute = Time::instance()->currentMinute();
//    std::cout << "MINUTE! " << (short) minute << "\n";
//
//    m_timeAngle = (hour * (180 / 12)) + (minute * (180 / 12) / 60 );
//
//    double angle = (m_timeAngle + 90) * (M_PI / 180);
//    float newX = _viewportCenter.x + cos(angle) * 400;
//    float newY = _viewportCenter.y + sin(angle) * 400;
//    m_sunPosition = sf::Vector2f(newX, newY);
//    m_sunSprite.setPosition(m_sunPosition);
//
//    angle = (m_timeAngle + 90 - 180) * (M_PI / 180);
//    newX = _viewportCenter.x + cos(angle) * 400;
//    newY = _viewportCenter.y + sin(angle) * 400;
//
//    m_moonPosition = sf::Vector2f(newX, newY);
//    m_moonSprite.setPosition(m_moonPosition);
}

void SkyRenderer::render()
{
    renderSunAndMoon();
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
