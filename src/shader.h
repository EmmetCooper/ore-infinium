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

#ifndef SHADER_H
#define SHADER_H

#include "glew.h"

class QString;

/**
* Represents the vertex, fragment shader handles,
* as well as the shader program handle itself.
*
* Also has built in logging for these and binding functions.
*/
class Shader
{
public:
    explicit Shader(const char* vertexShader, const char* fragmentShader);
    ~Shader();

    GLuint shaderProgram() const;
    GLuint vertexShader() const;
    GLuint fragmentShader() const;

    void bindProgram() const;
    void unbindProgram() const;

    static QString loadFile(const char* fileName);
private:

    void loadShaders(const char* vertexShader, const char* fragmentShader);
    void printShaderInfoLog(GLuint shader);
    bool checkShaderCompileStatus(GLuint obj);
    bool checkProgramLinkStatus(GLuint obj);

    GLuint m_shaderProgram = 0;
    GLuint m_vertexShader = 0;
    GLuint m_fragmentShader = 0;
};

#endif
