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

#include <sys/stat.h>

Image::Image(const std::string& fileName)
{
    loadImage(fileName);
    flipVertically();
}

Image::~Image()
{
}

uint32_t Image::width() const
{
    return m_image.width();
}

uint32_t Image::height() const
{
    return m_image.height();
}

void Image::loadImage(const std::string& filename)
{
    Debug::log(Debug::Area::ImageLoaderArea) << "Loading image: " << filename << " ... ";

    struct stat fileAttribute;
    bool fileExists = stat(filename.c_str(), &fileAttribute) == 0;

    Debug::fatal(fileExists, Debug::Area::ImageLoaderArea, "image file failed to load, file does not exist. Filename: " + filename);

    m_image.load(QString::fromStdString(filename));

    Debug::fatal(!m_image.isNull(), Debug::Area::ImageLoaderArea, "failure to load image");

    if(m_image.format() == QImage::Format_ARGB32) {
        m_format = GL_BGRA;
    } else {
        Debug::fatal(false, Debug::ImageLoaderArea, "image format is different than what we're used to. format is NOT RGBA");
    }
}

void Image::flipVertically()
{
    Debug::fatal(!m_image.isNull(), Debug::Area::ImageLoaderArea, "bitmap invalid!");
    m_image = m_image.mirrored(false, true);
}

void* Image::bytes()
{
    Debug::fatal(!m_image.isNull(), Debug::Area::ImageLoaderArea, "bitmap invalid!");
    return m_image.bits();
}
