#include "fontmanager.h"
#include "resourcemanager.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <logger.hpp>

FontManager::FontManager(ResourceManager* manager)
:
    mManager(manager)
{    
}

FontManager::~FontManager()
{
}


// *** Public Methods *** //

const GLFont* FontManager::font(std::string name) const
{
    auto found = mFontCollection.find(name);
    
    if(found != mFontCollection.end())
        return &((*found).second);
    else
        return nullptr;
}


Texture* FontManager::loadFont(std::string fontPath, std::string fontName, unsigned int fontSize)
{
    FT_Library library;
    if (FT_Init_FreeType(&library))
        LOG("ERROR::FREETYPE: Could not init FreeType Library");    
    
    FT_Face face;
    if (FT_New_Face(library, (fontPath + fontName).c_str(), 0, &face))
        LOG("ERROR::FREETYPE: Failed to load font");
    
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    
    Texture* fontTextute = mManager->createTexture(fontName, GL_TEXTURE_2D, GL_R8, 1024, 1024, GL_RED, GL_UNSIGNED_BYTE, {}, GL_NEAREST, GL_NEAREST, 0, false);    
    std::vector<Glyph> glyphData;    
    glm::ivec2 texturePosition{0, 0};
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    for (GLubyte character = 0; character < 128; character++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, character, FT_LOAD_RENDER))
        {
            LOG("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        
        Glyph glyph = 
        {        
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x),
            texturePosition
        };
        
        glyphData.emplace_back(glyph);
        glTextureSubImage3D(fontTextute->textureArrayId(), 0, texturePosition.x, texturePosition.y, fontTextute->textureArrayLayerIndex() , face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, fontTextute->format(), fontTextute->type(), face->glyph->bitmap.buffer);
        
        if((texturePosition.x + glyph.mSize.x + 1 + fontSize) < fontTextute->width())
            texturePosition.x += glyph.mSize.x + 1;
        else
        {
            texturePosition.x = 0;
            texturePosition.y += fontSize;
        }               
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    
    mFontCollection[fontName] = {fontName, fontSize, fontTextute, glyphData};
    
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    
    return fontTextute;
}

Mesh* FontManager::generateMeshText(std::string fontName, std::string text, int x, int y, int align)
{
    auto font = mFontCollection[fontName];
    auto fontData = font.mGlyphData;
    auto fontTexture = font.mTexture;
    int scale = 1;
    glm::vec4 color{0, 0, 0, 0};
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texCoords;
    std::vector<GLuint> indices;
           
    unsigned int vertexCount = 0;
    
    if(align & Alignment::Right)
        x -= textWidth(font, text);
    else if(align & Alignment::CenteredHorizontal)
        x -= textWidth(font, text)/2;
    
    if(align & Alignment::Top)
        y -= textHeight(font, text);
    else if(align & Alignment::CenteredVertical)
        y -= textHeight(font, text)/2;
    else if(align & Alignment::Bottom)
        y -= textHeight(font, text);
    
    
    for (auto character = text.begin(); character != text.end(); character++)
    {
        Glyph glyph = fontData[*character];

        GLfloat xpos = x + glyph.mBearing.x * scale;
        GLfloat ypos = y - (glyph.mSize.y - glyph.mBearing.y) * scale;

        GLfloat w = glyph.mSize.x * scale;
        GLfloat h = glyph.mSize.y * scale;
        
        GLfloat xTexCoord = static_cast<GLfloat>(glyph.mTexturePosition.x) / fontTexture->width();
        GLfloat yTexCoord = static_cast<GLfloat>(glyph.mTexturePosition.y) / fontTexture->height();
        GLfloat wTexCoord = w / fontTexture->width();
        GLfloat hTexCoord = h / fontTexture->height();
        
        vertices.insert(end(vertices),
                        { xpos,     ypos + h,   0.0,
                          xpos,     ypos,       0.0,
                          xpos + w, ypos,       0.0,
                          xpos + w, ypos + h,   0.0,
                        }
                       );
        
        colors.insert(end(colors),
                        { color.r, color.g, color.b, color.a,
                          color.r, color.g, color.b, color.a,
                          color.r, color.g, color.b, color.a,
                          color.r, color.g, color.b, color.a,                        
                        }
                       );
        
        normals.insert(end(normals),
                        { 0.0, 0.0, 1.0,
                          0.0, 0.0, 1.0,
                          0.0, 0.0, 1.0,
                          0.0, 0.0, 1.0,
                        }
                       );

        texCoords.insert(end(texCoords),
                        { xTexCoord,             yTexCoord,
                          xTexCoord,             yTexCoord + hTexCoord,
                          xTexCoord + wTexCoord, yTexCoord + hTexCoord,
                          xTexCoord + wTexCoord, yTexCoord,
                        }
                       );

        indices.insert(end(indices), {vertexCount, vertexCount + 1, vertexCount + 2, vertexCount, vertexCount + 2, vertexCount + 3});
        
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (glyph.mAdvance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        
        vertexCount += 4;
    }
    
    std::vector<GLuint> subMeshIndexes = { static_cast<GLuint>(indices.size()) };
    
    Mesh* mesh = new Mesh(fontName + std::to_string(x) + std::to_string(y));
    mesh->updateData(GLBuffer::Vertex, 0, vertices.size() * sizeof(GLfloat), vertices.data());
    mesh->updateData(GLBuffer::Color, 0, colors.size() * sizeof(GLfloat), colors.data());    
    mesh->updateData(GLBuffer::Normal, 0, normals.size() * sizeof(GLfloat), normals.data());
    mesh->updateData(GLBuffer::UV, 0, texCoords.size() * sizeof(GLfloat), texCoords.data());
    mesh->updateData(GLBuffer::Index, 0, indices.size() * sizeof(GLuint), indices.data());
    mesh->updateSubMeshData(subMeshIndexes);
    
    return mesh;
}


// *** Private Methods *** //

GLuint FontManager::textWidth(const GLFont& font, const std::string& text)
{
    GLuint length = 0;
    auto fontData = font.mGlyphData;
        
    for (auto character = text.begin(); character != text.end(); character++)
        length += fontData[*character].mAdvance >> 6;
    
    return length;
}

GLuint FontManager::textHeight(const GLFont& font, const std::string& text)
{
    GLuint height = 0;
    auto fontData = font.mGlyphData;
    
    for (auto character = text.begin(); character != text.end(); character++)
    {
        auto charHeight = fontData[*character].mSize.y;
        if(charHeight > height)
            height = charHeight;
    }
    
    return height;
}
