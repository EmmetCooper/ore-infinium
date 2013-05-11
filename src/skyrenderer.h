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

class Camera;
class Texture;
class World;
class Shader;
class Camera;
class Time;

class SkyRenderer
{
public:
    SkyRenderer(World* world, Camera* camera, Time* time);
    ~SkyRenderer();

    void update(const float elapsedTime);
    void render();

private:
    void initGL();
    void initGLSkyBackground();
    void initGLCelestialBodies();

    void renderSkyBackgroundDay();
    void renderCelestialBodies();

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

    ///size of celestialBodies.png
    const int SPRITESHEET_WIDTH = 1024;
    const int SPRITESHEET_HEIGHT = 1024;
    const int SKY_TEXTURE_WIDTH = 1920;
    const int SKY_TEXTURE_HEIGHT = 1080;

    struct SpriteFrame {
        /// screen position of said object
       glm::vec2 position;
       /// size in meters
       glm::vec2 sizeMeters;

       /// uv position, within spritesheet texture.
       glm::vec2 texturePosition;
       /// uv size, within spritesheet texture.
       glm::vec2 textureSize;
    };

    GLuint m_vaoSkyBackground; // vertex array object
    GLuint m_vboSkyBackground; // vertex buffer object
    GLuint m_eboSkyBackground; // element buffer object

    GLuint m_vaoCelestialBodies; // vertex array object
    GLuint m_vboCelestialBodies; // vertex buffer object
    GLuint m_eboCelestialBodies; // element buffer object

    std::vector<SpriteFrame> m_celestialBodiesSprites;

    // sun, moon
    uint16_t m_maxCelestialBodies = 2;
    uint16_t m_maxSkyBackgrounds= 1;

    Texture* m_celestialBodiesTexture = nullptr;
    Texture* m_skyBackgroundDayTexture = nullptr;
    Shader* m_celestialBodiesShader = nullptr;
    Shader* m_skyBackgroundDayShader = nullptr;

    //CloudSystem *m_cloudSystem = nullptr;

    glm::mat4 m_orthoMatrix;
    glm::mat4 m_viewMatrix;

    Camera* m_camera = nullptr;
    Time* m_time = nullptr;
};

#endif
