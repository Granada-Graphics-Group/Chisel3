#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "const.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

class ResourceManager;
class Texture;
class Mesh;

struct Glyph 
{
    glm::ivec2 mSize;       // Size of glyph
    glm::ivec2 mBearing;    // Offset from baseline to left/top of glyph
    GLuint     mAdvance;    // Offset to advance to next glyph
    glm::ivec2 mTexturePosition;
};

struct GLFont
{
    std::string mName;
    unsigned int mSize;
    Texture* mTexture;
    std::vector<Glyph> mGlyphData;
};

class FontManager
{
public:    
    FontManager(ResourceManager *manager);
    ~FontManager();
    
    const GLFont* font(std::string name) const;
    Texture* loadFont(std::string fontPath, std::string fontName, unsigned int fontSize);    
    Mesh* generateMeshText(std::string fontName, std::string text, int x, int y, int align = Alignment::Left);
    
private:
    GLuint textWidth(const GLFont& font, const std::string& text);
    GLuint textHeight(const GLFont& font, const std::string& text);
    
    ResourceManager* mManager;
    std::map<std::string, GLFont> mFontCollection;       
};

#endif // FONTMANAGER_H
