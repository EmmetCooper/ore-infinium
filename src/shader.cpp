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

#include "shader.h"

#include "debug.h"
#include "game.h"
#include "settings/settings.h"

#include "glew.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <memory>
#include <sys/stat.h>


Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
    loadShaders(vertexShader, fragmentShader);

    printShaderInfoLog(m_vertexShader);
    printShaderInfoLog(m_fragmentShader);
}

Shader::~Shader()
{
    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);

    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    glDeleteProgram(m_shaderProgram);
}

void Shader::bindProgram() const
{
    glUseProgram(m_shaderProgram);
}

void Shader::unbindProgram() const
{
    glUseProgram(0);
}

GLuint Shader::shaderProgram() const
{
    return m_shaderProgram;
}

GLuint Shader::vertexShader() const
{
    return m_vertexShader;
}

GLuint Shader::fragmentShader() const
{
    return m_fragmentShader;
}

std::string Shader::loadFile(const char* fileName)
{
    struct stat fileAttribute;
    bool fileExists = stat(fileName, &fileAttribute) == 0;
    Debug::fatal(fileExists, Debug::Area::ImageLoaderArea, "shader file failed to load, file does not exist. Filename: " + std::string(fileName));

    qCDebug(ORE_SHADERS) << "shader:" << fileName << "loaded successfully";

    std::ifstream in(fileName);

    std::stringstream buffer;
    buffer << in.rdbuf();

    std::string contents(buffer.str());

    return contents;
}

void Shader::loadShaders(const char* vertexShader, const char* fragmentShader)
{
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    std::string vertSource = loadFile(vertexShader);

    const char* vertSourceArray = vertSource.c_str();
    glShaderSource(m_vertexShader, 1, &vertSourceArray, nullptr);
    glCompileShader(m_vertexShader);

    if (!checkShaderCompileStatus(m_vertexShader)) {
        qFatal("vertex shader failed to compile properly");
    } else {
        qCDebug(ORE_SHADERS) << "vertex shader compiled!";
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string fragSource = loadFile(fragmentShader);
    const char* fragSourceArray = fragSource.c_str();
    glShaderSource(m_fragmentShader, 1, &fragSourceArray, nullptr);
    glCompileShader(m_fragmentShader);

    if (!checkShaderCompileStatus(m_fragmentShader)) {
        qFatal("fragment shader failed to compile properly");
    } else {
        qCDebug(ORE_SHADERS) << "fragment shader compiled!";
    }

    m_shaderProgram = glCreateProgram();

    // attach shaders
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    glBindFragDataLocation(m_shaderProgram, 0, "fragColor");

    // link the program and check for errors
    glLinkProgram(m_shaderProgram);

    if (checkProgramLinkStatus(m_shaderProgram)) {
        qCDebug(ORE_SHADERS) << "shader program linked!";
    } else {
        qFatal("shader program link FAILURE");
    }

}

bool Shader::checkShaderCompileStatus(GLuint obj)
{
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length;

        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);

        std::unique_ptr<char[]> log(new char[length]);
        glGetShaderInfoLog(obj, length, &length, log.get());

        qCDebug(ORE_SHADERS) << log.get();
        return false;
    }
    return true;
}

bool Shader::checkProgramLinkStatus(GLuint obj)
{
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length;

        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);

        std::unique_ptr<char[]> log(new char[length]);
        glGetProgramInfoLog(obj, length, &length, log.get());

        qCDebug(ORE_SHADERS) << log.get();
        return false;
    }
    return true;
}

void Shader::printShaderInfoLog(GLuint shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        std::unique_ptr<char[]> infoLog(new GLchar[infoLogLen]);
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog.get());

        qCDebug(ORE_SHADERS) << "Shader info log: " << infoLog.get();
    }

    // should additionally check for OpenGL errors here
}
