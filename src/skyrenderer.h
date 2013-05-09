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

#ifndef SKY_H
#define SKY_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/core/type.hpp>
#include <vector>

class Texture;
class World;
class Shader;

class SkyRenderer
{
public:
    SkyRenderer(World* world);
    ~SkyRenderer();

    void update(const float elapsedTime);
    void render();

private:
    void initGL();
    void initGLSkyBackground();
    void initGLSunMoon();

    void renderSkyBackground();
    void renderSunAndMoon();

    /* Each vertex is:
     * two floats for the 2d coordinate
     * four u8s for the color
     * two f32s for the texcoords
     * the vbo contains data of the aforementioned elements interleaved.
     * Each sprite has four vertices.
     * */
    struct Vertex {
        float x, y;
        unsigned int color; // packed with 4 u8s (unsigned chars) for color
        float u, v;
    };

    struct SunOrMoon {
       GLuint texture;
       glm::vec2 position;
       glm::vec2 size;
    };

    Texture* m_sunTexture = nullptr;
    Texture* m_moonTexture = nullptr;

    //Renderable m_sunSprite;
    //Renderable m_moonSprite;
    //Renderable m_skyBox;

    //sf::Texture m_skyBoxDayTexture;
    //sf::Texture m_skyBoxDuskTexture;
    //sf::Texture m_skyBoxNightTexture;

    glm::vec2 m_sunPosition;
    glm::vec2 m_moonPosition;

    GLuint m_vao; // vertex array object
    GLuint m_vbo; // vertex buffer object
    GLuint m_ebo; // element buffer object

    GLuint m_vaoSunMoon; // vertex array object
    GLuint m_vboSunMoon; // vertex buffer object
    GLuint m_eboSunMoon; // element buffer object

    // sun, moon
    uint16_t m_maxSpriteCount = 2;

    Shader* m_sunMoonShader = nullptr;

    //CloudSystem *m_cloudSystem = nullptr;

    glm::mat4 m_orthoMatrix;
    glm::mat4 m_viewMatrix;

    float m_timeAngle = 0.0f;
    int m_hour = 0;
};

#endif
