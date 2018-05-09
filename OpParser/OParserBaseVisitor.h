
// Generated from /media/loki/Niflheim/Tesis/CHISel/OpParser/OParser.g4 by ANTLR 4.7

#pragma once


#include "antlr4-runtime.h"
#include "OParserVisitor.h"


namespace OpLanguage {

/**
 * This class provides an empty implementation of OParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  OParserBaseVisitor : public OParserVisitor {
public:

  virtual antlrcpp::Any visitProgram(OParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDefs(OParser::DefsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDef(OParser::DefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLogAndExp(OParser::LogAndExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnitaryExp(OParser::UnitaryExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitShiftExp(OParser::ShiftExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCondExp(OParser::CondExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLogOrExp(OParser::LogOrExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitParExp(OParser::ParExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBinaryOpExp(OParser::BinaryOpExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstantExp(OParser::ConstantExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExponentialExp(OParser::ExponentialExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctionExp(OParser::FunctionExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBinLogOpExp(OParser::BinLogOpExpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAbs(OParser::AbsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAcos(OParser::AcosContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAsin(OParser::AsinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAtan(OParser::AtanContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAtan2(OParser::Atan2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCos(OParser::CosContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCastToDouble(OParser::CastToDoubleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEval(OParser::EvalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExponential(OParser::ExponentialContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExponential2(OParser::Exponential2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCastToFloat(OParser::CastToFloatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIf(OParser::IfContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIf2(OParser::If2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIf3(OParser::If3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIf4(OParser::If4Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCastToInt(OParser::CastToIntContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIsNull(OParser::IsNullContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLog(OParser::LogContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLog2(OParser::Log2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMax(OParser::MaxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMedian(OParser::MedianContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMin(OParser::MinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMathMode(OParser::MathModeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNmax(OParser::NmaxContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNmedian(OParser::NmedianContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNmin(OParser::NminContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNmode(OParser::NmodeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNot(OParser::NotContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRand(OParser::RandContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRound(OParser::RoundContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRound2(OParser::Round2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRound3(OParser::Round3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSin(OParser::SinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSqrt(OParser::SqrtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTan(OParser::TanContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitXor(OParser::XorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNull(OParser::NullContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSql(OParser::SqlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpr_list(OParser::Expr_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIntConst(OParser::IntConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFloatConst(OParser::FloatConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDoubleConst(OParser::DoubleConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIdentifier(OParser::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStringConst(OParser::StringConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNegOp(OParser::NegOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCompOp(OParser::CompOpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNotOp(OParser::NotOpContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace OpLanguage
