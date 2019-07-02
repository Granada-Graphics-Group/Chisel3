#include "palette.h"

#include <glm/gtc/type_ptr.hpp>

#include <iterator>

Palette::Palette(unsigned int index, std::string name, std::string path, Type type, const std::map< double, glm::vec4 >& controlPoints)
:
    mIndex(index),
    mName(name),
    mPath(path),
    mType(type),
    mControlPoints(controlPoints)    
{

}

Palette::~Palette()
{

}


// *** Public Methods ***//

const std::pair< double, glm::tvec4< float, glm::precision::packed_highp > > Palette::controlPoint(int index) const
{
    auto iter = mControlPoints.begin();
    std::advance(iter, index);
    return *iter;
}

std::vector<double> Palette::controlPointValues() const
{
	std::vector<double> values;

	for (const auto& controlPoint : mControlPoints)
		values.push_back(controlPoint.first);

	return values;
}

const glm::vec4 Palette::color(double value) const
{
    auto found = mControlPoints.find(value);
    
    if(value >= minValue() && value <= maxValue())
    {
        if (found != end(mControlPoints))
            return found->second;
        else //if(mType == Type::Linear)
        {
            auto lower = mControlPoints.lower_bound(value);
            auto upper = mControlPoints.upper_bound(value);

            lower--;
            
            auto interpolant = (value - lower->first) / (upper->first - lower->first);
            
            return glm::mix(lower->second, upper->second, interpolant);// (lower->second) * static_cast<float>(1.0 - percentage) + (upper->second) * static_cast<float>(percentage);
        }
        //else
        //{
        //    auto lower = mControlPoints.lower_bound(value);
        //    lower--;
        //    return lower->second;
        //}
    }
    else if(value < minValue())
    {
        return mControlPoints.begin()->second;
    }
    else if(value > maxValue())
        return mControlPoints.rbegin()->second;
}

const double Palette::minValue() const
{
    return mControlPoints.begin()->first;
}

const double Palette::maxValue() const
{
    return mControlPoints.rbegin()->first;
}

int Palette::indexOfValue(double value)
{
    auto found = false;
    auto index = -1;
    auto iter = mControlPoints.begin();
    
    for(int i = 0; i < mControlPoints.size() && !found; ++i)
    {
        if((*iter).first == value)
        {
            found = true;
            index = i;
        }
        
        iter++;
    }
    
    return index;
}

bool Palette::containControlPoint(double value) const
{
    return mControlPoints.find(value) != end(mControlPoints);
}

void Palette::rescaleTo(double min, double max, bool round)
{
    auto oldMin = mControlPoints.begin()->first;
    auto oldMax = mControlPoints.rbegin()->first;
    
    std::map<double, glm::vec4> newControlPoints;
/*    
    newControlPoints[min] = mControlPoints[oldMin];
    newControlPoints[max] = mControlPoints[oldMax];*/
    
    for(const auto& cpoint: mControlPoints)
    {
        auto newValue = ((max - min) * (cpoint.first - oldMin))/(oldMax - oldMin) + min;
        if(round) newValue = std::round(newValue);
        newControlPoints[newValue] = cpoint.second;
    }
    
    mControlPoints.swap(newControlPoints);
}

void Palette::addControlPoint(double value, glm::vec4 color)
{
    mControlPoints[value] = color;
    mDiskDirtyFlag = mDirtyFlag = true;
}

bool Palette::delControlPoint(double value)
{
    auto found = mControlPoints.find(value);
    if(found != end(mControlPoints))
    {
        mControlPoints.erase(found);
        mDiskDirtyFlag = mDirtyFlag = true;
        
        return true;
    }
    
    return false;
}

bool Palette::replaceControlPoint(double value, double newValue)
{
    auto found = mControlPoints.find(value);
    
    if(found != end(mControlPoints))
    {
        auto color = found->second;
        mControlPoints.erase(found);
        mControlPoints[newValue] = color;
        mDiskDirtyFlag = mDirtyFlag = true;
        
        return true;
    }
    
    return false;
}

bool Palette::replaceControlPoint(double value, double newValue, glm::vec4 color)
{
    auto found = mControlPoints.find(value);
    
    if(found != end(mControlPoints))
    {
        mControlPoints.erase(found);
        mControlPoints[newValue] = color;
        mDiskDirtyFlag = mDirtyFlag = true;
        return true;
    }
    
    return false;    
}

bool Palette::updateColor(double value, glm::vec4 color)
{
    auto found = mControlPoints.find(value);
    
    if(found != end(mControlPoints))
    {
        found->second = color;
        mDiskDirtyFlag = mDirtyFlag = true;
    }
    
    return false;
}

std::array< float, int(16384) > Palette::paletteTextureData(Type type)
{
    std::array<float, 4096 * 4> paletteTextureData;
    
    if(type == Type::Discrete)
    {
        auto count = std::floor(mControlPoints.rbegin()->first - mControlPoints.begin()->first) + 1;
        auto colorWidth = 4096 / count;

        for (int i = 0; i < count; ++i)
        {
            auto currentColor = color(mControlPoints.begin()->first + i);
            for(int j = i * colorWidth; j < (i + 1) * colorWidth; ++j)
                std::copy(glm::value_ptr(currentColor), glm::value_ptr(currentColor) + 4, begin(paletteTextureData) + j * 4);
        }
    }
    else
    {
        auto min = minValue();
        auto stepDistance = (maxValue() - min) / 4096.0;
 
        for (int i = 0; i < 4096; ++i)
        {
            auto currentColor = color(min + i * stepDistance);
            std::copy(glm::value_ptr(currentColor), glm::value_ptr(currentColor) + 4, begin(paletteTextureData) + i * 4);
        }
    }
    
    return paletteTextureData;
}


