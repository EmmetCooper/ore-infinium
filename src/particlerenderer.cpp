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

//HACK, remove me obviously..
#define PI M_PI
#define TWOPI (M_PI * 2.0)

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


//    // specify transform feedback output
//    const char *varyings[] = {"outposition", "outvelocity"};
//FIXME //    glTransformFeedbackVaryings(transform_shader_program, 2, varyings, GL_INTERLEAVED_ATTRIBS);
//

    const char * outputNames[] = { "Position", "Velocity", "StartTime" };
    glTransformFeedbackVaryings(shader_program, 3, outputNames, GL_SEPARATE_ATTRIBS);

    // link the program and check for errors
    glLinkProgram(shader_program);
    check_program_link_status(shader_program);

    Debug::checkGLError();

    /// FIXME: HACK: ///////////////

    Debug::checkGLError();
    // Generate the buffers
    glGenBuffers(2, posBuf);    // position buffers
    glGenBuffers(2, velBuf);    // velocity buffers
    glGenBuffers(2, startTime); // Start time buffers
    glGenBuffers(1, &initVel);  // Initial velocity buffer (never changes, only need one)

    Debug::checkGLError();
    // Allocate space for all buffers
    int size = nParticles * 3 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);

    Debug::checkGLError();
    // Fill the first position buffer with zeroes
    GLfloat *data = new GLfloat[nParticles * 3];
    for( int i = 0; i < nParticles * 3; i += 3 ) {
        data[i] = 0.0f;
        data[i+1] = 0.0f;
        data[i+2] = 0.0f;
    }

    Debug::checkGLError();
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

    // Fill the first velocity buffer with random velocities
    float theta, phi, velocity;
    glm::vec3 v(0.0f);
    for( int i = 0; i < nParticles; i++ ) {
        theta = glm::mix(0.0f, (float)PI / 1.5f, randFloat());
        phi = glm::mix(0.0f, (float)TWOPI, randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        velocity = glm::mix(0.1f,0.2f,randFloat());
        v = glm::normalize(v) * velocity;

        data[3*i]   = v.x;
        data[3*i+1] = v.y;
        data[3*i+2] = v.z;
    }
    glBindBuffer(GL_ARRAY_BUFFER,velBuf[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    glBindBuffer(GL_ARRAY_BUFFER,initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

    // Fill the first start time buffer
    delete [] data;

    data = new GLfloat[nParticles];
    float time = 0.0f;
    float rate = 0.01f;
    for( int i = 0; i < nParticles; i++ ) {
        data[i] = time;
        time += rate;
    }
    glBindBuffer(GL_ARRAY_BUFFER,startTime[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    delete [] data;

    Debug::checkGLError();
    // Create vertex arrays for each set of buffers
    glGenVertexArrays(2, particleArray);

    // Set up particle array 0
    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);

    // Set up particle array 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    Debug::checkGLError();
    glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    // Setup the feedback objects
    glGenTransformFeedbacks(2, feedback);

    // Transform feedback 0
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[0]);

    // Transform feedback 1
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Debug::checkGLError();
//    projection = glm::mat4(1.0f);

    angle = (float)( PI / 2.0f );
  model = glm::mat4(1.0f);

    Debug::checkGLError();

    m_smokeTexture = new Texture("../textures/smoke.png");
    m_smokeTexture->generate(Texture::TextureFilterNearest);

    Debug::checkGLError();
   // setMatrices();
}

void ParticleRenderer::render()
{
//    float t = SDL_GetTicks() / 1000.0;

    static float t = 0.0f;
    t += 1.0/60.0;
    deltaT = t - time;
    time = t;
    Debug::log(Debug::ImportantArea) << t;
//    deltaT = t - time;
//    time = t;

    Debug::checkGLError();

    glUseProgram(shader_program);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint lifetimeLoc = glGetUniformLocation(shader_program, "ParticleLifetime");
    glUniform1f(lifetimeLoc, 6.0f);


    GLint accelLoc = glGetUniformLocation(shader_program, "Accel");
    glUniform3f(accelLoc, -0.1f, 0.1f, 0.0f);

    glActiveTexture(GL_TEXTURE0);

    m_smokeTexture->bind();

    GLint texLoc = glGetUniformLocation(shader_program, "ParticleTex");
    glUniform1i(texLoc, 0);

    Debug::checkGLError();
    // Update pass
    GLint passTypeLoc = glGetUniformLocation(shader_program, "passType");

    glUniform1i(passTypeLoc, 0);

    GLint timeLoc = glGetUniformLocation(shader_program, "Time");
    glUniform1f(timeLoc, time);

    GLint deltaTLoc = glGetUniformLocation(shader_program, "H");
    glUniform1f(deltaTLoc, deltaT);

    glEnable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);

    Debug::checkGLError();
    glBeginTransformFeedback(GL_POINTS);
    glBindVertexArray(particleArray[1-drawBuf]);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glEndTransformFeedback();

    glDisable(GL_RASTERIZER_DISCARD);

    // Render pass
    glUniform1i(passTypeLoc, 1);
//    glClear( GL_COLOR_BUFFER_BIT );
    view = glm::lookAt(glm::vec3(3.0f * cosf(angle),1.5f,3.0f * sinf(angle)), glm::vec3(0.0f,1.5f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
    float w = 1600;
    float h = 900;
    projection = glm::perspective(60.0f, (float)w/h, 0.3f, 100.0f);
//    view = glm::mat4(1.0);

    pushMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(particleArray[drawBuf]);

    glDrawArrays(GL_POINTS, 0, nParticles);

    Debug::checkGLError();
    // Swap buffers
    drawBuf = 1 - drawBuf;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    Debug::checkGLError();

    glActiveTexture(GL_TEXTURE0);
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

/// ////////////////////////////////////////////////////////////////////////////////////////////

//void SceneSmoke::update( float t )
//{
//
//}
//
//void SceneSmoke::compileAndLinkShader()
//{
//    if( ! prog.compileShaderFromFile("shader/smoke.vs",GLSLShader::VERTEX) )
//    {
//        printf("Vertex shader failed to compile!\n%s",
//               prog.log().c_str());
//        exit(1);
//    }
//    if( ! prog.compileShaderFromFile("shader/smoke.fs",GLSLShader::FRAGMENT))
//    {
//        printf("Fragment shader failed to compile!\n%s",
//               prog.log().c_str());
//        exit(1);
//    }
//
//    //////////////////////////////////////////////////////
//    // Setup the transform feedback (must be done before linking the program)
//    GLuint progHandle = prog.getHandle();
//    const char * outputNames[] = { "Position", "Velocity", "StartTime" };
//    glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);
//    ///////////////////////////////////////////////////////
//
//    if( ! prog.link() )
//    {
//        printf("Shader program failed to link!\n%s",
//               prog.log().c_str());
//        exit(1);
//    }
//
//    prog.use();
//}
//
