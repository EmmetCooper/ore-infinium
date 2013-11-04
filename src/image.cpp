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

#include "image.h"

#include "debug.h"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>

#include <sys/stat.h>

Image::Image(const std::string& fileName)
{
    loadImage(fileName);
    invert_surface_vertical(m_surface);
}

Image::~Image()
{
  SDL_FreeSurface(m_surface);
}

uint32_t Image::width() const
{
    return m_surface->w;
}

uint32_t Image::height() const
{
    return m_surface->h;
}

void Image::loadImage(const std::string& filename)
{
    Debug::log(Debug::Area::ImageLoaderArea) << "Loading image: " << filename << " ... ";

    struct stat fileAttribute;
    bool fileExists = stat(filename.c_str(), &fileAttribute) == 0;

    Debug::fatal(fileExists, Debug::Area::ImageLoaderArea, "image file failed to load, file does not exist. Filename: " + filename);

    m_surface = IMG_Load(filename.c_str());

    Debug::fatal(m_surface, Debug::Area::ImageLoaderArea, "failure to load image, bitmap pointer invalid");
    Debug::fatal(m_surface->w > 0, Debug::Area::ImageLoaderArea, "failure to load image, width is 0");
    Debug::fatal(m_surface->h > 0, Debug::Area::ImageLoaderArea, "failure to load image, height is 0");

    if(m_surface->format->BytesPerPixel == 4) {
        m_format = GL_RGBA;
    } else {
        Debug::fatal(false, Debug::ImageLoaderArea, "image format is different than what we're used to. format is NOT RGBA");
    }
}

void Image::flipVertically()
{
    invert_surface_vertical(m_surface);
}

#define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
#define SDL_UNLOCKIFMUST(s) { if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s); }

int Image::invert_surface_vertical(SDL_Surface *surface)
{
    Uint8 *t;
    register Uint8 *a, *b;
    Uint8 *last;
    register Uint16 pitch;

    if( SDL_LOCKIFMUST(surface) < 0 )
        return -2;

    /* do nothing unless at least two lines */
    if(surface->h < 2) {
        SDL_UNLOCKIFMUST(surface);
        return 0;
    }

    /* get a place to store a line */
    pitch = surface->pitch;
    t = (Uint8*)malloc(pitch);

    if(t == NULL) {
        SDL_UNLOCKIFMUST(surface);
        return -2;
    }

    /* get first line; it's about to be trampled */
    memcpy(t,surface->pixels,pitch);

    /* now, shuffle the rest so it's almost correct */
    a = (Uint8*)surface->pixels;
    last = a + pitch * (surface->h - 1);
    b = last;

    while(a < b) {
        memcpy(a,b,pitch);
        a += pitch;
        memcpy(b,a,pitch);
        b -= pitch;
    }

    /* in this shuffled state, the bottom slice is too far down */
    memmove( b, b+pitch, last-b );

    /* now we can put back that first row--in the last place */
    memcpy(last,t,pitch);

    /* everything is in the right place; close up. */
    free(t);
    SDL_UNLOCKIFMUST(surface);

    return 0;
}

void* Image::bytes()
{
    Debug::fatal(m_surface, Debug::Area::ImageLoaderArea, "bitmap invalid!");
    return m_surface->pixels;
}
