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

#include "texture.h"
#include "image.h"

#include "debug.h"

Texture::Texture(const QString& fileName)
{
    loadImage(fileName);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
    delete m_image;
}

void Texture::loadImage(const QString& filename)
{
    m_image = new Image(filename);
}

int Texture::format() const {
    Debug::assertf(m_image, "image pointer not existent through texture interface, but format was requested; BUG");
    return m_image->format();
}

void Texture::bind()
{
    Debug::assertf(m_textureID, "bind called before generate on Image");

    Debug::checkGLError();
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    Debug::checkGLError();
}

void Texture::generate(Texture::TextureFilter textureFilter)
{
    Debug::checkGLError();
    glActiveTexture(GL_TEXTURE0);// FIXME: JUNK?

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    switch (textureFilter) {
    case TextureFilterLinear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TextureFilterNearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//    void* bytes = m_image->bytes();
//    Debug::assertf(bytes, "Image::generate, could not gen texture, image bits are empty.");

    m_internal_format = GL_RGBA;
    m_level = 0;
    m_border = 0;
    glTexImage2D(GL_TEXTURE_2D, m_level, m_internal_format, m_image->width(), m_image->height(), m_border, m_image->format(), GL_UNSIGNED_BYTE, m_image->bytes());

    Debug::checkGLError();
}

GLuint Texture::textureHandle() const
{
    return m_textureID;
}

unsigned int Texture::width() const
{
    return m_image->width();
}

unsigned int Texture::height() const
{
    return m_image->height();
}
