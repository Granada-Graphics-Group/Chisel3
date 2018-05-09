#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "fileloader.h"

class ResourceManager;
class Scene3D;

class OBJLoader : public FileLoader
{
public:
    OBJLoader(ResourceManager* manager, Scene3D* scene, std::string filePath);
    
    void loadScene(std::string filePath);
//     virtual void extractWordsFromNextLine(std::ifstream& file, std::vector< std::string >& words, const std::string& delimiter);
//     virtual void extractWordsFromLine(std::ifstream& file, std::string& line, std::vector< std::string >& words, const std::string& delimiter);
//     virtual void extractWords (std::vector< std::string >& words, std::string& line, const std::string& delimiter);

private:
    ResourceManager* mManager;
    Scene3D* mScene;
};

#endif // OBJLOADER_H
