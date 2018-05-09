#ifndef FILELOADER_H
#define FILELOADER_H

#include <iostream>
#include <vector>
#include <string>

class FileLoader
{
protected:
    virtual void extractWordsFromNextLine(std::ifstream& file, std::vector< std::string >& words, const std::string& delimiter);
    virtual void extractWordsFromLine(std::ifstream& file, std::string& line, std::vector< std::string >& words, const std::string& delimiter);
    virtual void extractWords (std::vector< std::string >& words, std::string& line, const std::string& delimiter);
};

#endif // FILELOADER_H
