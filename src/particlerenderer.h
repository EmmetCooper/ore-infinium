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

#ifndef PARTICLERENDERER_H
#define PARTICLERENDERER_H

#include "debug.h"
#include "block.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glew.h"

#include <map>
#include <string>
#include <vector>

class Texture;
class Image;

namespace Entities
{
class Player;
}

class Camera;
class Image;
class World;
class Shader;

class ParticleRenderer
{
public:
    explicit ParticleRenderer(World* world, Camera* camera, Entities::Player* mainPlayer);
    ~ParticleRenderer();

    void render();

private:
    bool check_program_link_status(GLuint obj);
    bool check_shader_compile_status(GLuint obj);

    void initGL();

    void pushMatrix();

    GLuint m_shaderProgramSmoke, m_shaderSmokeVertex, m_shaderSmokeFragment;

    GLuint m_shaderProgramSmokeTransform, m_shaderSmokeVertexTransform;

    glm::mat4 m_view;
    glm::mat4 m_ortho;
};

#endif
