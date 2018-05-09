
// Generated from /media/loki/Niflheim/Tesis/CHISel/OpParser/OLexer.g4 by ANTLR 4.7

#pragma once


#include "antlr4-runtime.h"


namespace OpLanguage {


class  OLexer : public antlr4::Lexer {
public:
  enum {
    BKNL = 1, SHARP = 2, OPENPAR = 3, CLOSEPAR = 4, ASSIGN = 5, COLON = 6, 
    SEMICOLON = 7, COMMA = 8, NEGSUB = 9, COMPLEMENT = 10, EXPONENT = 11, 
    MODULUS = 12, DIV = 13, MULT = 14, ADD = 15, LSHIFT = 16, RSHIFT = 17, 
    RSHIFTU = 18, GREATER = 19, GREATEREQUAL = 20, LESS = 21, LESSEQUEAL = 22, 
    EQUAL = 23, NEQUAL = 24, BITAND = 25, BITOR = 26, LAND = 27, LANDONE = 28, 
    LOR = 29, LORONE = 30, LNOT = 31, COND = 32, ABS = 33, ACOS = 34, ASIN = 35, 
    ATAN = 36, COS = 37, DOUBLECAST = 38, EVAL = 39, EXPFUNC = 40, FLOATCAST = 41, 
    IF = 42, INTCAST = 43, ISNULL = 44, LOG = 45, MAX = 46, MEDIAN = 47, 
    MIN = 48, MODE = 49, NMAX = 50, NMEDIAN = 51, NMIN = 52, NMODE = 53, 
    NOT = 54, POW = 55, RAND = 56, ROUND = 57, SIN = 58, SQRT = 59, TAN = 60, 
    XOR = 61, NULLV = 62, SQL = 63, FLOAT = 64, DOUBLE = 65, INT = 66, NAME = 67, 
    STRING = 68, WS = 69
  };

  OLexer(antlr4::CharStream *input);
  ~OLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace OpLanguage
