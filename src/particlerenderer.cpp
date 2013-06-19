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

    glDeleteVertexArrays(buffercount, vao);
    glDeleteBuffers(buffercount, vbo);

    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, geometry_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(geometry_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(shader_program);

    glDetachShader(transform_shader_program, transform_vertex_shader);
    glDeleteShader(transform_vertex_shader);
    glDeleteProgram(transform_shader_program);

}

void ParticleRenderer::initGL()
{

    // shader source code

    // the vertex shader simply passes through data
    std::string vertex_source = Shader::loadFile("particlerenderer.vert");


    // the geometry shader creates the billboard quads
    std::string geometry_source = Shader::loadFile("particlerenderer.gs");

    // the fragment shader creates a bell like radial color distribution
    std::string fragment_source = Shader::loadFile("particlerenderer.frag");


    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertexArray = vertex_source.c_str();

    glShaderSource(vertex_shader, 1, &vertexArray, nullptr);
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader))
    {
        exit(1);
    }

    // create and compiler geometry shader
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);

    const char* geometryArray = geometry_source.c_str();
    glShaderSource(geometry_shader, 1, &geometryArray, nullptr);
    glCompileShader(geometry_shader);
    if(!check_shader_compile_status(geometry_shader))
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

    // create program
    shader_program = glCreateProgram();

    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, geometry_shader);
    glAttachShader(shader_program, fragment_shader);

    // link the program and check for errors
    glLinkProgram(shader_program);
    check_program_link_status(shader_program);

    // obtain location of projection uniform
    View_location = glGetUniformLocation(shader_program, "View");
    Projection_location = glGetUniformLocation(shader_program, "Projection");

    // the transform feedback shader only has a vertex shader
    std::string transform_vertex_source = Shader::loadFile("particlerenderer_transform_fire.vert");

    // create and compiler vertex shader
    transform_vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char* transformVertexArray = transform_vertex_source.c_str();

    glShaderSource(transform_vertex_shader, 1, &transformVertexArray, nullptr);
    glCompileShader(transform_vertex_shader);
    if(!check_shader_compile_status(transform_vertex_shader))
    {
        exit(1);
    }

    // create program
    transform_shader_program = glCreateProgram();

    // attach shaders
    glAttachShader(transform_shader_program, transform_vertex_shader);

    // specify transform feedback output
    const char *varyings[] = {"outposition", "outvelocity"};
    glTransformFeedbackVaryings(transform_shader_program, 2, varyings, GL_INTERLEAVED_ATTRIBS);

    // link the program and check for errors
    glLinkProgram(transform_shader_program);
    check_program_link_status(transform_shader_program);

    center_location = glGetUniformLocation(transform_shader_program, "center");
    radius_location = glGetUniformLocation(transform_shader_program, "radius");
    g_location = glGetUniformLocation(transform_shader_program, "g");
    dt_location = glGetUniformLocation(transform_shader_program, "dt");
    bounce_location = glGetUniformLocation(transform_shader_program, "bounce");
    seed_location = glGetUniformLocation(transform_shader_program, "seed");

    // randomly place particles in a cube
    std::vector<glm::vec3> vertexData(2*particles);
    for(int i = 0;i<particles;++i)
    {
        // initial position
        vertexData[2*i+0] = glm::vec3(
                                0.5f-float(std::rand())/RAND_MAX,
                                0.5f-float(std::rand())/RAND_MAX,
                                0.5f-float(std::rand())/RAND_MAX
                            );
        vertexData[2*i+0] = glm::vec3(0.0f,20.0f,0.0f) + 5.0f*vertexData[2*i+0];

        // initial velocity
        vertexData[2*i+1] = glm::vec3(0,0,0);
    }


    glGenVertexArrays(buffercount, vao);
    glGenBuffers(buffercount, vbo);

    for(int i = 0;i<buffercount;++i)
    {
        glBindVertexArray(vao[i]);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);

        // fill with initial data
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

        // set up generic attrib pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));
        // set up generic attrib pointers
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));
    }

    // "unbind" vao
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ParticleRenderer::render()
{

    // we ar blending so no depth testing
    glDisable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    //  and set the blend function to result = 1*source + 1*destination
    glBlendFunc(GL_ONE, GL_ONE);

    // define spheres for the particles to bounce off
    const int spheres = 3;
    glm::vec3 center[spheres];
    float radius[spheres];
    center[0] = glm::vec3(0,12,1);
    radius[0] = 3;
    center[1] = glm::vec3(-3,0,0);
    radius[1] = 7;
    center[2] = glm::vec3(5,-10,0);
    radius[2] = 1;

    // physical parameters
    float dt = 1.0f/60.0f;
    glm::vec3 g(0.0f, -2.81f, 0.0f);
    float bounce = 1.2f; // inelastic: 1.0f, elastic: 2.0f

    int current_buffer=0;

    // get the time in seconds
    float t = SDL_GetTicks();

    // use the transform shader program
    glUseProgram(transform_shader_program);

    // set the uniforms
    glUniform3fv(center_location, 3, reinterpret_cast<GLfloat*>(center));
    glUniform1fv(radius_location, 3, reinterpret_cast<GLfloat*>(radius));
    glUniform3fv(g_location, 1, glm::value_ptr(g));
    glUniform1f(dt_location, dt);
    glUniform1f(bounce_location, bounce);
    glUniform1i(seed_location, std::rand());

    // bind the current vao
    glBindVertexArray(vao[(current_buffer+1)%buffercount]);

    // bind transform feedback target
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[current_buffer]);

    glEnable(GL_RASTERIZER_DISCARD);

    // perform transform feedback
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, particles);
    glEndTransformFeedback();

    glDisable(GL_RASTERIZER_DISCARD);

    // clear first
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the shader program
    glUseProgram(shader_program);

    // calculate ViewProjection matrix
    glm::mat4 Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.f);

    // translate the world/view position
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -30.0f));

    // make the camera rotate around the origin
//        View = glm::rotate(View, 30.0f, glm::vec3(1.0f, 0.0f, 0.0f));
//        View = glm::rotate(View, -22.5f*t, glm::vec3(0.0f, 1.0f, 0.0f));

    // set the uniform
    glUniformMatrix4fv(View_location, 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(Projection_location, 1, GL_FALSE, glm::value_ptr(Projection));

    // bind the current vao
    glBindVertexArray(vao[current_buffer]);

    // draw
    glDrawArrays(GL_POINTS, 0, particles);

    Debug::checkGLError();

    // advance buffer index
    current_buffer = (current_buffer + 1) % buffercount;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
