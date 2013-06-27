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

#ifndef FLUIDRENDERER_H
#define FLUIDRENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <GL/glew.h>

#include <map>
#include <string>
#include <vector>

class Torch;
class Camera;
class Texture;

class FluidRenderer
{
public:
    FluidRenderer(Camera* camera);
    ~FluidRenderer();

    void setCamera(Camera* camera);
    Camera* camera() {
        return m_camera;
    }

private:
    struct Vertex {
        glm::vec2 position;
        unsigned int color; // packed with 4 u8s (unsigned chars) for color
        glm::vec2 uv;
    };

    void initGL();
    void initGLCharacters();

    GLuint m_vao; // vertex array object
    GLuint m_vbo; // vertex buffer object
    GLuint m_ebo; // element buffer object

    uint32_t m_maxWaterCount = 22000;

    Camera* m_camera = nullptr;
    Shader* m_shaderWater = nullptr;
};

#endif
