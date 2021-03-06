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

#include "camera.h"
#include "globals.h"
#include "shader.h"

#include "glew.h"

#include "settings/settings.h"

#include "debug.h"

Camera::Camera()
{
    float x = 0.0f;
    float y = 0.0f;
    m_viewMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, 0.0f));
    qCDebug(ORE_CLIENT_RENDERER) << "camera init, screen at width: " << Settings::instance()->screenResolutionWidth << " height: " << Settings::instance()->screenResolutionHeight;
//    m_orthoMatrix = glm::ortho(0.0f, float(Settings::instance()->screenResolutionWidth), float(Settings::instance()->screenResolutionHeight), 0.0f, -1.0f, 1.0f);

    float width = static_cast<float>(Settings::instance()->screenResolutionWidth);
    float height = static_cast<float>(Settings::instance()->screenResolutionHeight);

    m_orthoMatrix = glm::ortho(0.0f, float(width / PIXELS_PER_METER), 0.0f, float(height / PIXELS_PER_METER), -1.0f, 1.0f);
}

//FIXME: unused
void Camera::translate(const glm::vec2 vec)
{
    m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(vec, 0.0f));
    pushMatrix();
}

void Camera::zoom(const float factor)
{
    m_scaleFactor *= factor;

    pushMatrix();
}

void Camera::centerOn(const glm::vec2 vec)
{
    glm::vec2 position = glm::vec2((vec.x), (vec.y));

    glm::vec2 halfScreen((Settings::instance()->screenResolutionWidth) / PIXELS_PER_METER / 2.0f, (Settings::instance()->screenResolutionHeight / PIXELS_PER_METER) / 2.0f);

    m_viewMatrix =
        glm::translate(glm::mat4(), glm::vec3(halfScreen, 0.0)) *
        glm::scale(glm::mat4(), glm::vec3(m_scaleFactor)) *
        glm::translate(glm::mat4(), -glm::vec3(position, 0.0f));

    pushMatrix();
}

//FIXME: unused
void Camera::setPosition(const glm::vec2 vec)
{
    glm::vec2 halfScreen((Settings::instance()->screenResolutionWidth) / PIXELS_PER_METER / 2.0f, (Settings::instance()->screenResolutionHeight / PIXELS_PER_METER) / 2.0f);
    m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(halfScreen, 0.0));
    m_viewMatrix = glm::scale(m_viewMatrix, glm::vec3(m_scaleFactor));
    m_viewMatrix = glm::translate(glm::mat4(), -glm::vec3(vec, 0.0f));
    pushMatrix();
}

void Camera::addShader(Shader* shader)
{
    assert(shader);
    m_shaders.push_back(shader);
    pushMatrix();
}

void Camera::pushMatrix()
{
    Debug::assertf(m_shaders.size() > 0, "no shader to push the camera matrix to. This is INVALID");

    for (auto * shader : m_shaders) {
        shader->bindProgram();

        glm::mat4 mvp =  m_orthoMatrix * m_viewMatrix;

        int mvpLoc = glGetUniformLocation(shader->shaderProgram(), "mvp");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

        shader->unbindProgram();
    }
}

void Camera::setOrtho(const glm::mat4& ortho)
{
    m_orthoMatrix = ortho;
    pushMatrix();
}

void Camera::setView(const glm::mat4& view)
{
    m_viewMatrix = view;
    pushMatrix();
}
