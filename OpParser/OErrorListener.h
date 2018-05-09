
// Generated from /media/loki/Niflheim/Tesis/CHISel/OpParser/OParser.g4 by ANTLR 4.6

#pragma once


#include "antlr4-runtime.h"
#include "OParser.h"


namespace OpLanguage 
{
    class  OErrorListener : public antlr4::BaseErrorListener 
    {
    public:
        void syntaxError(antlr4::Recognizer * recognizer, antlr4::Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string & msg, std::exception_ptr e) override;
        
        const std::map<int, antlr4::Token*, std::greater<int>>& offendingSymbols() { return mOffendingSymbols; }
        const std::string& syntaxErrorMessages() { return mSyntaxErrorMessages; }
    
    private:
        std::map<int, antlr4::Token*, std::greater<int>> mOffendingSymbols;
        std::string mSyntaxErrorMessages;
    };
}
