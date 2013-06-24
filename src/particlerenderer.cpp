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

#include "particlerenderer.h"

#include "src/world.h"
#include "src/camera.h"
#include "src/player.h"
#include "src/texture.h"
#include "shader.h"

#include <SDL_timer.h>

#include <fstream>

#include <glm/core/func_common.hpp>
#include <glm/gtc/type_ptr.hpp>

ParticleRenderer::ParticleRenderer(World* world, Camera* camera, Entities::Player* mainPlayer)
{
    initGL();
}

ParticleRenderer::~ParticleRenderer()
{
    // delete the created objects

//    glDeleteVertexArrays(buffercount, vao);
//    glDeleteBuffers(buffercount, vbo);
//
//    glDetachShader(shader_program, vertex_shader);
//    glDetachShader(shader_program, geometry_shader);
//    glDetachShader(shader_program, fragment_shader);
//    glDeleteShader(vertex_shader);
//    glDeleteShader(geometry_shader);
//    glDeleteShader(fragment_shader);
//    glDeleteProgram(shader_program);
//
//    glDetachShader(transform_shader_program, transform_vertex_shader);
//    glDeleteShader(transform_vertex_shader);
//    glDeleteProgram(transform_shader_program);

}

static float randFloat()
{
    return ((float)rand() / RAND_MAX);
}

void ParticleRenderer::initGL()
{

    // shader source code

    std::string vertex_source = Shader::loadFile("particlerenderer_smoke.vert");

    std::string fragment_source = Shader::loadFile("particlerenderer_smoke.frag");

//    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertexArray = vertex_source.c_str();

    glShaderSource(vertex_shader, 1, &vertexArray, nullptr);
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader))
    {
        exit(1);
    }

    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* fragmentArray = fragment_source.c_str();
    glShaderSource(fragment_shader, 1, &fragmentArray, nullptr);
    glCompileShader(fragment_shader);
    if(!check_shader_compile_status(fragment_shader))
    {
        exit(1);
    }

    Debug::checkGLError();
    // create program
    shader_program = glCreateProgram();

    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    const char * outputNames[] = { "Position", "Velocity", "StartTime" };
    glTransformFeedbackVaryings(shader_program, 3, outputNames, GL_SEPARATE_ATTRIBS);

    // link the program and check for errors
    glLinkProgram(shader_program);
    check_program_link_status(shader_program);

    Debug::checkGLError();

    /// FIXME: HACK: ///////////////

}

void ParticleRenderer::render()
{

}

void ParticleRenderer::pushMatrix()
{

    Debug::checkGLError();
    glm::mat4 mvp = view * model;
//    prog.setUniform("MVP", projection * mv);
//    glm::mat4 mvp =  m_orthoMatrix * m_viewMatrix;

    glm::mat4 final = projection* mvp;
    int mvpLoc = glGetUniformLocation(shader_program, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &final[0][0]);
}


// helper to check and display for shader compiler errors
bool ParticleRenderer::check_shader_compile_status(GLuint obj)
{
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }
    return true;
}

// helper to check and display for shader linker error
bool ParticleRenderer::check_program_link_status(GLuint obj)
{
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(obj, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }
    return true;
}
