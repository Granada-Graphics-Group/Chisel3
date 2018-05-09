#ifndef LAYER_H
#define LAYER_H

#include "palette.h"

#include <cereal/types/utility.hpp>
#include <cereal/types/string.hpp>
#include <cereal/access.hpp>

#include <vector>
#include <set>
#include <string>

class Texture;

class Layer
{
public:
    enum class Type : int
    {
        Int8 = 0, 
        UInt8 = 1,
        Int16 = 2,
        UInt16 = 3,
        Int32 = 4,
        UInt32 = 5,
        Register = 6,
        Float16 = 7,
        Float32 = 8
    };
    
    Layer(std::string name, Layer::Type type, const std::pair< int, int >& resolution, float opacity);
    Layer(std::string name, Layer::Type type, const std::pair< int, int >& resolution, const std::vector< std::pair< Texture*, Texture* > >& textures, Palette* palette = nullptr);
    virtual ~Layer();
    
    const std::string& name() const { return mName; }
    const std::string& path() const { return mPath; }
    const Type type() const { return mType; }
    std::string typeString() const;
    bool registerType() const { return mType == Layer::Type::Register; }
    bool discrete() const { return mType < Layer::Type::Float16; }
    bool continuous() const { return mType > Layer::Type::Register;}    
    const int width() const { return mResolution.first; }
    const int height() const { return mResolution.second; }
    const std::pair<int, int> resolution() const { return mResolution; }
    Palette* palette() const { return mPalette; }
    const std::vector<std::pair<Texture*, Texture*>>& textures() const { return mTextures; }
    //TODO: Doesn't support multiple textures per Layer
    Texture *dataTexture() const { return mTextures.back().first; }
    Texture *maskTexture() const { return mTextures.back().second; }
//    Texture *paletteTexture() const { return (discrete()) ? mPalette->texture(Palette::Type::Discrete) : mPalette->texture(Palette::Type::Linear); };
    Texture *paletteTexture() const { return mPalette->texture(); }
    float opacity() const { return mOpacity; }
    bool isVisible() const { return mVisibility == true; }
    bool isDirty() const { return mDirtyFlag == true; }
    
    void setName(const std::string& name) { mName = name; }
    void setPalette(Palette* palette);
    void copyPalette(const Palette& palette);
    void setOpacity(float opacity){ if(opacity >= 0.0 && opacity <= 1.0) mOpacity = opacity; mDirtyFlag = true; }
    void setVisibility(bool visible) { mVisibility = visible; mDirtyFlag = true; }
    void toggleVisibility(){ mVisibility = !mVisibility; }
    void setDirty(bool value) { mDirtyFlag = value; }
    
    std::pair< int, bool > addValue(double value);
    bool updateValue(double prevValue, double newValue);
    bool removeValue(double value);
    
    void updatePaletteInterpolation();
    
    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(mName, mType, mResolution, mOpacity);
    }
    
    template <class Archive>
    static void load_and_construct( Archive & ar, cereal::construct<Layer> & construct )
    {
        std::string name;
        Type lType;
        std::pair<int, int> resolution;
        float opacity;
        
        ar(name, lType, resolution, opacity);
        
        construct(name, lType, resolution, opacity);  
    }
    
private:
    //void updatePaletteInterpolation();
    
    std::string mName;
    std::string mPath;
    Type mType;
    std::pair<int, int> mResolution;    
    Palette* mPalette = nullptr;
    float mOpacity = 1.0;
    bool mVisibility = true;
    bool mDirtyFlag = false;
    
    std::vector<std::pair<Texture*, Texture*>> mTextures;
    std::set<double> mValues;
};

#endif // LAYER_H
