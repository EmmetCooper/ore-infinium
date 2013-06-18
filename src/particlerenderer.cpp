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

#include <SDL_timer.h>

#include <fstream>

#include <glm/core/func_common.hpp>
#include <glm/gtc/type_ptr.hpp>

ParticleRenderer::ParticleRenderer(World* world, Camera* camera, Entities::Player* mainPlayer)
{

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
    std::string vertex_source =
        "#version 330\n"
        "layout(location = 0) in vec4 vposition;\n"
        "void main() {\n"
        "   gl_Position = vposition;\n"
        "}\n";

    // the geometry shader creates the billboard quads
    std::string geometry_source =
        "#version 330\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "layout (points) in;\n"
        "layout (triangle_strip, max_vertices = 4) out;\n"
        "out vec2 txcoord;\n"
        "void main() {\n"
        "   vec4 pos = View*gl_in[0].gl_Position;\n"
        "   txcoord = vec2(-1,-1);\n"
        "   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
        "   EmitVertex();\n"
        "   txcoord = vec2( 1,-1);\n"
        "   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
        "   EmitVertex();\n"
        "   txcoord = vec2(-1, 1);\n"
        "   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
        "   EmitVertex();\n"
        "   txcoord = vec2( 1, 1);\n"
        "   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
        "   EmitVertex();\n"
        "}\n";

    // the fragment shader creates a bell like radial color distribution
    std::string fragment_source =
        "#version 330\n"
        "in vec2 txcoord;\n"
        "layout(location = 0) out vec4 FragColor;\n"
        "void main() {\n"
        "   float s = 0.2*(1/(1+15.*dot(txcoord, txcoord))-1/16.);\n"
        "   FragColor = s*vec4(0.9,0.5,0.0,1);\n"
        "}\n";

    // we need these to properly pass the strings
    const char *source;
    int length;

    // create and compiler vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertex_source.c_str();
    length = vertex_source.size();
    glShaderSource(vertex_shader, 1, &source, &length);
    glCompileShader(vertex_shader);
    if(!check_shader_compile_status(vertex_shader))
    {
        exit(1);
    }

    // create and compiler geometry shader
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    source = geometry_source.c_str();
    length = geometry_source.size();
    glShaderSource(geometry_shader, 1, &source, &length);
    glCompileShader(geometry_shader);
    if(!check_shader_compile_status(geometry_shader))
    {
        exit(1);
    }

    // create and compiler fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragment_source.c_str();
    length = fragment_source.size();
    glShaderSource(fragment_shader, 1, &source, &length);
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
    std::string transform_vertex_source =
        "#version 330\n"
        "uniform vec3 center[3];\n"
        "uniform float radius[3];\n"
        "uniform vec3 g;\n"
        "uniform float dt;\n"
        "uniform float bounce;\n"
        "uniform int seed;\n"
        "layout(location = 0) in vec3 inposition;\n"
        "layout(location = 1) in vec3 invelocity;\n"
        "out vec3 outposition;\n"
        "out vec3 outvelocity;\n"

        "float hash(int x) {\n"
        "   x = x*12 + gl_VertexID*1 + seed*1;\n"
        "   x = (x >> 13) ^ x;\n"
        "   return ((x * (x * x * 2 + 19990303) + 1) & 0x7fffffff)/float(0x7fffffff-1);\n"
        "}\n"

        "void main() {\n"
        "   outvelocity = invelocity;\n"
        "   for(int j = 0;j<1;++j) {\n"
        "       vec3 diff = inposition-center[j];\n"
        "       float dist = length(diff);\n"
        "       float vdot = dot(diff, invelocity);\n"
        "       if(dist<radius[j] && vdot<0.0)\n"
        "           outvelocity -= bounce*diff*vdot/(dist*dist);\n"
        "   }\n"
        "   outvelocity += dt*g;\n"
        "   outposition = inposition + dt*outvelocity;\n"
        "   if(outposition.y < -10.0)\n"
        "   {\n"
        "   }\n"
        "}\n";

    // create and compiler vertex shader
    transform_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = transform_vertex_source.c_str();
    length = transform_vertex_source.size();

    glShaderSource(transform_vertex_shader, 1, &source, &length);
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

}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
char* ParticleRenderer::loadFile(const char* fname, GLint* fSize)
{
    std::ifstream::pos_type size;
    char * memblock = 0;
    std::string text;

    std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        *fSize = (GLuint) size;

        memblock = new char [size];
        file.seekg(0, std::ios::beg);
        file.read(memblock, size);
        file.close();
        text.assign(memblock);

        Debug::log(Debug::Area::ShadersArea) << "shader : " << fname << " loaded successfully";
    } else {
        Debug::fatal(false,  Debug::Area::ShadersArea, "failed to load shader: " + std::string(fname));
    }
    return memblock;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
