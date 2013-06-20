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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <GL/glew.h>

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

    char* loadFile(const char* fname, GLint* fSize);
    bool check_program_link_status(GLuint obj);
    bool check_shader_compile_status(GLuint obj);

    void initGL();

    GLint center_location;
    GLint radius_location;
    GLint g_location;
    GLint dt_location;
    GLint bounce_location;
    GLint seed_location;

    GLuint transform_shader_program, transform_vertex_shader;

    GLuint shader_program, vertex_shader, geometry_shader, fragment_shader;

    int buffercount = 2;
    // generate vbos and vaos
    GLuint vao[2], vbo[2];

   GLint View_location;
    GLint Projection_location;

    const int particles = 128*1024 * 1;


    float angle;
    float time = 0.0f;
    float deltaT = 0.0f;
    int width, height;

    int nParticles = 1000;
    GLuint posBuf[2], velBuf[2];
    GLuint particleArray[2];
    GLuint feedback[2], initVel, startTime[2];
    GLuint drawBuf = 1;
    GLuint query;
    GLuint renderSub, updateSub;

    Texture* m_smokeTexture = nullptr;
};

#endif
