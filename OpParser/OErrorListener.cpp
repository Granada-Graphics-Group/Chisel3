#include "OErrorListener.h"

void OpLanguage::OErrorListener::syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine, const std::string& msg, std::exception_ptr e)
{    
    mSyntaxErrorMessages += "line " + std::to_string(line) + " : position " + std::to_string(charPositionInLine) + " : " + msg + "\n";
    mOffendingSymbols[offendingSymbol->getStartIndex()] = offendingSymbol;    
}

