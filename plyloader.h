#ifndef PLYLOADER_H
#define PLYLOADER_H

#include <glm/glm.hpp>
#include <glm/gtx/raw_data.hpp>

#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <array>

class ResourceManager;
class Scene3D;

struct PLYProperty
{
    enum class Type : uint8_t
    {
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        FLOAT32,
        DOUBLE,
        INVALID
    };
    
    static constexpr std::array<size_t, 9> typeSizes =
    {
      sizeof(int32_t),
      sizeof(uint32_t),
      sizeof(int16_t),
      sizeof(uint16_t),
      sizeof(int32_t),
      sizeof(uint32_t),
      sizeof(float),
      sizeof(double),
      0
    };
    
    PLYProperty(){};
    PLYProperty(const std::string& name, Type & type) : mName(name), mType(type), mTypeSize(typeSizes[static_cast<int>(mType)]){};
        
    std::string mName;
    Type mType;
    size_t mTypeSize;
    Type mListType;
    bool mIsList = false;
    size_t mListCount = 0;
};

struct PLYElement
{

    PLYElement(const std::string& name, size_t count) : mName(name), mCount(count){};
    
    std::string mName;
    size_t mCount;
    std::vector<PLYProperty> mProperties;
};

class PLYLoader
{
public:
    PLYLoader(ResourceManager* manager, Scene3D* scene, std::string filePath);
    Scene3D* loadScene(std::string filePath);
    
private:
    bool readHeader(std::istream& inputStream);
    void readFormat(std::istream& lineStream);
    void readElement(std::istream& lineStream);
    void readProperty(std::istream& lineStream);
    PLYProperty::Type readPropertyType(const std::string& stringType);
    
    template<typename T>
    T readASCII(std::istream & inputStream)
    {
        T data;
        inputStream >> data;
        return data;
    }
    template<typename T>
    void castASCIIProperty(T data, std::vector<glm::byte>& destination, size_t offset)
    {
        std::vector<T> vecData(1, data);
        memcpy(destination.data() + offset, &data, sizeof(data));
        //std::copy(begin(vecData), end(vecData), begin(destination) + offset);
    }
    
    void readASCIIProperty(PLYProperty::Type type, std::istream & inputStream, std::vector<glm::byte>& destination, size_t offset);
    
    void readData(std::istream& inputStream);
    
    ResourceManager* mResourceManager = nullptr;
    Scene3D* mScene = nullptr;
    bool mIsBinary = false;
    bool mIsBigEndian = false;
    
    std::vector<PLYElement> mElements;
};

#endif
