
// Generated from /media/loki/Niflheim/Tesis/github/Chisel3/OpParser/OLexer.g4 by ANTLR 4.7

#pragma once


#include "antlr4-runtime.h"


namespace OpLanguage {


class  OLexer : public antlr4::Lexer {
public:
  enum {
    BKNL = 1, SHARP = 2, OPENPAR = 3, CLOSEPAR = 4, OPENBRACK = 5, CLOSEBRACK = 6, 
    ASSIGN = 7, COLON = 8, SEMICOLON = 9, COMMA = 10, NEGSUB = 11, COMPLEMENT = 12, 
    EXPONENT = 13, MODULUS = 14, DIV = 15, MULT = 16, ADD = 17, LSHIFT = 18, 
    RSHIFT = 19, RSHIFTU = 20, GREATER = 21, GREATEREQUAL = 22, LESS = 23, 
    LESSEQUEAL = 24, EQUAL = 25, NEQUAL = 26, BITAND = 27, BITOR = 28, LAND = 29, 
    LANDONE = 30, LOR = 31, LORONE = 32, LNOT = 33, COND = 34, ABS = 35, 
    ACOS = 36, ASIN = 37, ATAN = 38, CEIL = 39, COS = 40, DOUBLECAST = 41, 
    EVAL = 42, EXPFUNC = 43, FLOATCAST = 44, FLOOR = 45, GRAPH = 46, GRAPH2 = 47, 
    IF = 48, INTCAST = 49, ISNULL = 50, LOG = 51, MAX = 52, MEDIAN = 53, 
    MIN = 54, MODE = 55, NMAX = 56, NMEDIAN = 57, NMIN = 58, NMODE = 59, 
    NOT = 60, POW = 61, RAND = 62, ROUND = 63, SIN = 64, SQRT = 65, TAN = 66, 
    XOR = 67, NULLV = 68, SQL = 69, FLOAT = 70, DOUBLE = 71, INT = 72, NAME = 73, 
    STRING = 74, WS = 75
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
