#ifndef PALETTE_H
#define PALETTE_H

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include <glm/glm.hpp>
#include <map>
#include <array>
#include <string>

class Texture;

class Palette
{

public:
    enum class Type: int
    {
        Discrete = 0,
        Linear
    };
    
    Palette(): Palette(0, "","") {};
    Palette(unsigned int index, std::string name, std::string path): Palette(index, name, path, Type::Discrete){};
    Palette(unsigned int index, std::string name, std::string path, Type type): Palette(index, name, path, type, {}){}; 
    Palette(unsigned int index, std::string name, std::string path, Type type, const std::map<double, glm::vec4>& controlPoints);
    ~Palette();

    const unsigned int index() const { return mIndex; }
    const std::string& name() const { return mName; }
    const std::string& path() const { return mPath; }
    const Type type() const { return mType; }
    bool isInterpolating() const { return mInterpolation; }
    const std::pair<double, glm::vec4> controlPoint(int index) const;
    const std::map<double, glm::vec4>& controlPoints() const { return mControlPoints; }
	std::vector<double> controlPointValues() const;
    Texture* texture() const { return mTexture; }
    bool isDirty() const { return mDirtyFlag; }
    bool isDiskDirty() const { return mDiskDirtyFlag; }
    const glm::vec4 color(double value) const;
    const double minValue() const;
    const double maxValue() const;
    int indexOfValue(double value);
    bool containControlPoint(double value) const;
    
    void setName(const std::string& name) { mName = name; mDiskDirtyFlag = true; }
    void setIndex(unsigned int index) { mIndex = index; }
    void setType(Type type) { mType = type; mDirtyFlag = true; mDiskDirtyFlag = true; }
    void setInterpolation(bool value) { mInterpolation = value; }
    void setControlPoints(const std::map<double, glm::vec4>& points) { mControlPoints = points; mDirtyFlag = true; mDiskDirtyFlag = true; }
    void setTexture(Texture* texture) { mTexture = texture; mDirtyFlag = true; }
    void setDirty(bool value) { mDirtyFlag = value; }
    void setDiskDirty(bool value) { mDiskDirtyFlag = value; }
    
    void rescaleTo(double min, double max, bool round = false);
    void addControlPoint(double value, glm::vec4 color);
    bool delControlPoint(double value);
    bool replaceControlPoint(double value, double newValue);
    bool replaceControlPoint(double value, double newValue, glm::vec4 color);
    bool updateColor(double value, glm::vec4 color);
    
    std::array<float, 4096 * 4> paletteTextureData(Type type);
    
    template <class Archive>
    void save(Archive& archive, std::uint32_t const version) const
    {
        archive(mType);
        archive(mInterpolation);
        archive(mControlPoints.size());
        for(const auto& point: mControlPoints)
            archive(point.first, point.second.x, point.second.y, point.second.z, point.second.w);
    }
    
    template <class Archive>
    void load(Archive& archive, std::uint32_t const version)
    {
        archive(mType);
        archive(mInterpolation);
        std::size_t pointCount;
        archive(pointCount);
        
        for(int i = 0; i < pointCount; ++i)
        {
            double value;
            glm::vec4 color;
            archive(value, color.x, color.y, color.z, color.w);
            mControlPoints[value] = color;
        }
    }    
    
private:
    unsigned int mIndex;
    std::string mName;
    std::string mPath;
    Type mType;
    bool mInterpolation = true;
    
    std::map<double, glm::vec4> mControlPoints;
    Texture* mTexture = nullptr;
    bool mDirtyFlag = false;
    bool mDiskDirtyFlag = false;
};

CEREAL_CLASS_VERSION(Palette, 1);

#endif // PALETTE_H
