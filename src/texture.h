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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "glew.h"

#include <cstdint>

#include <QString>

class Image;

class Texture
{
public:
    explicit Texture(const QString& fileName);
    ~Texture();

    uint32_t width() const;
    uint32_t height() const;

    enum TextureFilter {
        TextureFilterLinear,
        TextureFilterNearest
    };

    int format() const;

    void bind();
    void generate(TextureFilter textureFilter);
    GLuint textureHandle() const;

private:
    void loadImage(const QString& fileName);

private:
    Image* m_image = nullptr;

    GLuint m_textureID = 0;
    GLenum m_image_format = 0;
    GLint m_internal_format = 0;
    GLint m_level = 0;
    GLint m_border = 0;
};

#endif
