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

#include "spritesheetrenderer.h"

#include "sprite.h"
#include "entity.h"
#include "debug.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "settings/settings.h"
#include "globals.h"

#include <QtCore/QFile>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <stdio.h>
#include <stdlib.h>

SpriteSheetRenderer::SpriteSheetRenderer(Camera* camera)
{
    Debug::checkGLError();

    m_shader = new Shader("sprite.vert", "sprite.frag");
    setCamera(camera);

    Debug::checkGLError();

    initGL();

    Debug::checkGLError();

    loadAllSpriteSheets();
    Debug::checkGLError();
    parseAllSpriteSheets();
    Debug::checkGLError();
}

SpriteSheetRenderer::~SpriteSheetRenderer()
{
    unloadAllSpriteSheets();

    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vboEntities);
    glDeleteBuffers(1, &m_eboEntities);

    glDeleteVertexArrays(1, &m_vao);
    glDeleteVertexArrays(1, &m_vaoEntities);

    delete m_spriteSheetTextures[SpriteSheetRenderer::SpriteSheetType::Character].texture;
    delete m_spriteSheetTextures[SpriteSheetRenderer::SpriteSheetType::Entity].texture;

    delete m_shader;
}

void SpriteSheetRenderer::setCamera(Camera* camera)
{
    m_camera = camera;
    m_camera->addShader(m_shader);
}

void SpriteSheetRenderer::loadAllSpriteSheets()
{
    loadSpriteSheet("../textures/characters.png", SpriteSheetType::Character);
    loadSpriteSheet("../textures/entities.png", SpriteSheetType::Entity);
}

void SpriteSheetRenderer::loadSpriteSheet(const QString& fileName, SpriteSheetRenderer::SpriteSheetType type)
{
    auto& wrapper = m_spriteSheetTextures[type];
    wrapper.texture = new Texture(fileName);
    wrapper.texture->generate(Texture::TextureFilterLinear);
}

void SpriteSheetRenderer::unloadSpriteSheet(SpriteSheetRenderer::SpriteSheetType type)
{
    auto it = m_spriteSheetTextures.find(type);
    delete it.value().texture;
    m_spriteSheetTextures.erase(it);
}

void SpriteSheetRenderer::unloadAllSpriteSheets()
{
    QMap<SpriteSheetRenderer::SpriteSheetType, SpriteSheet>::const_iterator i = m_spriteSheetTextures.begin();

    while (i != m_spriteSheetTextures.end()) {
        unloadSpriteSheet(i.key());
        ++i;
    }

    m_spriteSheetTextures.clear();
}

void SpriteSheetRenderer::bindSpriteSheet(SpriteSheetRenderer::SpriteSheetType type)
{
    m_spriteSheetTextures[type].texture->bind();
}

glm::vec2 SpriteSheetRenderer::spriteSheetSize(SpriteSheetRenderer::SpriteSheetType type)
{
    auto texture = m_spriteSheetTextures.find(type);
    glm::vec2 imageSize(float(texture.value().texture->width()), float(texture.value().texture->height()));

    return imageSize;
}

SpriteSheetRenderer::SpriteFrameIdentifier SpriteSheetRenderer::spriteFrame(const QString& frameName)
{
    return m_spriteSheetEntitiesDescription.find(frameName).value();
}

void SpriteSheetRenderer::registerSprite(Sprite* sprite)
{
    switch (sprite->spriteSheetType()) {
    case SpriteSheetType::Character: {
        m_characterSprites.append(sprite);
        // TODO: look up the size of the graphic/frame, in the spritesheet map.

        auto frameIdentifier = m_spriteSheetCharactersDescription.find(sprite->frameName());
        SpriteFrameIdentifier& frame = frameIdentifier.value();
        sprite->m_sizeMeters = glm::vec2(frame.width / PIXELS_PER_METER, frame.height / PIXELS_PER_METER);

        //NOTE: terraria's player size is (blocksize*2, blocksize*3), and that's a great default. sprite->m_size = glm::vec2(Block::blockSize * 2, Block::blockSize * 3);
        qCDebug(ORE_SPRITE_SHEET_RENDERER) << "character sprite registered, setting default size, which is that of source texture: width: " << frame.width << " height: " << frame.height << " new sprite count: " << m_entitySprites.size();
        break;
    }

    case SpriteSheetType::Entity: {
        m_entitySprites.append(sprite);

        auto frameIdentifier = m_spriteSheetEntitiesDescription.find(sprite->frameName());
        SpriteFrameIdentifier& frame = frameIdentifier.value();
        sprite->m_sizeMeters = glm::vec2(frame.width / PIXELS_PER_METER, frame.height / PIXELS_PER_METER);

        qCDebug(ORE_SPRITE_SHEET_RENDERER) << "entity sprite registered, setting default size, which is that of source texture: width: " << frame.width << " height: " << frame.height << " new sprite count: " << m_entitySprites.size();
        break;
    }
    }
}

void SpriteSheetRenderer::parseAllSpriteSheets()
{
    m_spriteSheetCharactersDescription = parseSpriteSheet("../textures/characters.yaml");
    m_spriteSheetEntitiesDescription = parseSpriteSheet("../textures/entities.yaml");
}

void operator >> (const YAML::Node& node, SpriteSheetRenderer::SpriteFrameIdentifier& frame)
{
    std::string str; //FIXME: HACK for QSTRING AND YAML. get rid of yaml and use something in qt, less deps == more win
    node["frameName"] >> str;
    frame.frameName = QString::fromStdString(str);

    node["x"] >> frame.x;
    node["y"] >> frame.y;
    node["width"] >> frame.width;
    node["height"] >> frame.height;
}

QMap<QString, SpriteSheetRenderer::SpriteFrameIdentifier> SpriteSheetRenderer::parseSpriteSheet(const QString& filename)
{
    QMap<QString, SpriteFrameIdentifier> descriptionMap;

    bool fileExists = QFile::exists(filename);

    //qCDebug(ORE_SPRITE_SHEET_RENDERER) << "parsing: '" << filename.toStdString() << "' spreadsheet description...";
    //Debug::fatal(fileExists, Debug::Area::SpriteSheetRendererArea, "sprite sheet description file failed to load, filename: " + filename);

    std::ifstream fin(filename.toStdString());
    YAML::Parser parser(fin);

    YAML::Node doc;
    parser.GetNextDocument(doc);

    for (size_t i = 0; i < doc.size(); i++) {
        SpriteFrameIdentifier frame;
        doc[i] >> frame;

        //qCDebug(ORE_SPRITE_SHEET_RENDERER) << "frameName: " << frame.frameName;
        qCDebug(ORE_SPRITE_SHEET_RENDERER) << "x: " << frame.x;
        qCDebug(ORE_SPRITE_SHEET_RENDERER) << "y: " << frame.y;
        qCDebug(ORE_SPRITE_SHEET_RENDERER) << "width: " << frame.width;
        qCDebug(ORE_SPRITE_SHEET_RENDERER) << "height: " << frame.height;

        descriptionMap[frame.frameName] = frame;
    }

    return descriptionMap;
}

void SpriteSheetRenderer::renderCharacters()
{
    m_shader->bindProgram();

    bindSpriteSheet(SpriteSheetType::Character);

    Debug::checkGLError();

    int index = 0;

    for (Sprite * sprite : m_characterSprites) {
        auto frameIdentifier = m_spriteSheetCharactersDescription.find(sprite->frameName());
        //Debug::fatal(frameIdentifier != m_spriteSheetCharactersDescription.end(), Debug::Area::SpriteSheetRendererArea, "sprite sheet character frame description could not be located, name: " + sprite->frameName());

        SpriteFrameIdentifier& frame = frameIdentifier.value();

        // vertices that will be uploaded.
        Vertex vertices[4];

        // vertices[n][0] -> X, and [1] -> Y
        // vertices[0] -> top left
        // vertices[1] -> bottom left
        // vertices[2] -> bottom right
        // vertices[3] -> top right

        glm::vec2 spritePosition = sprite->position();

        glm::vec2 spriteSize = sprite->sizeMeters();

        glm::vec4 rect = glm::vec4(spritePosition.x - (spriteSize.x * 0.5f), spritePosition.y - (spriteSize.x * 0.5f), spritePosition.x + (spriteSize.x * 0.5f), spritePosition.y + (spriteSize.y * 0.5f));

        float x = rect.x;
        float width = rect.z;
        float y = rect.y;
        float height = rect.w;

        vertices[0].position.x = x; // top left X
        vertices[0].position.y = y; //top left Y

        vertices[1].position.x = x; // bottom left X
        vertices[1].position.y = height; // bottom left Y

        vertices[2].position.x = width; // bottom right X
        vertices[2].position.y = height; //bottom right Y

        vertices[3].position.x = width; // top right X
        vertices[3].position.y = y; // top right Y

        Debug::checkGLError();
        // copy color to the buffer
        for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
            //        *colorp = color.bgra;
            uint8_t red = 255;
            uint8_t green = 255;
            uint8_t blue = 255;
            uint8_t alpha = 255;
            int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
            vertices[i].color = color;
        }

        // copy texcoords to the buffer
        const float textureWidth = float(frame.width) / float(SPRITESHEET_WIDTH);
        const float textureHeight = float(frame.height) / float(SPRITESHEET_HEIGHT);
        const float textureX = float(frame.x) / float(SPRITESHEET_WIDTH);
        const float textureY = float(frame.y) / float(SPRITESHEET_HEIGHT);

        const float spriteLeft = textureX;
        const float spriteRight = spriteLeft + textureWidth;
        const float spriteTop = 1.0f - (textureY);
        const float spriteBottom = spriteTop - textureHeight;

        // copy texcoords to the buffer
        vertices[0].uv.x = vertices[1].uv.x = spriteLeft;
        vertices[0].uv.y = vertices[3].uv.y = spriteTop;
        vertices[1].uv.y = vertices[2].uv.y = spriteBottom;
        vertices[2].uv.x = vertices[3].uv.x = spriteRight;

        // finally upload everything to the actual vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            sizeof(vertices) * index,
            sizeof(vertices),
            vertices);

        ++index;
    }

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindVertexArray(m_vao);

    Debug::checkGLError();

    m_shader->bindProgram();

    Debug::checkGLError();

    glDrawElements(
        GL_TRIANGLES,
        6 * (m_characterSprites.size()), // 6 indices per 2 triangles
        GL_UNSIGNED_INT,
        (const GLvoid*)0);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}

void SpriteSheetRenderer::renderEntities()
{
    m_shader->bindProgram();

    bindSpriteSheet(SpriteSheetType::Entity);

    Debug::checkGLError();

    int index = 0;
    for (Sprite * sprite : m_entitySprites) {
        auto frameIdentifier = m_spriteSheetEntitiesDescription.find(sprite->frameName());
        //Debug::fatal(frameIdentifier != m_spriteSheetEntitiesDescription.end(), Debug::Area::SpriteSheetRendererArea, "sprite sheet entity frame description could not be located framename: " + sprite->frameName());

        SpriteFrameIdentifier& frame = frameIdentifier.value();

        // vertices that will be uploaded.
        Vertex vertices[4];

        // vertices[n][0] -> X, and [1] -> Y
        // vertices[0] -> top left
        // vertices[1] -> bottom left
        // vertices[2] -> bottom right
        // vertices[3] -> top right

        glm::vec2 spritePosition = sprite->position();

        glm::vec2 spriteSize = sprite->sizeMeters();

        glm::vec4 rect = glm::vec4(spritePosition.x - (spriteSize.x * 0.5f), spritePosition.y - (spriteSize.x * 0.5f), spritePosition.x + (spriteSize.x * 0.5f), spritePosition.y + (spriteSize.y * 0.5f));

        float x = rect.x;
        float width = rect.z;

        float y = rect.y;
        float height = rect.w;

        vertices[0].position.x = x; // top left X
        vertices[0].position.y = y; //top left Y

        vertices[1].position.x = x; // bottom left X
        vertices[1].position.y = height; // bottom left Y

        vertices[2].position.x = width; // bottom right X
        vertices[2].position.y = height; //bottom right Y

        vertices[3].position.x = width; // top right X
        vertices[3].position.y = y; // top right Y

        Debug::checkGLError();

        // copy color to the buffer
        for (size_t i = 0; i < sizeof(vertices) / sizeof(*vertices); i++) {
            //        *colorp = color.bgra;
            uint8_t red = 255;
            uint8_t green = 255;
            uint8_t blue = 255;
            uint8_t alpha = 255;
            int32_t color = red | (green << 8) | (blue << 16) | (alpha << 24);
            vertices[i].color = color;
        }
        /*
        const float tileWidth = 1.0f / TILESHEET_WIDTH * 16.0f;
        const float tileHeight = 1.0f / TILESHEET_HEIGHT * 16.0f;

        float xPadding = 1.0f / TILESHEET_WIDTH * 1.0f * (column + 1);
        float yPadding = 1.0f / TILESHEET_HEIGHT * 1.0f * (row + 1);

        const float tileLeft = (column *  tileWidth) + xPadding;
        const float tileRight = tileLeft + tileWidth;
        const float tileTop = 1.0f - ((row * tileHeight)) - yPadding;
        const float tileBottom = tileTop - tileHeight;

        // copy texcoords to the buffer
        vertices[0].u = vertices[1].u = tileLeft;
        vertices[0].v = vertices[3].v = tileTop;
        vertices[1].v = vertices[2].v = tileBottom;
        vertices[2].u = vertices[3].u = tileRight;
        */

        // copy texcoords to the buffer
        const float textureWidth = float(frame.width) / float(SPRITESHEET_WIDTH);
        const float textureHeight = float(frame.height) / float(SPRITESHEET_HEIGHT);
        const float textureX = float(frame.x) / float(SPRITESHEET_WIDTH);
        const float textureY = float(frame.y) / float(SPRITESHEET_HEIGHT);

        const float spriteLeft = textureX;
        const float spriteRight = spriteLeft + textureWidth;
        const float spriteTop = 1.0f - (textureY);
        const float spriteBottom = spriteTop - textureHeight;

        // copy texcoords to the buffer
        vertices[0].uv.x = vertices[1].uv.x = spriteLeft;
        vertices[0].uv.y = vertices[3].uv.y = spriteTop;
        vertices[1].uv.y = vertices[2].uv.y = spriteBottom;
        vertices[2].uv.x = vertices[3].uv.x = spriteRight;

        // finally upload everything to the actual vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vboEntities);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            sizeof(vertices) * index,
            sizeof(vertices),
            vertices);

        ++index;
    }

    ////////////////////////////////FINALLY RENDER IT ALL //////////////////////////////////////////
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboEntities);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboEntities);
    glBindVertexArray(m_vaoEntities);

    Debug::checkGLError();

    m_shader->bindProgram();

    Debug::checkGLError();

    glDrawElements(
        GL_TRIANGLES,
        6 * (m_entitySprites.size()), // 6 indices per 2 triangles
        GL_UNSIGNED_INT,
        (const GLvoid*)0);

    m_shader->unbindProgram();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);

    Debug::checkGLError();
}

void SpriteSheetRenderer::initGL()
{
    Debug::checkGLError();
    initGLCharacters();
    Debug::checkGLError();
    initGLEntities();
    Debug::checkGLError();
}

void SpriteSheetRenderer::initGLCharacters()
{
    ///////////CHARACTERS////////////////
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        m_maxSpriteCount * 4 * sizeof(Vertex),
        NULL,
        GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < m_maxSpriteCount; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indicesv.size()*sizeof(uint32_t),
        indicesv.data(),
        GL_STATIC_DRAW);

    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(
        pos_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(glm::vec2);

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(
        color_attrib,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(Vertex),
        (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    Debug::checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(
        texcoord_attrib,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Debug::checkGLError();

}

void SpriteSheetRenderer::initGLEntities()
{
    /////////////////////////////// ENTITIES
    glGenVertexArrays(1, &m_vaoEntities);
    glBindVertexArray(m_vaoEntities);

    glGenBuffers(1, &m_vboEntities);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboEntities);
    glBufferData(GL_ARRAY_BUFFER, m_maxEntityCount * 4 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

    Debug::checkGLError();

    std::vector<uint32_t> indicesv;

    // prepare and upload indices as a one time deal
    const uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // pattern for a triangle array
    // for each possible sprite, add the 6 index pattern
    for (uint32_t j = 0; j < m_maxSpriteCount; j++) {
        for (uint32_t i = 0; i < sizeof(indices) / sizeof(*indices); i++) {
            indicesv.push_back(4 * j + indices[i]);
        }
    }

    glGenBuffers(1, &m_eboEntities);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_eboEntities);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesv.size() * sizeof(uint32_t), indicesv.data(), GL_STATIC_DRAW);

    Debug::checkGLError();

    size_t buffer_offset = 0;

    GLint pos_attrib = glGetAttribLocation(m_shader->shaderProgram(), "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)buffer_offset);

    buffer_offset += sizeof(glm::vec2);

    GLint color_attrib = glGetAttribLocation(m_shader->shaderProgram(), "color");

    Debug::checkGLError();

    glEnableVertexAttribArray(color_attrib);
    glVertexAttribPointer(color_attrib, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)buffer_offset);
    buffer_offset += sizeof(uint32_t);

    Debug::checkGLError();

    GLint texcoord_attrib = glGetAttribLocation(m_shader->shaderProgram(), "texcoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)buffer_offset);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
