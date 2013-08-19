/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (C) 2013 Shaun Reich <sreich@kde.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "ShellRenderInterfaceOpenGL.h"

#include "src/image.h"
#include "src/debug.h"
#include "src/shader.h"
#include <src/texture.h>

#include <Rocket/Core.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

ShellRenderInterfaceOpenGL::ShellRenderInterfaceOpenGL()
{
    Debug::checkGLError();

    m_shader = new Shader("guirenderer.vert", "guirenderer.frag");
    m_shader->bindProgram();

    Debug::checkGLError();
    initGL();

    Debug::checkGLError();
}

ShellRenderInterfaceOpenGL::~ShellRenderInterfaceOpenGL()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);

    glDeleteTextures(1, &m_whiteTexture);

    delete m_shader;
}

void ShellRenderInterfaceOpenGL::initGL()
{
    Debug::checkGLError();
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_ebo);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_shader->bindProgram();
    // vertices that will be uploaded.
    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Rocket::Core::Vertex),
        (const GLvoid*)buffer_offset
    );

    buffer_offset += sizeof(float) * 2;

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Rocket::Core::Vertex),
        (const GLvoid*)buffer_offset
    );
    buffer_offset += sizeof(uint32_t);

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Rocket::Core::Vertex),
        (const GLvoid*)buffer_offset
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    uint8_t red = 255;
    uint8_t green = 255;
    uint8_t blue = 255;
    uint8_t alpha = 255;
    int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);

    GLubyte imageData = color;

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &m_whiteTexture);
    glBindTexture(GL_TEXTURE_2D, m_whiteTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_BGRA, GL_UNSIGNED_BYTE, &imageData);
    Debug::checkGLError();
}

void ShellRenderInterfaceOpenGL::SetViewport(int width, int height)
{
    m_width = width;
    m_height = height;

    m_ortho = glm::ortho(0.0f, float(m_width), float(m_height), 0.0f, -1.0f, 1.0f);
}

// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void ShellRenderInterfaceOpenGL::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
    Debug::checkGLError();
    glActiveTexture(GL_TEXTURE0);

    if (!texture) {
        glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
    } else {
        glBindTexture(GL_TEXTURE_2D, GLuint(texture));
    }

    m_shader->bindProgram();

    Debug::checkGLError();
    glm::mat4 view =  glm::translate(glm::mat4(), glm::vec3(translation.x, translation.y, 0.0f));

    glm::mat4 mvp =  m_ortho * view;

    int mvpLoc = glGetUniformLocation(m_shader->shaderProgram(), "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    Debug::checkGLError();
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    Debug::checkGLError();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        num_indices * sizeof(indices),
        indices,
        GL_DYNAMIC_DRAW
    );

    Debug::checkGLError();
    // finally upload everything to the actual vbo
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Rocket::Core::Vertex) * num_vertices,
        vertices,
        GL_DYNAMIC_DRAW
    );

    Debug::checkGLError();
    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Debug::checkGLError();

    m_shader->bindProgram();

    glDrawElements(
        GL_TRIANGLES,
        num_indices,
        GL_UNSIGNED_INT,
        (const GLvoid*)0
    );

    m_shader->unbindProgram();
    Debug::checkGLError();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Debug::checkGLError();

    glDisable(GL_BLEND);
    Debug::checkGLError();
}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
Rocket::Core::CompiledGeometryHandle ShellRenderInterfaceOpenGL::CompileGeometry(Rocket::Core::Vertex* ROCKET_UNUSED(vertices), int ROCKET_UNUSED(num_vertices), int* ROCKET_UNUSED(indices), int ROCKET_UNUSED(num_indices), const Rocket::Core::TextureHandle ROCKET_UNUSED(texture))
{
    return (Rocket::Core::CompiledGeometryHandle) NULL;
}

// Called by Rocket when it wants to render application-compiled geometry.
void ShellRenderInterfaceOpenGL::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry), const Rocket::Core::Vector2f& ROCKET_UNUSED(translation))
{
}

// Called by Rocket when it wants to release application-compiled geometry.
void ShellRenderInterfaceOpenGL::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry))
{
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.
void ShellRenderInterfaceOpenGL::EnableScissorRegion(bool enable)
{
    return;
    if (enable) {
        glEnable(GL_SCISSOR_TEST);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

// Called by Rocket when it wants to change the scissor region.
void ShellRenderInterfaceOpenGL::SetScissorRegion(int x, int y, int width, int height)
{
    glScissor(x, m_height - (y + height), width, height);
}

// Called by Rocket when a texture is required by the library.
bool ShellRenderInterfaceOpenGL::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
    Debug::log(Debug::Area::GUILoggerArea) << "Rocket OpenGL Renderer, loading texture name: " << source.CString();

    Debug::checkGLError();
    Image* image = new Image(source.CString());
    image->flipVertically();

    texture_dimensions.x = image->width();
    texture_dimensions.y = image->height();

    bool success = GenerateTexture(texture_handle, static_cast<Rocket::Core::byte*>(image->bytes()), texture_dimensions);
    Debug::checkGLError();

    return success;
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool ShellRenderInterfaceOpenGL::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
    Debug::checkGLError();
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    if (texture_id == 0) {
        Debug::log(Debug::Area::GUILoggerArea) << "Failed to generate textures\n";
        return false;
    }

    Debug::checkGLError();

    glBindTexture(GL_TEXTURE_2D, texture_id);

    Debug::checkGLError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    Debug::checkGLError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Debug::checkGLError();

    //FIXME: on my laptop, mesa 9.1, Gallium 0.4 on AMD RS880, invalid enumerant? odd shit.
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    Debug::checkGLError();

    GLenum image_format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, image_format, GL_UNSIGNED_BYTE, (void*)(source));

    Debug::checkGLError();

    texture_handle = (Rocket::Core::TextureHandle) texture_id;

    Debug::checkGLError();

    return true;
}

// Called by Rocket when a loaded texture is no longer required.
void ShellRenderInterfaceOpenGL::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    glDeleteTextures(1, (GLuint*) &texture_handle);
    Debug::checkGLError();
}
