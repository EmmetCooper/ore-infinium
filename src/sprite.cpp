/******************************************************************************
 *   Copyright (C) 2012, 2013 by Shaun Reich <sreich@kde.org>                 *
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

#include "sprite.h"

#include "src/spatialhash.h"
#include "spritesheetrenderer.h"
#include "game.h"

Sprite::Sprite(const std::string& frameName, SpriteSheetRenderer::SpriteSheetType spriteSheetType) :
    m_spriteSheetType(spriteSheetType)
{
    setFrameName(frameName);
    /*
        const glm::vec2 texSize = size();
        m_origin = glm::vec2(texSize.x() * 0.5, texSize.y() * 0.5);
        FIXME:
    */
}

Sprite::Sprite(const Sprite& entity)
    : m_sizeMeters(entity.m_sizeMeters),
      m_spriteSheetType(entity.spriteSheetType()),
      m_frameName(entity.frameName()),
      m_position(entity.position()),
      m_origin(entity.m_origin)
{
}

Sprite::~Sprite()
{
    m_spatialHash->remove(this);
    //vector clear is handled by spatial hash, just in case they want to remove it but not delete the thingy.
}
