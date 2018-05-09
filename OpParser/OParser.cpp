
// Generated from /media/loki/Niflheim/Tesis/CHISel/OpParser/OParser.g4 by ANTLR 4.7


#include "OParserListener.h"
#include "OParserVisitor.h"

#include "OParser.h"


using namespace antlrcpp;
using namespace OpLanguage;
using namespace antlr4;

OParser::OParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

OParser::~OParser() {
  delete _interpreter;
}

std::string OParser::getGrammarFileName() const {
  return "OParser.g4";
}

const std::vector<std::string>& OParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& OParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- ProgramContext ------------------------------------------------------------------

OParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

OParser::DefsContext* OParser::ProgramContext::defs() {
  return getRuleContext<OParser::DefsContext>(0);
}


size_t OParser::ProgramContext::getRuleIndex() const {
  return OParser::RuleProgram;
}

void OParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void OParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


antlrcpp::Any OParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

OParser::ProgramContext* OParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, OParser::RuleProgram);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(16);
    defs();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefsContext ------------------------------------------------------------------

OParser::DefsContext::DefsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OParser::DefContext *> OParser::DefsContext::def() {
  return getRuleContexts<OParser::DefContext>();
}

OParser::DefContext* OParser::DefsContext::def(size_t i) {
  return getRuleContext<OParser::DefContext>(i);
}


size_t OParser::DefsContext::getRuleIndex() const {
  return OParser::RuleDefs;
}

void OParser::DefsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefs(this);
}

void OParser::DefsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefs(this);
}


antlrcpp::Any OParser::DefsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitDefs(this);
  else
    return visitor->visitChildren(this);
}

OParser::DefsContext* OParser::defs() {
  DefsContext *_localctx = _tracker.createInstance<DefsContext>(_ctx, getState());
  enterRule(_localctx, 2, OParser::RuleDefs);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(18);
    def();
    setState(23);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(19);
        match(OParser::SEMICOLON);
        setState(20);
        def(); 
      }
      setState(25);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    }
    setState(27);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == OParser::SEMICOLON) {
      setState(26);
      match(OParser::SEMICOLON);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DefContext ------------------------------------------------------------------

OParser::DefContext::DefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* OParser::DefContext::STRING() {
  return getToken(OParser::STRING, 0);
}

OParser::ExprContext* OParser::DefContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

std::vector<tree::TerminalNode *> OParser::DefContext::NAME() {
  return getTokens(OParser::NAME);
}

tree::TerminalNode* OParser::DefContext::NAME(size_t i) {
  return getToken(OParser::NAME, i);
}


size_t OParser::DefContext::getRuleIndex() const {
  return OParser::RuleDef;
}

void OParser::DefContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDef(this);
}

void OParser::DefContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDef(this);
}


antlrcpp::Any OParser::DefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitDef(this);
  else
    return visitor->visitChildren(this);
}

OParser::DefContext* OParser::def() {
  DefContext *_localctx = _tracker.createInstance<DefContext>(_ctx, getState());
  enterRule(_localctx, 4, OParser::RuleDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(39);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OParser::STRING: {
        enterOuterAlt(_localctx, 1);
        setState(29);
        match(OParser::STRING);
        setState(30);
        match(OParser::ASSIGN);
        setState(31);
        expr(0);
        break;
      }

      case OParser::NAME: {
        enterOuterAlt(_localctx, 2);
        setState(32);
        match(OParser::NAME);
        setState(35);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == OParser::SHARP) {
          setState(33);
          match(OParser::SHARP);
          setState(34);
          match(OParser::NAME);
        }
        setState(37);
        match(OParser::ASSIGN);
        setState(38);
        expr(0);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

OParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OParser::ExprContext::getRuleIndex() const {
  return OParser::RuleExpr;
}

void OParser::ExprContext::copyFrom(ExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- LogAndExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::LogAndExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::LogAndExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::LogAndExpContext::LogAndExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::LogAndExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogAndExp(this);
}
void OParser::LogAndExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogAndExp(this);
}

antlrcpp::Any OParser::LogAndExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitLogAndExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- UnitaryExpContext ------------------------------------------------------------------

OParser::UnitaryOpContext* OParser::UnitaryExpContext::unitaryOp() {
  return getRuleContext<OParser::UnitaryOpContext>(0);
}

OParser::ExprContext* OParser::UnitaryExpContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::UnitaryExpContext::UnitaryExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::UnitaryExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnitaryExp(this);
}
void OParser::UnitaryExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnitaryExp(this);
}

antlrcpp::Any OParser::UnitaryExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitUnitaryExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ShiftExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::ShiftExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::ShiftExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::ShiftExpContext::ShiftExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::ShiftExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterShiftExp(this);
}
void OParser::ShiftExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitShiftExp(this);
}

antlrcpp::Any OParser::ShiftExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitShiftExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CondExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::CondExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::CondExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::CondExpContext::CondExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::CondExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCondExp(this);
}
void OParser::CondExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCondExp(this);
}

antlrcpp::Any OParser::CondExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitCondExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LogOrExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::LogOrExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::LogOrExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::LogOrExpContext::LogOrExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::LogOrExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogOrExp(this);
}
void OParser::LogOrExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogOrExp(this);
}

antlrcpp::Any OParser::LogOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitLogOrExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ParExpContext ------------------------------------------------------------------

OParser::ExprContext* OParser::ParExpContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::ParExpContext::ParExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::ParExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParExp(this);
}
void OParser::ParExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParExp(this);
}

antlrcpp::Any OParser::ParExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitParExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BinaryOpExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::BinaryOpExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::BinaryOpExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::BinaryOpExpContext::BinaryOpExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::BinaryOpExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBinaryOpExp(this);
}
void OParser::BinaryOpExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBinaryOpExp(this);
}

antlrcpp::Any OParser::BinaryOpExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitBinaryOpExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ConstantExpContext ------------------------------------------------------------------

OParser::ConstantContext* OParser::ConstantExpContext::constant() {
  return getRuleContext<OParser::ConstantContext>(0);
}

OParser::ConstantExpContext::ConstantExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::ConstantExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConstantExp(this);
}
void OParser::ConstantExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConstantExp(this);
}

antlrcpp::Any OParser::ConstantExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitConstantExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExponentialExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::ExponentialExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::ExponentialExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::ExponentialExpContext::ExponentialExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::ExponentialExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExponentialExp(this);
}
void OParser::ExponentialExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExponentialExp(this);
}

antlrcpp::Any OParser::ExponentialExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitExponentialExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- FunctionExpContext ------------------------------------------------------------------

OParser::FunctionContext* OParser::FunctionExpContext::function() {
  return getRuleContext<OParser::FunctionContext>(0);
}

OParser::FunctionExpContext::FunctionExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::FunctionExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunctionExp(this);
}
void OParser::FunctionExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunctionExp(this);
}

antlrcpp::Any OParser::FunctionExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitFunctionExp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BinLogOpExpContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::BinLogOpExpContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::BinLogOpExpContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::BinLogOpExpContext::BinLogOpExpContext(ExprContext *ctx) { copyFrom(ctx); }

void OParser::BinLogOpExpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBinLogOpExp(this);
}
void OParser::BinLogOpExpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBinLogOpExp(this);
}

antlrcpp::Any OParser::BinLogOpExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitBinLogOpExp(this);
  else
    return visitor->visitChildren(this);
}

OParser::ExprContext* OParser::expr() {
   return expr(0);
}

OParser::ExprContext* OParser::expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  OParser::ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, parentState);
  OParser::ExprContext *previousContext = _localctx;
  size_t startState = 6;
  enterRecursionRule(_localctx, 6, OParser::RuleExpr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(51);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OParser::OPENPAR: {
        _localctx = _tracker.createInstance<ParExpContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;

        setState(42);
        match(OParser::OPENPAR);
        setState(43);
        expr(0);
        setState(44);
        match(OParser::CLOSEPAR);
        break;
      }

      case OParser::ABS:
      case OParser::ACOS:
      case OParser::ASIN:
      case OParser::ATAN:
      case OParser::COS:
      case OParser::DOUBLECAST:
      case OParser::EVAL:
      case OParser::EXPFUNC:
      case OParser::FLOATCAST:
      case OParser::IF:
      case OParser::INTCAST:
      case OParser::ISNULL:
      case OParser::LOG:
      case OParser::MAX:
      case OParser::MEDIAN:
      case OParser::MIN:
      case OParser::MODE:
      case OParser::NMAX:
      case OParser::NMEDIAN:
      case OParser::NMIN:
      case OParser::NMODE:
      case OParser::NOT:
      case OParser::POW:
      case OParser::RAND:
      case OParser::ROUND:
      case OParser::SIN:
      case OParser::SQRT:
      case OParser::TAN:
      case OParser::XOR:
      case OParser::NULLV:
      case OParser::SQL: {
        _localctx = _tracker.createInstance<FunctionExpContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(46);
        function();
        break;
      }

      case OParser::FLOAT:
      case OParser::DOUBLE:
      case OParser::INT:
      case OParser::NAME:
      case OParser::STRING: {
        _localctx = _tracker.createInstance<ConstantExpContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(47);
        constant();
        break;
      }

      case OParser::NEGSUB:
      case OParser::COMPLEMENT:
      case OParser::LNOT: {
        _localctx = _tracker.createInstance<UnitaryExpContext>(_localctx);
        _ctx = _localctx;
        previousContext = _localctx;
        setState(48);
        unitaryOp();
        setState(49);
        expr(12);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(91);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(89);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 5, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<ExponentialExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(53);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(54);
          match(OParser::EXPONENT);
          setState(55);
          expr(12);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<BinaryOpExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(56);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(57);
          dynamic_cast<BinaryOpExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & ((1ULL << OParser::MODULUS)
            | (1ULL << OParser::DIV)
            | (1ULL << OParser::MULT))) != 0))) {
            dynamic_cast<BinaryOpExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(58);
          expr(11);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<BinaryOpExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(59);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(60);
          dynamic_cast<BinaryOpExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == OParser::NEGSUB

          || _la == OParser::ADD)) {
            dynamic_cast<BinaryOpExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(61);
          expr(10);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<ShiftExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(62);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(63);
          dynamic_cast<ShiftExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & ((1ULL << OParser::LSHIFT)
            | (1ULL << OParser::RSHIFT)
            | (1ULL << OParser::RSHIFTU))) != 0))) {
            dynamic_cast<ShiftExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(64);
          expr(9);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<BinLogOpExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(65);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(66);
          dynamic_cast<BinLogOpExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & ((1ULL << OParser::GREATER)
            | (1ULL << OParser::GREATEREQUAL)
            | (1ULL << OParser::LESS)
            | (1ULL << OParser::LESSEQUEAL))) != 0))) {
            dynamic_cast<BinLogOpExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(67);
          expr(8);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<BinLogOpExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(68);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(69);
          dynamic_cast<BinLogOpExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == OParser::EQUAL

          || _la == OParser::NEQUAL)) {
            dynamic_cast<BinLogOpExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(70);
          expr(7);
          break;
        }

        case 7: {
          auto newContext = _tracker.createInstance<BinaryOpExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(71);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(72);
          match(OParser::BITAND);
          setState(73);
          expr(6);
          break;
        }

        case 8: {
          auto newContext = _tracker.createInstance<BinaryOpExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(74);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(75);
          match(OParser::BITOR);
          setState(76);
          expr(5);
          break;
        }

        case 9: {
          auto newContext = _tracker.createInstance<LogAndExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(77);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");
          setState(78);
          dynamic_cast<LogAndExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == OParser::LAND

          || _la == OParser::LANDONE)) {
            dynamic_cast<LogAndExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(79);
          expr(4);
          break;
        }

        case 10: {
          auto newContext = _tracker.createInstance<LogOrExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(80);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(81);
          dynamic_cast<LogOrExpContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == OParser::LOR

          || _la == OParser::LORONE)) {
            dynamic_cast<LogOrExpContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(82);
          expr(3);
          break;
        }

        case 11: {
          auto newContext = _tracker.createInstance<CondExpContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(83);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(84);
          match(OParser::COND);
          setState(85);
          expr(0);
          setState(86);
          match(OParser::COLON);
          setState(87);
          expr(2);
          break;
        }

        } 
      }
      setState(93);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- FunctionContext ------------------------------------------------------------------

OParser::FunctionContext::FunctionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OParser::FunctionContext::getRuleIndex() const {
  return OParser::RuleFunction;
}

void OParser::FunctionContext::copyFrom(FunctionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ExponentialContext ------------------------------------------------------------------

OParser::ExprContext* OParser::ExponentialContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::ExponentialContext::ExponentialContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::ExponentialContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExponential(this);
}
void OParser::ExponentialContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExponential(this);
}

antlrcpp::Any OParser::ExponentialContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitExponential(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LogContext ------------------------------------------------------------------

OParser::ExprContext* OParser::LogContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::LogContext::LogContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::LogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLog(this);
}
void OParser::LogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLog(this);
}

antlrcpp::Any OParser::LogContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitLog(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NmaxContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::NmaxContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::NmaxContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::NmaxContext::NmaxContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::NmaxContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNmax(this);
}
void OParser::NmaxContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNmax(this);
}

antlrcpp::Any OParser::NmaxContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNmax(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CosContext ------------------------------------------------------------------

OParser::ExprContext* OParser::CosContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::CosContext::CosContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::CosContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCos(this);
}
void OParser::CosContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCos(this);
}

antlrcpp::Any OParser::CosContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitCos(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AtanContext ------------------------------------------------------------------

OParser::ExprContext* OParser::AtanContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::AtanContext::AtanContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::AtanContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtan(this);
}
void OParser::AtanContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtan(this);
}

antlrcpp::Any OParser::AtanContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitAtan(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CastToIntContext ------------------------------------------------------------------

OParser::ExprContext* OParser::CastToIntContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::CastToIntContext::CastToIntContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::CastToIntContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCastToInt(this);
}
void OParser::CastToIntContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCastToInt(this);
}

antlrcpp::Any OParser::CastToIntContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitCastToInt(this);
  else
    return visitor->visitChildren(this);
}
//----------------- SqlContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> OParser::SqlContext::NAME() {
  return getTokens(OParser::NAME);
}

tree::TerminalNode* OParser::SqlContext::NAME(size_t i) {
  return getToken(OParser::NAME, i);
}

OParser::SqlContext::SqlContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::SqlContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSql(this);
}
void OParser::SqlContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSql(this);
}

antlrcpp::Any OParser::SqlContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitSql(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NmodeContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::NmodeContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::NmodeContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::NmodeContext::NmodeContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::NmodeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNmode(this);
}
void OParser::NmodeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNmode(this);
}

antlrcpp::Any OParser::NmodeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNmode(this);
  else
    return visitor->visitChildren(this);
}
//----------------- RandContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::RandContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::RandContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::RandContext::RandContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::RandContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRand(this);
}
void OParser::RandContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRand(this);
}

antlrcpp::Any OParser::RandContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitRand(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NotContext ------------------------------------------------------------------

OParser::ExprContext* OParser::NotContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::NotContext::NotContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::NotContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNot(this);
}
void OParser::NotContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNot(this);
}

antlrcpp::Any OParser::NotContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNot(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MinContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::MinContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::MinContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::MinContext::MinContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::MinContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMin(this);
}
void OParser::MinContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMin(this);
}

antlrcpp::Any OParser::MinContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitMin(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NmedianContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::NmedianContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::NmedianContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::NmedianContext::NmedianContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::NmedianContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNmedian(this);
}
void OParser::NmedianContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNmedian(this);
}

antlrcpp::Any OParser::NmedianContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNmedian(this);
  else
    return visitor->visitChildren(this);
}
//----------------- SqrtContext ------------------------------------------------------------------

OParser::ExprContext* OParser::SqrtContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::SqrtContext::SqrtContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::SqrtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSqrt(this);
}
void OParser::SqrtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSqrt(this);
}

antlrcpp::Any OParser::SqrtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitSqrt(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NminContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::NminContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::NminContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::NminContext::NminContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::NminContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNmin(this);
}
void OParser::NminContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNmin(this);
}

antlrcpp::Any OParser::NminContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNmin(this);
  else
    return visitor->visitChildren(this);
}
//----------------- SinContext ------------------------------------------------------------------

OParser::ExprContext* OParser::SinContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::SinContext::SinContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::SinContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSin(this);
}
void OParser::SinContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSin(this);
}

antlrcpp::Any OParser::SinContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitSin(this);
  else
    return visitor->visitChildren(this);
}
//----------------- XorContext ------------------------------------------------------------------

OParser::ExprContext* OParser::XorContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::XorContext::XorContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::XorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterXor(this);
}
void OParser::XorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitXor(this);
}

antlrcpp::Any OParser::XorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitXor(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Log2Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::Log2Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::Log2Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::Log2Context::Log2Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::Log2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLog2(this);
}
void OParser::Log2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLog2(this);
}

antlrcpp::Any OParser::Log2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitLog2(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IfContext ------------------------------------------------------------------

OParser::ExprContext* OParser::IfContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::IfContext::IfContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::IfContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIf(this);
}
void OParser::IfContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIf(this);
}

antlrcpp::Any OParser::IfContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIf(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Atan2Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::Atan2Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::Atan2Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::Atan2Context::Atan2Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::Atan2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAtan2(this);
}
void OParser::Atan2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAtan2(this);
}

antlrcpp::Any OParser::Atan2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitAtan2(this);
  else
    return visitor->visitChildren(this);
}
//----------------- TanContext ------------------------------------------------------------------

OParser::ExprContext* OParser::TanContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::TanContext::TanContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::TanContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTan(this);
}
void OParser::TanContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTan(this);
}

antlrcpp::Any OParser::TanContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitTan(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MaxContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::MaxContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::MaxContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::MaxContext::MaxContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::MaxContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMax(this);
}
void OParser::MaxContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMax(this);
}

antlrcpp::Any OParser::MaxContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitMax(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CastToDoubleContext ------------------------------------------------------------------

OParser::ExprContext* OParser::CastToDoubleContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::CastToDoubleContext::CastToDoubleContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::CastToDoubleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCastToDouble(this);
}
void OParser::CastToDoubleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCastToDouble(this);
}

antlrcpp::Any OParser::CastToDoubleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitCastToDouble(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MathModeContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::MathModeContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::MathModeContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::MathModeContext::MathModeContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::MathModeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMathMode(this);
}
void OParser::MathModeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMathMode(this);
}

antlrcpp::Any OParser::MathModeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitMathMode(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AcosContext ------------------------------------------------------------------

OParser::ExprContext* OParser::AcosContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::AcosContext::AcosContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::AcosContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAcos(this);
}
void OParser::AcosContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAcos(this);
}

antlrcpp::Any OParser::AcosContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitAcos(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CastToFloatContext ------------------------------------------------------------------

OParser::ExprContext* OParser::CastToFloatContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::CastToFloatContext::CastToFloatContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::CastToFloatContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCastToFloat(this);
}
void OParser::CastToFloatContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCastToFloat(this);
}

antlrcpp::Any OParser::CastToFloatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitCastToFloat(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AbsContext ------------------------------------------------------------------

OParser::ExprContext* OParser::AbsContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::AbsContext::AbsContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::AbsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAbs(this);
}
void OParser::AbsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAbs(this);
}

antlrcpp::Any OParser::AbsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitAbs(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EvalContext ------------------------------------------------------------------

OParser::Expr_listContext* OParser::EvalContext::expr_list() {
  return getRuleContext<OParser::Expr_listContext>(0);
}

OParser::ExprContext* OParser::EvalContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::EvalContext::EvalContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::EvalContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEval(this);
}
void OParser::EvalContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEval(this);
}

antlrcpp::Any OParser::EvalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitEval(this);
  else
    return visitor->visitChildren(this);
}
//----------------- If2Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::If2Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::If2Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::If2Context::If2Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::If2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIf2(this);
}
void OParser::If2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIf2(this);
}

antlrcpp::Any OParser::If2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIf2(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MedianContext ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::MedianContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::MedianContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::MedianContext::MedianContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::MedianContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMedian(this);
}
void OParser::MedianContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMedian(this);
}

antlrcpp::Any OParser::MedianContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitMedian(this);
  else
    return visitor->visitChildren(this);
}
//----------------- RoundContext ------------------------------------------------------------------

OParser::ExprContext* OParser::RoundContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::RoundContext::RoundContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::RoundContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRound(this);
}
void OParser::RoundContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRound(this);
}

antlrcpp::Any OParser::RoundContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitRound(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NullContext ------------------------------------------------------------------

OParser::NullContext::NullContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::NullContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNull(this);
}
void OParser::NullContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNull(this);
}

antlrcpp::Any OParser::NullContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNull(this);
  else
    return visitor->visitChildren(this);
}
//----------------- If4Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::If4Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::If4Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::If4Context::If4Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::If4Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIf4(this);
}
void OParser::If4Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIf4(this);
}

antlrcpp::Any OParser::If4Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIf4(this);
  else
    return visitor->visitChildren(this);
}
//----------------- If3Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::If3Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::If3Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::If3Context::If3Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::If3Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIf3(this);
}
void OParser::If3Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIf3(this);
}

antlrcpp::Any OParser::If3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIf3(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IsNullContext ------------------------------------------------------------------

OParser::ExprContext* OParser::IsNullContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::IsNullContext::IsNullContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::IsNullContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIsNull(this);
}
void OParser::IsNullContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIsNull(this);
}

antlrcpp::Any OParser::IsNullContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIsNull(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AsinContext ------------------------------------------------------------------

OParser::ExprContext* OParser::AsinContext::expr() {
  return getRuleContext<OParser::ExprContext>(0);
}

OParser::AsinContext::AsinContext(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::AsinContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAsin(this);
}
void OParser::AsinContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAsin(this);
}

antlrcpp::Any OParser::AsinContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitAsin(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Exponential2Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::Exponential2Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::Exponential2Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::Exponential2Context::Exponential2Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::Exponential2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExponential2(this);
}
void OParser::Exponential2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExponential2(this);
}

antlrcpp::Any OParser::Exponential2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitExponential2(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Round3Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::Round3Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::Round3Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::Round3Context::Round3Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::Round3Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRound3(this);
}
void OParser::Round3Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRound3(this);
}

antlrcpp::Any OParser::Round3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitRound3(this);
  else
    return visitor->visitChildren(this);
}
//----------------- Round2Context ------------------------------------------------------------------

std::vector<OParser::ExprContext *> OParser::Round2Context::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::Round2Context::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}

OParser::Round2Context::Round2Context(FunctionContext *ctx) { copyFrom(ctx); }

void OParser::Round2Context::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRound2(this);
}
void OParser::Round2Context::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRound2(this);
}

antlrcpp::Any OParser::Round2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitRound2(this);
  else
    return visitor->visitChildren(this);
}
OParser::FunctionContext* OParser::function() {
  FunctionContext *_localctx = _tracker.createInstance<FunctionContext>(_ctx, getState());
  enterRule(_localctx, 8, OParser::RuleFunction);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(365);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::AbsContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(94);
      match(OParser::ABS);
      setState(95);
      match(OParser::OPENPAR);
      setState(96);
      expr(0);
      setState(97);
      match(OParser::CLOSEPAR);
      break;
    }

    case 2: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::AcosContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(99);
      match(OParser::ACOS);
      setState(100);
      match(OParser::OPENPAR);
      setState(101);
      expr(0);
      setState(102);
      match(OParser::CLOSEPAR);
      break;
    }

    case 3: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::AsinContext>(_localctx));
      enterOuterAlt(_localctx, 3);
      setState(104);
      match(OParser::ASIN);
      setState(105);
      match(OParser::OPENPAR);
      setState(106);
      expr(0);
      setState(107);
      match(OParser::CLOSEPAR);
      break;
    }

    case 4: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::AtanContext>(_localctx));
      enterOuterAlt(_localctx, 4);
      setState(109);
      match(OParser::ATAN);
      setState(110);
      match(OParser::OPENPAR);
      setState(111);
      expr(0);
      setState(112);
      match(OParser::CLOSEPAR);
      break;
    }

    case 5: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::Atan2Context>(_localctx));
      enterOuterAlt(_localctx, 5);
      setState(114);
      match(OParser::ATAN);
      setState(115);
      match(OParser::OPENPAR);
      setState(116);
      expr(0);
      setState(117);
      match(OParser::COMMA);
      setState(118);
      expr(0);
      setState(119);
      match(OParser::CLOSEPAR);
      break;
    }

    case 6: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::CosContext>(_localctx));
      enterOuterAlt(_localctx, 6);
      setState(121);
      match(OParser::COS);
      setState(122);
      match(OParser::OPENPAR);
      setState(123);
      expr(0);
      setState(124);
      match(OParser::CLOSEPAR);
      break;
    }

    case 7: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::CastToDoubleContext>(_localctx));
      enterOuterAlt(_localctx, 7);
      setState(126);
      match(OParser::DOUBLECAST);
      setState(127);
      match(OParser::OPENPAR);
      setState(128);
      expr(0);
      setState(129);
      match(OParser::CLOSEPAR);
      break;
    }

    case 8: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::EvalContext>(_localctx));
      enterOuterAlt(_localctx, 8);
      setState(131);
      match(OParser::EVAL);
      setState(132);
      match(OParser::OPENPAR);
      setState(133);
      expr_list();
      setState(134);
      expr(0);
      setState(135);
      match(OParser::CLOSEPAR);
      break;
    }

    case 9: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::ExponentialContext>(_localctx));
      enterOuterAlt(_localctx, 9);
      setState(137);
      match(OParser::EXPFUNC);
      setState(138);
      match(OParser::OPENPAR);
      setState(139);
      expr(0);
      setState(140);
      match(OParser::CLOSEPAR);
      break;
    }

    case 10: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::Exponential2Context>(_localctx));
      enterOuterAlt(_localctx, 10);
      setState(142);
      match(OParser::EXPFUNC);
      setState(143);
      match(OParser::OPENPAR);
      setState(144);
      expr(0);
      setState(145);
      match(OParser::COMMA);
      setState(146);
      expr(0);
      setState(147);
      match(OParser::CLOSEPAR);
      break;
    }

    case 11: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::CastToFloatContext>(_localctx));
      enterOuterAlt(_localctx, 11);
      setState(149);
      match(OParser::FLOATCAST);
      setState(150);
      match(OParser::OPENPAR);
      setState(151);
      expr(0);
      setState(152);
      match(OParser::CLOSEPAR);
      break;
    }

    case 12: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::IfContext>(_localctx));
      enterOuterAlt(_localctx, 12);
      setState(154);
      match(OParser::IF);
      setState(155);
      match(OParser::OPENPAR);
      setState(156);
      expr(0);
      setState(157);
      match(OParser::CLOSEPAR);
      break;
    }

    case 13: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::If2Context>(_localctx));
      enterOuterAlt(_localctx, 13);
      setState(159);
      match(OParser::IF);
      setState(160);
      match(OParser::OPENPAR);
      setState(161);
      expr(0);
      setState(162);
      match(OParser::COMMA);
      setState(163);
      expr(0);
      setState(164);
      match(OParser::CLOSEPAR);
      break;
    }

    case 14: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::If3Context>(_localctx));
      enterOuterAlt(_localctx, 14);
      setState(166);
      match(OParser::IF);
      setState(167);
      match(OParser::OPENPAR);
      setState(168);
      expr(0);
      setState(169);
      match(OParser::COMMA);
      setState(170);
      expr(0);
      setState(171);
      match(OParser::COMMA);
      setState(172);
      expr(0);
      setState(173);
      match(OParser::CLOSEPAR);
      break;
    }

    case 15: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::If4Context>(_localctx));
      enterOuterAlt(_localctx, 15);
      setState(175);
      match(OParser::IF);
      setState(176);
      match(OParser::OPENPAR);
      setState(177);
      expr(0);
      setState(178);
      match(OParser::COMMA);
      setState(179);
      expr(0);
      setState(180);
      match(OParser::COMMA);
      setState(181);
      expr(0);
      setState(182);
      match(OParser::COMMA);
      setState(183);
      expr(0);
      setState(184);
      match(OParser::CLOSEPAR);
      break;
    }

    case 16: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::CastToIntContext>(_localctx));
      enterOuterAlt(_localctx, 16);
      setState(186);
      match(OParser::INTCAST);
      setState(187);
      match(OParser::OPENPAR);
      setState(188);
      expr(0);
      setState(189);
      match(OParser::CLOSEPAR);
      break;
    }

    case 17: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::IsNullContext>(_localctx));
      enterOuterAlt(_localctx, 17);
      setState(191);
      match(OParser::ISNULL);
      setState(192);
      match(OParser::OPENPAR);
      setState(193);
      expr(0);
      setState(194);
      match(OParser::CLOSEPAR);
      break;
    }

    case 18: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::LogContext>(_localctx));
      enterOuterAlt(_localctx, 18);
      setState(196);
      match(OParser::LOG);
      setState(197);
      match(OParser::OPENPAR);
      setState(198);
      expr(0);
      setState(199);
      match(OParser::CLOSEPAR);
      break;
    }

    case 19: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::Log2Context>(_localctx));
      enterOuterAlt(_localctx, 19);
      setState(201);
      match(OParser::LOG);
      setState(202);
      match(OParser::OPENPAR);
      setState(203);
      expr(0);
      setState(204);
      match(OParser::COMMA);
      setState(205);
      expr(0);
      setState(206);
      match(OParser::CLOSEPAR);
      break;
    }

    case 20: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::MaxContext>(_localctx));
      enterOuterAlt(_localctx, 20);
      setState(208);
      match(OParser::MAX);
      setState(209);
      match(OParser::OPENPAR);
      setState(210);
      expr(0);
      setState(213); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(211);
        match(OParser::COMMA);
        setState(212);
        expr(0);
        setState(215); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(217);
      match(OParser::CLOSEPAR);
      break;
    }

    case 21: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::MedianContext>(_localctx));
      enterOuterAlt(_localctx, 21);
      setState(219);
      match(OParser::MEDIAN);
      setState(220);
      match(OParser::OPENPAR);
      setState(221);
      expr(0);
      setState(224); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(222);
        match(OParser::COMMA);
        setState(223);
        expr(0);
        setState(226); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(228);
      match(OParser::CLOSEPAR);
      break;
    }

    case 22: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::MinContext>(_localctx));
      enterOuterAlt(_localctx, 22);
      setState(230);
      match(OParser::MIN);
      setState(231);
      match(OParser::OPENPAR);
      setState(232);
      expr(0);
      setState(235); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(233);
        match(OParser::COMMA);
        setState(234);
        expr(0);
        setState(237); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(239);
      match(OParser::CLOSEPAR);
      break;
    }

    case 23: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::MathModeContext>(_localctx));
      enterOuterAlt(_localctx, 23);
      setState(241);
      match(OParser::MODE);
      setState(242);
      match(OParser::OPENPAR);
      setState(243);
      expr(0);
      setState(246); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(244);
        match(OParser::COMMA);
        setState(245);
        expr(0);
        setState(248); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(250);
      match(OParser::CLOSEPAR);
      break;
    }

    case 24: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::NmaxContext>(_localctx));
      enterOuterAlt(_localctx, 24);
      setState(252);
      match(OParser::NMAX);
      setState(253);
      match(OParser::OPENPAR);
      setState(254);
      expr(0);
      setState(257); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(255);
        match(OParser::COMMA);
        setState(256);
        expr(0);
        setState(259); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(261);
      match(OParser::CLOSEPAR);
      break;
    }

    case 25: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::NmedianContext>(_localctx));
      enterOuterAlt(_localctx, 25);
      setState(263);
      match(OParser::NMEDIAN);
      setState(264);
      match(OParser::OPENPAR);
      setState(265);
      expr(0);
      setState(268); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(266);
        match(OParser::COMMA);
        setState(267);
        expr(0);
        setState(270); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(272);
      match(OParser::CLOSEPAR);
      break;
    }

    case 26: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::NminContext>(_localctx));
      enterOuterAlt(_localctx, 26);
      setState(274);
      match(OParser::NMIN);
      setState(275);
      match(OParser::OPENPAR);
      setState(276);
      expr(0);
      setState(279); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(277);
        match(OParser::COMMA);
        setState(278);
        expr(0);
        setState(281); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(283);
      match(OParser::CLOSEPAR);
      break;
    }

    case 27: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::NmodeContext>(_localctx));
      enterOuterAlt(_localctx, 27);
      setState(285);
      match(OParser::NMODE);
      setState(286);
      match(OParser::OPENPAR);
      setState(287);
      expr(0);
      setState(290); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(288);
        match(OParser::COMMA);
        setState(289);
        expr(0);
        setState(292); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == OParser::COMMA);
      setState(294);
      match(OParser::CLOSEPAR);
      break;
    }

    case 28: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::NotContext>(_localctx));
      enterOuterAlt(_localctx, 28);
      setState(296);
      match(OParser::NOT);
      setState(297);
      match(OParser::OPENPAR);
      setState(298);
      expr(0);
      setState(299);
      match(OParser::CLOSEPAR);
      break;
    }

    case 29: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::Exponential2Context>(_localctx));
      enterOuterAlt(_localctx, 29);
      setState(301);
      match(OParser::POW);
      setState(302);
      match(OParser::OPENPAR);
      setState(303);
      expr(0);
      setState(304);
      match(OParser::COMMA);
      setState(305);
      expr(0);
      setState(306);
      match(OParser::CLOSEPAR);
      break;
    }

    case 30: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::RandContext>(_localctx));
      enterOuterAlt(_localctx, 30);
      setState(308);
      match(OParser::RAND);
      setState(309);
      match(OParser::OPENPAR);
      setState(310);
      expr(0);
      setState(311);
      match(OParser::COMMA);
      setState(312);
      expr(0);
      setState(313);
      match(OParser::CLOSEPAR);
      break;
    }

    case 31: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::RoundContext>(_localctx));
      enterOuterAlt(_localctx, 31);
      setState(315);
      match(OParser::ROUND);
      setState(316);
      match(OParser::OPENPAR);
      setState(317);
      expr(0);
      setState(318);
      match(OParser::CLOSEPAR);
      break;
    }

    case 32: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::Round2Context>(_localctx));
      enterOuterAlt(_localctx, 32);
      setState(320);
      match(OParser::ROUND);
      setState(321);
      match(OParser::OPENPAR);
      setState(322);
      expr(0);
      setState(323);
      match(OParser::COMMA);
      setState(324);
      expr(0);
      setState(325);
      match(OParser::CLOSEPAR);
      break;
    }

    case 33: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::Round3Context>(_localctx));
      enterOuterAlt(_localctx, 33);
      setState(327);
      match(OParser::ROUND);
      setState(328);
      match(OParser::OPENPAR);
      setState(329);
      expr(0);
      setState(330);
      match(OParser::COMMA);
      setState(331);
      expr(0);
      setState(332);
      match(OParser::COMMA);
      setState(333);
      expr(0);
      setState(334);
      match(OParser::CLOSEPAR);
      break;
    }

    case 34: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::SinContext>(_localctx));
      enterOuterAlt(_localctx, 34);
      setState(336);
      match(OParser::SIN);
      setState(337);
      match(OParser::OPENPAR);
      setState(338);
      expr(0);
      setState(339);
      match(OParser::CLOSEPAR);
      break;
    }

    case 35: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::SqrtContext>(_localctx));
      enterOuterAlt(_localctx, 35);
      setState(341);
      match(OParser::SQRT);
      setState(342);
      match(OParser::OPENPAR);
      setState(343);
      expr(0);
      setState(344);
      match(OParser::CLOSEPAR);
      break;
    }

    case 36: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::TanContext>(_localctx));
      enterOuterAlt(_localctx, 36);
      setState(346);
      match(OParser::TAN);
      setState(347);
      match(OParser::OPENPAR);
      setState(348);
      expr(0);
      setState(349);
      match(OParser::CLOSEPAR);
      break;
    }

    case 37: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::XorContext>(_localctx));
      enterOuterAlt(_localctx, 37);
      setState(351);
      match(OParser::XOR);
      setState(352);
      match(OParser::OPENPAR);
      setState(353);
      expr(0);
      setState(354);
      match(OParser::CLOSEPAR);
      break;
    }

    case 38: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::NullContext>(_localctx));
      enterOuterAlt(_localctx, 38);
      setState(356);
      match(OParser::NULLV);
      setState(357);
      match(OParser::OPENPAR);
      setState(358);
      match(OParser::CLOSEPAR);
      break;
    }

    case 39: {
      _localctx = dynamic_cast<FunctionContext *>(_tracker.createInstance<OParser::SqlContext>(_localctx));
      enterOuterAlt(_localctx, 39);
      setState(359);
      match(OParser::SQL);
      setState(360);
      match(OParser::OPENPAR);
      setState(361);
      match(OParser::NAME);
      setState(362);
      match(OParser::COMMA);
      setState(363);
      match(OParser::NAME);
      setState(364);
      match(OParser::CLOSEPAR);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Expr_listContext ------------------------------------------------------------------

OParser::Expr_listContext::Expr_listContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<OParser::ExprContext *> OParser::Expr_listContext::expr() {
  return getRuleContexts<OParser::ExprContext>();
}

OParser::ExprContext* OParser::Expr_listContext::expr(size_t i) {
  return getRuleContext<OParser::ExprContext>(i);
}


size_t OParser::Expr_listContext::getRuleIndex() const {
  return OParser::RuleExpr_list;
}

void OParser::Expr_listContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExpr_list(this);
}

void OParser::Expr_listContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExpr_list(this);
}


antlrcpp::Any OParser::Expr_listContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitExpr_list(this);
  else
    return visitor->visitChildren(this);
}

OParser::Expr_listContext* OParser::expr_list() {
  Expr_listContext *_localctx = _tracker.createInstance<Expr_listContext>(_ctx, getState());
  enterRule(_localctx, 10, OParser::RuleExpr_list);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(367);
    expr(0);
    setState(372);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == OParser::COMMA) {
      setState(368);
      match(OParser::COMMA);
      setState(369);
      expr(0);
      setState(374);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstantContext ------------------------------------------------------------------

OParser::ConstantContext::ConstantContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OParser::ConstantContext::getRuleIndex() const {
  return OParser::RuleConstant;
}

void OParser::ConstantContext::copyFrom(ConstantContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- FloatConstContext ------------------------------------------------------------------

tree::TerminalNode* OParser::FloatConstContext::FLOAT() {
  return getToken(OParser::FLOAT, 0);
}

OParser::FloatConstContext::FloatConstContext(ConstantContext *ctx) { copyFrom(ctx); }

void OParser::FloatConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloatConst(this);
}
void OParser::FloatConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloatConst(this);
}

antlrcpp::Any OParser::FloatConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitFloatConst(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IdentifierContext ------------------------------------------------------------------

tree::TerminalNode* OParser::IdentifierContext::NAME() {
  return getToken(OParser::NAME, 0);
}

OParser::IdentifierContext::IdentifierContext(ConstantContext *ctx) { copyFrom(ctx); }

void OParser::IdentifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIdentifier(this);
}
void OParser::IdentifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIdentifier(this);
}

antlrcpp::Any OParser::IdentifierContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIdentifier(this);
  else
    return visitor->visitChildren(this);
}
//----------------- DoubleConstContext ------------------------------------------------------------------

tree::TerminalNode* OParser::DoubleConstContext::DOUBLE() {
  return getToken(OParser::DOUBLE, 0);
}

OParser::DoubleConstContext::DoubleConstContext(ConstantContext *ctx) { copyFrom(ctx); }

void OParser::DoubleConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDoubleConst(this);
}
void OParser::DoubleConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDoubleConst(this);
}

antlrcpp::Any OParser::DoubleConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitDoubleConst(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IntConstContext ------------------------------------------------------------------

tree::TerminalNode* OParser::IntConstContext::INT() {
  return getToken(OParser::INT, 0);
}

OParser::IntConstContext::IntConstContext(ConstantContext *ctx) { copyFrom(ctx); }

void OParser::IntConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIntConst(this);
}
void OParser::IntConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIntConst(this);
}

antlrcpp::Any OParser::IntConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitIntConst(this);
  else
    return visitor->visitChildren(this);
}
//----------------- StringConstContext ------------------------------------------------------------------

tree::TerminalNode* OParser::StringConstContext::STRING() {
  return getToken(OParser::STRING, 0);
}

OParser::StringConstContext::StringConstContext(ConstantContext *ctx) { copyFrom(ctx); }

void OParser::StringConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStringConst(this);
}
void OParser::StringConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStringConst(this);
}

antlrcpp::Any OParser::StringConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitStringConst(this);
  else
    return visitor->visitChildren(this);
}
OParser::ConstantContext* OParser::constant() {
  ConstantContext *_localctx = _tracker.createInstance<ConstantContext>(_ctx, getState());
  enterRule(_localctx, 12, OParser::RuleConstant);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(380);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OParser::INT: {
        _localctx = dynamic_cast<ConstantContext *>(_tracker.createInstance<OParser::IntConstContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(375);
        match(OParser::INT);
        break;
      }

      case OParser::FLOAT: {
        _localctx = dynamic_cast<ConstantContext *>(_tracker.createInstance<OParser::FloatConstContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(376);
        match(OParser::FLOAT);
        break;
      }

      case OParser::DOUBLE: {
        _localctx = dynamic_cast<ConstantContext *>(_tracker.createInstance<OParser::DoubleConstContext>(_localctx));
        enterOuterAlt(_localctx, 3);
        setState(377);
        match(OParser::DOUBLE);
        break;
      }

      case OParser::NAME: {
        _localctx = dynamic_cast<ConstantContext *>(_tracker.createInstance<OParser::IdentifierContext>(_localctx));
        enterOuterAlt(_localctx, 4);
        setState(378);
        match(OParser::NAME);
        break;
      }

      case OParser::STRING: {
        _localctx = dynamic_cast<ConstantContext *>(_tracker.createInstance<OParser::StringConstContext>(_localctx));
        enterOuterAlt(_localctx, 5);
        setState(379);
        match(OParser::STRING);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnitaryOpContext ------------------------------------------------------------------

OParser::UnitaryOpContext::UnitaryOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t OParser::UnitaryOpContext::getRuleIndex() const {
  return OParser::RuleUnitaryOp;
}

void OParser::UnitaryOpContext::copyFrom(UnitaryOpContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- NotOpContext ------------------------------------------------------------------

OParser::NotOpContext::NotOpContext(UnitaryOpContext *ctx) { copyFrom(ctx); }

void OParser::NotOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNotOp(this);
}
void OParser::NotOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNotOp(this);
}

antlrcpp::Any OParser::NotOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNotOp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NegOpContext ------------------------------------------------------------------

OParser::NegOpContext::NegOpContext(UnitaryOpContext *ctx) { copyFrom(ctx); }

void OParser::NegOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNegOp(this);
}
void OParser::NegOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNegOp(this);
}

antlrcpp::Any OParser::NegOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitNegOp(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CompOpContext ------------------------------------------------------------------

OParser::CompOpContext::CompOpContext(UnitaryOpContext *ctx) { copyFrom(ctx); }

void OParser::CompOpContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCompOp(this);
}
void OParser::CompOpContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<OParserListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCompOp(this);
}

antlrcpp::Any OParser::CompOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<OParserVisitor*>(visitor))
    return parserVisitor->visitCompOp(this);
  else
    return visitor->visitChildren(this);
}
OParser::UnitaryOpContext* OParser::unitaryOp() {
  UnitaryOpContext *_localctx = _tracker.createInstance<UnitaryOpContext>(_ctx, getState());
  enterRule(_localctx, 14, OParser::RuleUnitaryOp);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(385);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case OParser::NEGSUB: {
        _localctx = dynamic_cast<UnitaryOpContext *>(_tracker.createInstance<OParser::NegOpContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(382);
        match(OParser::NEGSUB);
        break;
      }

      case OParser::COMPLEMENT: {
        _localctx = dynamic_cast<UnitaryOpContext *>(_tracker.createInstance<OParser::CompOpContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(383);
        match(OParser::COMPLEMENT);
        break;
      }

      case OParser::LNOT: {
        _localctx = dynamic_cast<UnitaryOpContext *>(_tracker.createInstance<OParser::NotOpContext>(_localctx));
        enterOuterAlt(_localctx, 3);
        setState(384);
        match(OParser::LNOT);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool OParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 3: return exprSempred(dynamic_cast<ExprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool OParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 11);
    case 1: return precpred(_ctx, 10);
    case 2: return precpred(_ctx, 9);
    case 3: return precpred(_ctx, 8);
    case 4: return precpred(_ctx, 7);
    case 5: return precpred(_ctx, 6);
    case 6: return precpred(_ctx, 5);
    case 7: return precpred(_ctx, 4);
    case 8: return precpred(_ctx, 3);
    case 9: return precpred(_ctx, 2);
    case 10: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> OParser::_decisionToDFA;
atn::PredictionContextCache OParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN OParser::_atn;
std::vector<uint16_t> OParser::_serializedATN;

std::vector<std::string> OParser::_ruleNames = {
  "program", "defs", "def", "expr", "function", "expr_list", "constant", 
  "unitaryOp"
};

std::vector<std::string> OParser::_literalNames = {
  "", "", "'#'", "'('", "')'", "'='", "':'", "';'", "','", "'-'", "'~'", 
  "'^'", "'%'", "'/'", "'*'", "'+'", "'<<'", "'>>'", "'>>>'", "'>'", "'>='", 
  "'<'", "'<='", "'=='", "'!='", "'&'", "'|'", "'&&'", "'&&&'", "'||'", 
  "'|||'", "'!'", "'?'", "'abs'", "'acos'", "'asin'", "'atan'", "'cos'", 
  "'double'", "'eval'", "'exp'", "'float'", "'if'", "'int'", "'isnull'", 
  "'log'", "'max'", "'median'", "'min'", "'mode'", "'nmax'", "'nmedian'", 
  "'nmin'", "'nmode'", "'not'", "'pow'", "'rand'", "'round'", "'sin'", "'sqrt'", 
  "'tan'", "'xor'", "'null'", "'sql'"
};

std::vector<std::string> OParser::_symbolicNames = {
  "", "BKNL", "SHARP", "OPENPAR", "CLOSEPAR", "ASSIGN", "COLON", "SEMICOLON", 
  "COMMA", "NEGSUB", "COMPLEMENT", "EXPONENT", "MODULUS", "DIV", "MULT", 
  "ADD", "LSHIFT", "RSHIFT", "RSHIFTU", "GREATER", "GREATEREQUAL", "LESS", 
  "LESSEQUEAL", "EQUAL", "NEQUAL", "BITAND", "BITOR", "LAND", "LANDONE", 
  "LOR", "LORONE", "LNOT", "COND", "ABS", "ACOS", "ASIN", "ATAN", "COS", 
  "DOUBLECAST", "EVAL", "EXPFUNC", "FLOATCAST", "IF", "INTCAST", "ISNULL", 
  "LOG", "MAX", "MEDIAN", "MIN", "MODE", "NMAX", "NMEDIAN", "NMIN", "NMODE", 
  "NOT", "POW", "RAND", "ROUND", "SIN", "SQRT", "TAN", "XOR", "NULLV", "SQL", 
  "FLOAT", "DOUBLE", "INT", "NAME", "STRING", "WS"
};

dfa::Vocabulary OParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> OParser::_tokenNames;

OParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x47, 0x186, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
    0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 
    0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x18, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x1b, 
    0xb, 0x3, 0x3, 0x3, 0x5, 0x3, 0x1e, 0xa, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 
    0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x26, 0xa, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x5, 0x4, 0x2a, 0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x5, 
    0x5, 0x36, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x7, 0x5, 0x5c, 0xa, 0x5, 0xc, 0x5, 0xe, 
    0x5, 0x5f, 0xb, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x6, 0x6, 0xd8, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0xd9, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x6, 0x6, 0xe3, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0xe4, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x6, 0x6, 0xee, 0xa, 
    0x6, 0xd, 0x6, 0xe, 0x6, 0xef, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x6, 0x6, 0xf9, 0xa, 0x6, 0xd, 0x6, 0xe, 
    0x6, 0xfa, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x6, 0x6, 0x104, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0x105, 0x3, 
    0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x6, 
    0x6, 0x10f, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0x110, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x6, 0x6, 0x11a, 0xa, 
    0x6, 0xd, 0x6, 0xe, 0x6, 0x11b, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x6, 0x6, 0x125, 0xa, 0x6, 0xd, 0x6, 0xe, 
    0x6, 0x126, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x5, 0x6, 0x170, 0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x7, 0x7, 0x175, 0xa, 0x7, 0xc, 0x7, 0xe, 0x7, 0x178, 0xb, 0x7, 
    0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x5, 0x8, 0x17f, 0xa, 
    0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 0x9, 0x184, 0xa, 0x9, 0x3, 0x9, 
    0x2, 0x3, 0x8, 0xa, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x2, 0x9, 
    0x3, 0x2, 0xe, 0x10, 0x4, 0x2, 0xb, 0xb, 0x11, 0x11, 0x3, 0x2, 0x12, 
    0x14, 0x3, 0x2, 0x15, 0x18, 0x3, 0x2, 0x19, 0x1a, 0x3, 0x2, 0x1d, 0x1e, 
    0x3, 0x2, 0x1f, 0x20, 0x2, 0x1c4, 0x2, 0x12, 0x3, 0x2, 0x2, 0x2, 0x4, 
    0x14, 0x3, 0x2, 0x2, 0x2, 0x6, 0x29, 0x3, 0x2, 0x2, 0x2, 0x8, 0x35, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x16f, 0x3, 0x2, 0x2, 0x2, 0xc, 0x171, 0x3, 
    0x2, 0x2, 0x2, 0xe, 0x17e, 0x3, 0x2, 0x2, 0x2, 0x10, 0x183, 0x3, 0x2, 
    0x2, 0x2, 0x12, 0x13, 0x5, 0x4, 0x3, 0x2, 0x13, 0x3, 0x3, 0x2, 0x2, 
    0x2, 0x14, 0x19, 0x5, 0x6, 0x4, 0x2, 0x15, 0x16, 0x7, 0x9, 0x2, 0x2, 
    0x16, 0x18, 0x5, 0x6, 0x4, 0x2, 0x17, 0x15, 0x3, 0x2, 0x2, 0x2, 0x18, 
    0x1b, 0x3, 0x2, 0x2, 0x2, 0x19, 0x17, 0x3, 0x2, 0x2, 0x2, 0x19, 0x1a, 
    0x3, 0x2, 0x2, 0x2, 0x1a, 0x1d, 0x3, 0x2, 0x2, 0x2, 0x1b, 0x19, 0x3, 
    0x2, 0x2, 0x2, 0x1c, 0x1e, 0x7, 0x9, 0x2, 0x2, 0x1d, 0x1c, 0x3, 0x2, 
    0x2, 0x2, 0x1d, 0x1e, 0x3, 0x2, 0x2, 0x2, 0x1e, 0x5, 0x3, 0x2, 0x2, 
    0x2, 0x1f, 0x20, 0x7, 0x46, 0x2, 0x2, 0x20, 0x21, 0x7, 0x7, 0x2, 0x2, 
    0x21, 0x2a, 0x5, 0x8, 0x5, 0x2, 0x22, 0x25, 0x7, 0x45, 0x2, 0x2, 0x23, 
    0x24, 0x7, 0x4, 0x2, 0x2, 0x24, 0x26, 0x7, 0x45, 0x2, 0x2, 0x25, 0x23, 
    0x3, 0x2, 0x2, 0x2, 0x25, 0x26, 0x3, 0x2, 0x2, 0x2, 0x26, 0x27, 0x3, 
    0x2, 0x2, 0x2, 0x27, 0x28, 0x7, 0x7, 0x2, 0x2, 0x28, 0x2a, 0x5, 0x8, 
    0x5, 0x2, 0x29, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x29, 0x22, 0x3, 0x2, 0x2, 
    0x2, 0x2a, 0x7, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x2c, 0x8, 0x5, 0x1, 0x2, 
    0x2c, 0x2d, 0x7, 0x5, 0x2, 0x2, 0x2d, 0x2e, 0x5, 0x8, 0x5, 0x2, 0x2e, 
    0x2f, 0x7, 0x6, 0x2, 0x2, 0x2f, 0x36, 0x3, 0x2, 0x2, 0x2, 0x30, 0x36, 
    0x5, 0xa, 0x6, 0x2, 0x31, 0x36, 0x5, 0xe, 0x8, 0x2, 0x32, 0x33, 0x5, 
    0x10, 0x9, 0x2, 0x33, 0x34, 0x5, 0x8, 0x5, 0xe, 0x34, 0x36, 0x3, 0x2, 
    0x2, 0x2, 0x35, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x35, 0x30, 0x3, 0x2, 0x2, 
    0x2, 0x35, 0x31, 0x3, 0x2, 0x2, 0x2, 0x35, 0x32, 0x3, 0x2, 0x2, 0x2, 
    0x36, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x37, 0x38, 0xc, 0xd, 0x2, 0x2, 0x38, 
    0x39, 0x7, 0xd, 0x2, 0x2, 0x39, 0x5c, 0x5, 0x8, 0x5, 0xe, 0x3a, 0x3b, 
    0xc, 0xc, 0x2, 0x2, 0x3b, 0x3c, 0x9, 0x2, 0x2, 0x2, 0x3c, 0x5c, 0x5, 
    0x8, 0x5, 0xd, 0x3d, 0x3e, 0xc, 0xb, 0x2, 0x2, 0x3e, 0x3f, 0x9, 0x3, 
    0x2, 0x2, 0x3f, 0x5c, 0x5, 0x8, 0x5, 0xc, 0x40, 0x41, 0xc, 0xa, 0x2, 
    0x2, 0x41, 0x42, 0x9, 0x4, 0x2, 0x2, 0x42, 0x5c, 0x5, 0x8, 0x5, 0xb, 
    0x43, 0x44, 0xc, 0x9, 0x2, 0x2, 0x44, 0x45, 0x9, 0x5, 0x2, 0x2, 0x45, 
    0x5c, 0x5, 0x8, 0x5, 0xa, 0x46, 0x47, 0xc, 0x8, 0x2, 0x2, 0x47, 0x48, 
    0x9, 0x6, 0x2, 0x2, 0x48, 0x5c, 0x5, 0x8, 0x5, 0x9, 0x49, 0x4a, 0xc, 
    0x7, 0x2, 0x2, 0x4a, 0x4b, 0x7, 0x1b, 0x2, 0x2, 0x4b, 0x5c, 0x5, 0x8, 
    0x5, 0x8, 0x4c, 0x4d, 0xc, 0x6, 0x2, 0x2, 0x4d, 0x4e, 0x7, 0x1c, 0x2, 
    0x2, 0x4e, 0x5c, 0x5, 0x8, 0x5, 0x7, 0x4f, 0x50, 0xc, 0x5, 0x2, 0x2, 
    0x50, 0x51, 0x9, 0x7, 0x2, 0x2, 0x51, 0x5c, 0x5, 0x8, 0x5, 0x6, 0x52, 
    0x53, 0xc, 0x4, 0x2, 0x2, 0x53, 0x54, 0x9, 0x8, 0x2, 0x2, 0x54, 0x5c, 
    0x5, 0x8, 0x5, 0x5, 0x55, 0x56, 0xc, 0x3, 0x2, 0x2, 0x56, 0x57, 0x7, 
    0x22, 0x2, 0x2, 0x57, 0x58, 0x5, 0x8, 0x5, 0x2, 0x58, 0x59, 0x7, 0x8, 
    0x2, 0x2, 0x59, 0x5a, 0x5, 0x8, 0x5, 0x4, 0x5a, 0x5c, 0x3, 0x2, 0x2, 
    0x2, 0x5b, 0x37, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x3a, 0x3, 0x2, 0x2, 0x2, 
    0x5b, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x40, 0x3, 0x2, 0x2, 0x2, 0x5b, 
    0x43, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x46, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x49, 
    0x3, 0x2, 0x2, 0x2, 0x5b, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x4f, 0x3, 
    0x2, 0x2, 0x2, 0x5b, 0x52, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x55, 0x3, 0x2, 
    0x2, 0x2, 0x5c, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x5d, 0x5b, 0x3, 0x2, 0x2, 
    0x2, 0x5d, 0x5e, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x9, 0x3, 0x2, 0x2, 0x2, 
    0x5f, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x60, 0x61, 0x7, 0x23, 0x2, 0x2, 0x61, 
    0x62, 0x7, 0x5, 0x2, 0x2, 0x62, 0x63, 0x5, 0x8, 0x5, 0x2, 0x63, 0x64, 
    0x7, 0x6, 0x2, 0x2, 0x64, 0x170, 0x3, 0x2, 0x2, 0x2, 0x65, 0x66, 0x7, 
    0x24, 0x2, 0x2, 0x66, 0x67, 0x7, 0x5, 0x2, 0x2, 0x67, 0x68, 0x5, 0x8, 
    0x5, 0x2, 0x68, 0x69, 0x7, 0x6, 0x2, 0x2, 0x69, 0x170, 0x3, 0x2, 0x2, 
    0x2, 0x6a, 0x6b, 0x7, 0x25, 0x2, 0x2, 0x6b, 0x6c, 0x7, 0x5, 0x2, 0x2, 
    0x6c, 0x6d, 0x5, 0x8, 0x5, 0x2, 0x6d, 0x6e, 0x7, 0x6, 0x2, 0x2, 0x6e, 
    0x170, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x70, 0x7, 0x26, 0x2, 0x2, 0x70, 0x71, 
    0x7, 0x5, 0x2, 0x2, 0x71, 0x72, 0x5, 0x8, 0x5, 0x2, 0x72, 0x73, 0x7, 
    0x6, 0x2, 0x2, 0x73, 0x170, 0x3, 0x2, 0x2, 0x2, 0x74, 0x75, 0x7, 0x26, 
    0x2, 0x2, 0x75, 0x76, 0x7, 0x5, 0x2, 0x2, 0x76, 0x77, 0x5, 0x8, 0x5, 
    0x2, 0x77, 0x78, 0x7, 0xa, 0x2, 0x2, 0x78, 0x79, 0x5, 0x8, 0x5, 0x2, 
    0x79, 0x7a, 0x7, 0x6, 0x2, 0x2, 0x7a, 0x170, 0x3, 0x2, 0x2, 0x2, 0x7b, 
    0x7c, 0x7, 0x27, 0x2, 0x2, 0x7c, 0x7d, 0x7, 0x5, 0x2, 0x2, 0x7d, 0x7e, 
    0x5, 0x8, 0x5, 0x2, 0x7e, 0x7f, 0x7, 0x6, 0x2, 0x2, 0x7f, 0x170, 0x3, 
    0x2, 0x2, 0x2, 0x80, 0x81, 0x7, 0x28, 0x2, 0x2, 0x81, 0x82, 0x7, 0x5, 
    0x2, 0x2, 0x82, 0x83, 0x5, 0x8, 0x5, 0x2, 0x83, 0x84, 0x7, 0x6, 0x2, 
    0x2, 0x84, 0x170, 0x3, 0x2, 0x2, 0x2, 0x85, 0x86, 0x7, 0x29, 0x2, 0x2, 
    0x86, 0x87, 0x7, 0x5, 0x2, 0x2, 0x87, 0x88, 0x5, 0xc, 0x7, 0x2, 0x88, 
    0x89, 0x5, 0x8, 0x5, 0x2, 0x89, 0x8a, 0x7, 0x6, 0x2, 0x2, 0x8a, 0x170, 
    0x3, 0x2, 0x2, 0x2, 0x8b, 0x8c, 0x7, 0x2a, 0x2, 0x2, 0x8c, 0x8d, 0x7, 
    0x5, 0x2, 0x2, 0x8d, 0x8e, 0x5, 0x8, 0x5, 0x2, 0x8e, 0x8f, 0x7, 0x6, 
    0x2, 0x2, 0x8f, 0x170, 0x3, 0x2, 0x2, 0x2, 0x90, 0x91, 0x7, 0x2a, 0x2, 
    0x2, 0x91, 0x92, 0x7, 0x5, 0x2, 0x2, 0x92, 0x93, 0x5, 0x8, 0x5, 0x2, 
    0x93, 0x94, 0x7, 0xa, 0x2, 0x2, 0x94, 0x95, 0x5, 0x8, 0x5, 0x2, 0x95, 
    0x96, 0x7, 0x6, 0x2, 0x2, 0x96, 0x170, 0x3, 0x2, 0x2, 0x2, 0x97, 0x98, 
    0x7, 0x2b, 0x2, 0x2, 0x98, 0x99, 0x7, 0x5, 0x2, 0x2, 0x99, 0x9a, 0x5, 
    0x8, 0x5, 0x2, 0x9a, 0x9b, 0x7, 0x6, 0x2, 0x2, 0x9b, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x9c, 0x9d, 0x7, 0x2c, 0x2, 0x2, 0x9d, 0x9e, 0x7, 0x5, 0x2, 
    0x2, 0x9e, 0x9f, 0x5, 0x8, 0x5, 0x2, 0x9f, 0xa0, 0x7, 0x6, 0x2, 0x2, 
    0xa0, 0x170, 0x3, 0x2, 0x2, 0x2, 0xa1, 0xa2, 0x7, 0x2c, 0x2, 0x2, 0xa2, 
    0xa3, 0x7, 0x5, 0x2, 0x2, 0xa3, 0xa4, 0x5, 0x8, 0x5, 0x2, 0xa4, 0xa5, 
    0x7, 0xa, 0x2, 0x2, 0xa5, 0xa6, 0x5, 0x8, 0x5, 0x2, 0xa6, 0xa7, 0x7, 
    0x6, 0x2, 0x2, 0xa7, 0x170, 0x3, 0x2, 0x2, 0x2, 0xa8, 0xa9, 0x7, 0x2c, 
    0x2, 0x2, 0xa9, 0xaa, 0x7, 0x5, 0x2, 0x2, 0xaa, 0xab, 0x5, 0x8, 0x5, 
    0x2, 0xab, 0xac, 0x7, 0xa, 0x2, 0x2, 0xac, 0xad, 0x5, 0x8, 0x5, 0x2, 
    0xad, 0xae, 0x7, 0xa, 0x2, 0x2, 0xae, 0xaf, 0x5, 0x8, 0x5, 0x2, 0xaf, 
    0xb0, 0x7, 0x6, 0x2, 0x2, 0xb0, 0x170, 0x3, 0x2, 0x2, 0x2, 0xb1, 0xb2, 
    0x7, 0x2c, 0x2, 0x2, 0xb2, 0xb3, 0x7, 0x5, 0x2, 0x2, 0xb3, 0xb4, 0x5, 
    0x8, 0x5, 0x2, 0xb4, 0xb5, 0x7, 0xa, 0x2, 0x2, 0xb5, 0xb6, 0x5, 0x8, 
    0x5, 0x2, 0xb6, 0xb7, 0x7, 0xa, 0x2, 0x2, 0xb7, 0xb8, 0x5, 0x8, 0x5, 
    0x2, 0xb8, 0xb9, 0x7, 0xa, 0x2, 0x2, 0xb9, 0xba, 0x5, 0x8, 0x5, 0x2, 
    0xba, 0xbb, 0x7, 0x6, 0x2, 0x2, 0xbb, 0x170, 0x3, 0x2, 0x2, 0x2, 0xbc, 
    0xbd, 0x7, 0x2d, 0x2, 0x2, 0xbd, 0xbe, 0x7, 0x5, 0x2, 0x2, 0xbe, 0xbf, 
    0x5, 0x8, 0x5, 0x2, 0xbf, 0xc0, 0x7, 0x6, 0x2, 0x2, 0xc0, 0x170, 0x3, 
    0x2, 0x2, 0x2, 0xc1, 0xc2, 0x7, 0x2e, 0x2, 0x2, 0xc2, 0xc3, 0x7, 0x5, 
    0x2, 0x2, 0xc3, 0xc4, 0x5, 0x8, 0x5, 0x2, 0xc4, 0xc5, 0x7, 0x6, 0x2, 
    0x2, 0xc5, 0x170, 0x3, 0x2, 0x2, 0x2, 0xc6, 0xc7, 0x7, 0x2f, 0x2, 0x2, 
    0xc7, 0xc8, 0x7, 0x5, 0x2, 0x2, 0xc8, 0xc9, 0x5, 0x8, 0x5, 0x2, 0xc9, 
    0xca, 0x7, 0x6, 0x2, 0x2, 0xca, 0x170, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xcc, 
    0x7, 0x2f, 0x2, 0x2, 0xcc, 0xcd, 0x7, 0x5, 0x2, 0x2, 0xcd, 0xce, 0x5, 
    0x8, 0x5, 0x2, 0xce, 0xcf, 0x7, 0xa, 0x2, 0x2, 0xcf, 0xd0, 0x5, 0x8, 
    0x5, 0x2, 0xd0, 0xd1, 0x7, 0x6, 0x2, 0x2, 0xd1, 0x170, 0x3, 0x2, 0x2, 
    0x2, 0xd2, 0xd3, 0x7, 0x30, 0x2, 0x2, 0xd3, 0xd4, 0x7, 0x5, 0x2, 0x2, 
    0xd4, 0xd7, 0x5, 0x8, 0x5, 0x2, 0xd5, 0xd6, 0x7, 0xa, 0x2, 0x2, 0xd6, 
    0xd8, 0x5, 0x8, 0x5, 0x2, 0xd7, 0xd5, 0x3, 0x2, 0x2, 0x2, 0xd8, 0xd9, 
    0x3, 0x2, 0x2, 0x2, 0xd9, 0xd7, 0x3, 0x2, 0x2, 0x2, 0xd9, 0xda, 0x3, 
    0x2, 0x2, 0x2, 0xda, 0xdb, 0x3, 0x2, 0x2, 0x2, 0xdb, 0xdc, 0x7, 0x6, 
    0x2, 0x2, 0xdc, 0x170, 0x3, 0x2, 0x2, 0x2, 0xdd, 0xde, 0x7, 0x31, 0x2, 
    0x2, 0xde, 0xdf, 0x7, 0x5, 0x2, 0x2, 0xdf, 0xe2, 0x5, 0x8, 0x5, 0x2, 
    0xe0, 0xe1, 0x7, 0xa, 0x2, 0x2, 0xe1, 0xe3, 0x5, 0x8, 0x5, 0x2, 0xe2, 
    0xe0, 0x3, 0x2, 0x2, 0x2, 0xe3, 0xe4, 0x3, 0x2, 0x2, 0x2, 0xe4, 0xe2, 
    0x3, 0x2, 0x2, 0x2, 0xe4, 0xe5, 0x3, 0x2, 0x2, 0x2, 0xe5, 0xe6, 0x3, 
    0x2, 0x2, 0x2, 0xe6, 0xe7, 0x7, 0x6, 0x2, 0x2, 0xe7, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0xe8, 0xe9, 0x7, 0x32, 0x2, 0x2, 0xe9, 0xea, 0x7, 0x5, 0x2, 
    0x2, 0xea, 0xed, 0x5, 0x8, 0x5, 0x2, 0xeb, 0xec, 0x7, 0xa, 0x2, 0x2, 
    0xec, 0xee, 0x5, 0x8, 0x5, 0x2, 0xed, 0xeb, 0x3, 0x2, 0x2, 0x2, 0xee, 
    0xef, 0x3, 0x2, 0x2, 0x2, 0xef, 0xed, 0x3, 0x2, 0x2, 0x2, 0xef, 0xf0, 
    0x3, 0x2, 0x2, 0x2, 0xf0, 0xf1, 0x3, 0x2, 0x2, 0x2, 0xf1, 0xf2, 0x7, 
    0x6, 0x2, 0x2, 0xf2, 0x170, 0x3, 0x2, 0x2, 0x2, 0xf3, 0xf4, 0x7, 0x33, 
    0x2, 0x2, 0xf4, 0xf5, 0x7, 0x5, 0x2, 0x2, 0xf5, 0xf8, 0x5, 0x8, 0x5, 
    0x2, 0xf6, 0xf7, 0x7, 0xa, 0x2, 0x2, 0xf7, 0xf9, 0x5, 0x8, 0x5, 0x2, 
    0xf8, 0xf6, 0x3, 0x2, 0x2, 0x2, 0xf9, 0xfa, 0x3, 0x2, 0x2, 0x2, 0xfa, 
    0xf8, 0x3, 0x2, 0x2, 0x2, 0xfa, 0xfb, 0x3, 0x2, 0x2, 0x2, 0xfb, 0xfc, 
    0x3, 0x2, 0x2, 0x2, 0xfc, 0xfd, 0x7, 0x6, 0x2, 0x2, 0xfd, 0x170, 0x3, 
    0x2, 0x2, 0x2, 0xfe, 0xff, 0x7, 0x34, 0x2, 0x2, 0xff, 0x100, 0x7, 0x5, 
    0x2, 0x2, 0x100, 0x103, 0x5, 0x8, 0x5, 0x2, 0x101, 0x102, 0x7, 0xa, 
    0x2, 0x2, 0x102, 0x104, 0x5, 0x8, 0x5, 0x2, 0x103, 0x101, 0x3, 0x2, 
    0x2, 0x2, 0x104, 0x105, 0x3, 0x2, 0x2, 0x2, 0x105, 0x103, 0x3, 0x2, 
    0x2, 0x2, 0x105, 0x106, 0x3, 0x2, 0x2, 0x2, 0x106, 0x107, 0x3, 0x2, 
    0x2, 0x2, 0x107, 0x108, 0x7, 0x6, 0x2, 0x2, 0x108, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x109, 0x10a, 0x7, 0x35, 0x2, 0x2, 0x10a, 0x10b, 0x7, 0x5, 
    0x2, 0x2, 0x10b, 0x10e, 0x5, 0x8, 0x5, 0x2, 0x10c, 0x10d, 0x7, 0xa, 
    0x2, 0x2, 0x10d, 0x10f, 0x5, 0x8, 0x5, 0x2, 0x10e, 0x10c, 0x3, 0x2, 
    0x2, 0x2, 0x10f, 0x110, 0x3, 0x2, 0x2, 0x2, 0x110, 0x10e, 0x3, 0x2, 
    0x2, 0x2, 0x110, 0x111, 0x3, 0x2, 0x2, 0x2, 0x111, 0x112, 0x3, 0x2, 
    0x2, 0x2, 0x112, 0x113, 0x7, 0x6, 0x2, 0x2, 0x113, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x114, 0x115, 0x7, 0x36, 0x2, 0x2, 0x115, 0x116, 0x7, 0x5, 
    0x2, 0x2, 0x116, 0x119, 0x5, 0x8, 0x5, 0x2, 0x117, 0x118, 0x7, 0xa, 
    0x2, 0x2, 0x118, 0x11a, 0x5, 0x8, 0x5, 0x2, 0x119, 0x117, 0x3, 0x2, 
    0x2, 0x2, 0x11a, 0x11b, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x119, 0x3, 0x2, 
    0x2, 0x2, 0x11b, 0x11c, 0x3, 0x2, 0x2, 0x2, 0x11c, 0x11d, 0x3, 0x2, 
    0x2, 0x2, 0x11d, 0x11e, 0x7, 0x6, 0x2, 0x2, 0x11e, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x11f, 0x120, 0x7, 0x37, 0x2, 0x2, 0x120, 0x121, 0x7, 0x5, 
    0x2, 0x2, 0x121, 0x124, 0x5, 0x8, 0x5, 0x2, 0x122, 0x123, 0x7, 0xa, 
    0x2, 0x2, 0x123, 0x125, 0x5, 0x8, 0x5, 0x2, 0x124, 0x122, 0x3, 0x2, 
    0x2, 0x2, 0x125, 0x126, 0x3, 0x2, 0x2, 0x2, 0x126, 0x124, 0x3, 0x2, 
    0x2, 0x2, 0x126, 0x127, 0x3, 0x2, 0x2, 0x2, 0x127, 0x128, 0x3, 0x2, 
    0x2, 0x2, 0x128, 0x129, 0x7, 0x6, 0x2, 0x2, 0x129, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x12a, 0x12b, 0x7, 0x38, 0x2, 0x2, 0x12b, 0x12c, 0x7, 0x5, 
    0x2, 0x2, 0x12c, 0x12d, 0x5, 0x8, 0x5, 0x2, 0x12d, 0x12e, 0x7, 0x6, 
    0x2, 0x2, 0x12e, 0x170, 0x3, 0x2, 0x2, 0x2, 0x12f, 0x130, 0x7, 0x39, 
    0x2, 0x2, 0x130, 0x131, 0x7, 0x5, 0x2, 0x2, 0x131, 0x132, 0x5, 0x8, 
    0x5, 0x2, 0x132, 0x133, 0x7, 0xa, 0x2, 0x2, 0x133, 0x134, 0x5, 0x8, 
    0x5, 0x2, 0x134, 0x135, 0x7, 0x6, 0x2, 0x2, 0x135, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x136, 0x137, 0x7, 0x3a, 0x2, 0x2, 0x137, 0x138, 0x7, 0x5, 
    0x2, 0x2, 0x138, 0x139, 0x5, 0x8, 0x5, 0x2, 0x139, 0x13a, 0x7, 0xa, 
    0x2, 0x2, 0x13a, 0x13b, 0x5, 0x8, 0x5, 0x2, 0x13b, 0x13c, 0x7, 0x6, 
    0x2, 0x2, 0x13c, 0x170, 0x3, 0x2, 0x2, 0x2, 0x13d, 0x13e, 0x7, 0x3b, 
    0x2, 0x2, 0x13e, 0x13f, 0x7, 0x5, 0x2, 0x2, 0x13f, 0x140, 0x5, 0x8, 
    0x5, 0x2, 0x140, 0x141, 0x7, 0x6, 0x2, 0x2, 0x141, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x142, 0x143, 0x7, 0x3b, 0x2, 0x2, 0x143, 0x144, 0x7, 0x5, 
    0x2, 0x2, 0x144, 0x145, 0x5, 0x8, 0x5, 0x2, 0x145, 0x146, 0x7, 0xa, 
    0x2, 0x2, 0x146, 0x147, 0x5, 0x8, 0x5, 0x2, 0x147, 0x148, 0x7, 0x6, 
    0x2, 0x2, 0x148, 0x170, 0x3, 0x2, 0x2, 0x2, 0x149, 0x14a, 0x7, 0x3b, 
    0x2, 0x2, 0x14a, 0x14b, 0x7, 0x5, 0x2, 0x2, 0x14b, 0x14c, 0x5, 0x8, 
    0x5, 0x2, 0x14c, 0x14d, 0x7, 0xa, 0x2, 0x2, 0x14d, 0x14e, 0x5, 0x8, 
    0x5, 0x2, 0x14e, 0x14f, 0x7, 0xa, 0x2, 0x2, 0x14f, 0x150, 0x5, 0x8, 
    0x5, 0x2, 0x150, 0x151, 0x7, 0x6, 0x2, 0x2, 0x151, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x152, 0x153, 0x7, 0x3c, 0x2, 0x2, 0x153, 0x154, 0x7, 0x5, 
    0x2, 0x2, 0x154, 0x155, 0x5, 0x8, 0x5, 0x2, 0x155, 0x156, 0x7, 0x6, 
    0x2, 0x2, 0x156, 0x170, 0x3, 0x2, 0x2, 0x2, 0x157, 0x158, 0x7, 0x3d, 
    0x2, 0x2, 0x158, 0x159, 0x7, 0x5, 0x2, 0x2, 0x159, 0x15a, 0x5, 0x8, 
    0x5, 0x2, 0x15a, 0x15b, 0x7, 0x6, 0x2, 0x2, 0x15b, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x15c, 0x15d, 0x7, 0x3e, 0x2, 0x2, 0x15d, 0x15e, 0x7, 0x5, 
    0x2, 0x2, 0x15e, 0x15f, 0x5, 0x8, 0x5, 0x2, 0x15f, 0x160, 0x7, 0x6, 
    0x2, 0x2, 0x160, 0x170, 0x3, 0x2, 0x2, 0x2, 0x161, 0x162, 0x7, 0x3f, 
    0x2, 0x2, 0x162, 0x163, 0x7, 0x5, 0x2, 0x2, 0x163, 0x164, 0x5, 0x8, 
    0x5, 0x2, 0x164, 0x165, 0x7, 0x6, 0x2, 0x2, 0x165, 0x170, 0x3, 0x2, 
    0x2, 0x2, 0x166, 0x167, 0x7, 0x40, 0x2, 0x2, 0x167, 0x168, 0x7, 0x5, 
    0x2, 0x2, 0x168, 0x170, 0x7, 0x6, 0x2, 0x2, 0x169, 0x16a, 0x7, 0x41, 
    0x2, 0x2, 0x16a, 0x16b, 0x7, 0x5, 0x2, 0x2, 0x16b, 0x16c, 0x7, 0x45, 
    0x2, 0x2, 0x16c, 0x16d, 0x7, 0xa, 0x2, 0x2, 0x16d, 0x16e, 0x7, 0x45, 
    0x2, 0x2, 0x16e, 0x170, 0x7, 0x6, 0x2, 0x2, 0x16f, 0x60, 0x3, 0x2, 0x2, 
    0x2, 0x16f, 0x65, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x6a, 0x3, 0x2, 0x2, 0x2, 
    0x16f, 0x6f, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x74, 0x3, 0x2, 0x2, 0x2, 0x16f, 
    0x7b, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x80, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x85, 
    0x3, 0x2, 0x2, 0x2, 0x16f, 0x8b, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x90, 0x3, 
    0x2, 0x2, 0x2, 0x16f, 0x97, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x9c, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0xa1, 0x3, 0x2, 0x2, 0x2, 0x16f, 0xa8, 0x3, 0x2, 0x2, 
    0x2, 0x16f, 0xb1, 0x3, 0x2, 0x2, 0x2, 0x16f, 0xbc, 0x3, 0x2, 0x2, 0x2, 
    0x16f, 0xc1, 0x3, 0x2, 0x2, 0x2, 0x16f, 0xc6, 0x3, 0x2, 0x2, 0x2, 0x16f, 
    0xcb, 0x3, 0x2, 0x2, 0x2, 0x16f, 0xd2, 0x3, 0x2, 0x2, 0x2, 0x16f, 0xdd, 
    0x3, 0x2, 0x2, 0x2, 0x16f, 0xe8, 0x3, 0x2, 0x2, 0x2, 0x16f, 0xf3, 0x3, 
    0x2, 0x2, 0x2, 0x16f, 0xfe, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x109, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x114, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x11f, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x12f, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x136, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x13d, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x142, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x149, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x152, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x157, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x15c, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x161, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x166, 0x3, 0x2, 0x2, 0x2, 0x16f, 0x169, 0x3, 0x2, 
    0x2, 0x2, 0x170, 0xb, 0x3, 0x2, 0x2, 0x2, 0x171, 0x176, 0x5, 0x8, 0x5, 
    0x2, 0x172, 0x173, 0x7, 0xa, 0x2, 0x2, 0x173, 0x175, 0x5, 0x8, 0x5, 
    0x2, 0x174, 0x172, 0x3, 0x2, 0x2, 0x2, 0x175, 0x178, 0x3, 0x2, 0x2, 
    0x2, 0x176, 0x174, 0x3, 0x2, 0x2, 0x2, 0x176, 0x177, 0x3, 0x2, 0x2, 
    0x2, 0x177, 0xd, 0x3, 0x2, 0x2, 0x2, 0x178, 0x176, 0x3, 0x2, 0x2, 0x2, 
    0x179, 0x17f, 0x7, 0x44, 0x2, 0x2, 0x17a, 0x17f, 0x7, 0x42, 0x2, 0x2, 
    0x17b, 0x17f, 0x7, 0x43, 0x2, 0x2, 0x17c, 0x17f, 0x7, 0x45, 0x2, 0x2, 
    0x17d, 0x17f, 0x7, 0x46, 0x2, 0x2, 0x17e, 0x179, 0x3, 0x2, 0x2, 0x2, 
    0x17e, 0x17a, 0x3, 0x2, 0x2, 0x2, 0x17e, 0x17b, 0x3, 0x2, 0x2, 0x2, 
    0x17e, 0x17c, 0x3, 0x2, 0x2, 0x2, 0x17e, 0x17d, 0x3, 0x2, 0x2, 0x2, 
    0x17f, 0xf, 0x3, 0x2, 0x2, 0x2, 0x180, 0x184, 0x7, 0xb, 0x2, 0x2, 0x181, 
    0x184, 0x7, 0xc, 0x2, 0x2, 0x182, 0x184, 0x7, 0x21, 0x2, 0x2, 0x183, 
    0x180, 0x3, 0x2, 0x2, 0x2, 0x183, 0x181, 0x3, 0x2, 0x2, 0x2, 0x183, 
    0x182, 0x3, 0x2, 0x2, 0x2, 0x184, 0x11, 0x3, 0x2, 0x2, 0x2, 0x15, 0x19, 
    0x1d, 0x25, 0x29, 0x35, 0x5b, 0x5d, 0xd9, 0xe4, 0xef, 0xfa, 0x105, 0x110, 
    0x11b, 0x126, 0x16f, 0x176, 0x17e, 0x183, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

OParser::Initializer OParser::_init;
