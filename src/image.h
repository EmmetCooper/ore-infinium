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

#ifndef IMAGE_H
#define IMAGE_H

#include "glew.h"

#include <string>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_main.h>

struct SDL_Surface;

class Image
{
public:
    explicit Image(const std::string& fileName);
    ~Image();

    uint32_t width() const;
    uint32_t height() const;

    void flipVertically();

    void* bytes();

    int format() const {
        return m_format;
    }

private:

    /**
     * Load an image as an OpenGL texture and make it the current texture
     * If @p texID is already in use, it will be unloaded and replaced with this texture
     *
     * @p filename path relative or absolute including extension to the image to load
     * @p texID the GLint handle to the resulting texture
     * @p image_format which format the image on disk is in. Default is GL_BGRA
     * @p internal_format the format to tell OpenGL how it should store it. Default GL_RGBA
     * @p level what mipmapping level to utilize. Default 0.
     * @p border border size. Default 0
     */
    void loadImage(const std::string& filename);

    int invert_surface_vertical(SDL_Surface *surface);

    SDL_Surface* m_surface = nullptr;

    int m_format = GL_RGBA;
};

#endif
