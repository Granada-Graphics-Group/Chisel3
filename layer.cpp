#include "layer.h"
#include "palette.h"

Layer::Layer(std::string name, Layer::Type type, const std::pair<int, int>& resolution, float opacity)
:
    mName(name),
    mType(type),
    mResolution(resolution),
    mOpacity(opacity)
{
    
}


Layer::Layer(std::string name, Type type, const std::pair<int, int>& resolution, const std::vector<std::pair<Texture*, Texture*>>& textures, Palette* palette)
:
    mName(name),
    mType(type),
    mResolution(resolution),
    mTextures(textures),
    mPalette(palette)
{
    updatePaletteInterpolation();
}

Layer::~Layer()
{

}


// *** Public Methods *** //

std::string Layer::typeString() const
{
    switch(mType)
    {
        case Type::Int8: return "Int8";
        case Type::UInt8: return "UInt8";
        case Type::Int16: return "Int16";
        case Type::UInt16: return "UInt16";
        case Type::Int32: return "Int32";
        case Type::UInt32: return "UInt32";
        case Type::Register: return "DBase";
        case Type::Float16: return "Float16";
        case Type::Float32: return "Float32";
    }
}


void Layer::setPalette(Palette* palette)
{
    mPalette = palette;
    updatePaletteInterpolation();
    
    mDirtyFlag = true;
}

void Layer::copyPalette(const Palette& palette)
{   
    //auto name = mPalette->name();
    //auto texture = mPalette->texture();
    //*mPalette = palette;
    //mPalette->setName(palette.name());
    //mPalette->setTexture(texture);
    mPalette->setType(palette.type());
    mPalette->setIndex(palette.index());
    mPalette->setControlPoints(palette.controlPoints());
    
    updatePaletteInterpolation();
    
    mDirtyFlag = true;
}

std::pair< int, bool > Layer::addValue(double value)
{
    auto result = mValues.insert(value);
    
    if(result.second)
        return { std::distance(begin(mValues), result.first), result.second };
    else
        return { -1, result.second };
}

bool Layer::updateValue(double prevValue, double newValue)
{
    auto find = mValues.find(prevValue);
    
    if(find != end(mValues))
    {
        mValues.erase(find);
        mValues.insert(newValue);
        
        return true;
    }
    
    return false;
}

bool Layer::removeValue(double value)
{
    auto find = mValues.find(value);
    
    if(find != end(mValues))
    {
        mValues.erase(find);
        return true;
    }
    
    return false;
}


// *** Private Methods *** //

void Layer::updatePaletteInterpolation()
{
    if(discrete())
        mPalette->setType(Palette::Type::Discrete);
    else
        mPalette->setType(Palette::Type::Linear);
}
