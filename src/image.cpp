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
    qCDebug(ORE_IMAGE_LOADER) << "Loading image:" << filename << "... ";

    struct stat fileAttribute;
    bool fileExists = stat(filename.c_str(), &fileAttribute) == 0;

    if (!fileExists) {
        qFatal("image file failed to load, file does not exist");
    }

    m_image.load(QString::fromStdString(filename));

    if (m_image.isNull()) {
        qFatal("failure to load image");
    }

    if(m_image.format() == QImage::Format_ARGB32) {
        m_format = GL_BGRA;
    } else {
        qFatal("image format is different than what we're used to. format is NOT RGBA");
    }
}

void Image::flipVertically()
{
    Q_ASSERT(!m_image.isNull());
    m_image = m_image.mirrored(false, true);
}

void* Image::bytes()
{
    Q_ASSERT(!m_image.isNull());
    return m_image.bits();
}
