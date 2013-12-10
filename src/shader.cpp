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

#include <QtCore/QString>
#include <QtCore/QFile>

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

QString Shader::loadFile(const char* fileName)
{
    qCDebug(ORE_SHADERS) << "loading shader:" << fileName;

    bool fileExists = QFile::exists(fileName);

    if (!fileExists) {
        qFatal("shader does not exist!");
    }

    qCDebug(ORE_SHADERS) << "shader:" << fileName << "loaded successfully";

    std::ifstream in(fileName);

    std::stringstream buffer;
    buffer << in.rdbuf();

    QString contents(QString::fromStdString(std::string(buffer.str())));
    qCDebug(ORE_SHADERS) << "SHADER: " << contents;

    return contents;
}

void Shader::loadShaders(const char* vertexShader, const char* fragmentShader)
{
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const QString vertSource = loadFile(vertexShader);

    const char* vertSourceArray = vertSource.toLatin1();
    glShaderSource(m_vertexShader, 1, &vertSourceArray, nullptr);
    glCompileShader(m_vertexShader);

    if (!checkShaderCompileStatus(m_vertexShader)) {
        qFatal("vertex shader failed to compile properly");
    } else {
        qCDebug(ORE_SHADERS) << "vertex shader compiled!";
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    QString fragSource = loadFile(fragmentShader);

    const char* fragSourceArray = fragSource.toLatin1();
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
