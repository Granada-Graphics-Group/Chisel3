#include "fileloader.h"

#include "cstdlib"
#include "fstream"

using namespace std;

void FileLoader::extractWords(vector< string >& words, string& line, const string& delimiter)
{
    size_t pos, posAnterior = 0;
    words.clear();

    while ( ( pos = line.find ( delimiter, posAnterior ) ) != string::npos ) 
    {
        words.push_back ( line.substr ( posAnterior,pos-posAnterior ) );
        posAnterior = pos + delimiter.size();
    }

    words.push_back ( line.substr ( posAnterior, line.size() ) );
}

void FileLoader::extractWordsFromLine(ifstream& file, string& line, vector< string >& words, const string& delimiter)
{
    size_t pos, lastPos = 0;

    words.clear();

    getline ( file, line );

    while ( ( pos = line.find ( delimiter, lastPos ) ) != string::npos )
    {
        if(pos != lastPos)
            words.push_back ( line.substr ( lastPos,pos-lastPos ) );

        lastPos = pos + delimiter.size();
    }

    words.push_back ( line.substr ( lastPos, line.size() ) );
}

void FileLoader::extractWordsFromNextLine(ifstream& file, vector< string >& words, const string& delimiter)
{
    string line;
    size_t pos, lastPos = 0;

    words.clear();

    getline ( file, line );

    while ( ( pos = line.find ( delimiter, lastPos ) ) != string::npos )
    {
        if(pos != lastPos)
            words.push_back ( line.substr ( lastPos,pos-lastPos ) );

        lastPos = pos + delimiter.size();
    }

    words.push_back ( line.substr ( lastPos, line.size() ) );
}

