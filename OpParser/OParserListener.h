
// Generated from /media/loki/Niflheim/Tesis/github/Chisel3/OpParser/OParser.g4 by ANTLR 4.7

#pragma once


#include "antlr4-runtime.h"
#include "OParser.h"


namespace OpLanguage {

/**
 * This interface defines an abstract listener for a parse tree produced by OParser.
 */
class  OParserListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(OParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(OParser::ProgramContext *ctx) = 0;

  virtual void enterDefs(OParser::DefsContext *ctx) = 0;
  virtual void exitDefs(OParser::DefsContext *ctx) = 0;

  virtual void enterDef(OParser::DefContext *ctx) = 0;
  virtual void exitDef(OParser::DefContext *ctx) = 0;

  virtual void enterLogAndExp(OParser::LogAndExpContext *ctx) = 0;
  virtual void exitLogAndExp(OParser::LogAndExpContext *ctx) = 0;

  virtual void enterUnitaryExp(OParser::UnitaryExpContext *ctx) = 0;
  virtual void exitUnitaryExp(OParser::UnitaryExpContext *ctx) = 0;

  virtual void enterShiftExp(OParser::ShiftExpContext *ctx) = 0;
  virtual void exitShiftExp(OParser::ShiftExpContext *ctx) = 0;

  virtual void enterCondExp(OParser::CondExpContext *ctx) = 0;
  virtual void exitCondExp(OParser::CondExpContext *ctx) = 0;

  virtual void enterLogOrExp(OParser::LogOrExpContext *ctx) = 0;
  virtual void exitLogOrExp(OParser::LogOrExpContext *ctx) = 0;

  virtual void enterParExp(OParser::ParExpContext *ctx) = 0;
  virtual void exitParExp(OParser::ParExpContext *ctx) = 0;

  virtual void enterBinaryOpExp(OParser::BinaryOpExpContext *ctx) = 0;
  virtual void exitBinaryOpExp(OParser::BinaryOpExpContext *ctx) = 0;

  virtual void enterConstantExp(OParser::ConstantExpContext *ctx) = 0;
  virtual void exitConstantExp(OParser::ConstantExpContext *ctx) = 0;

  virtual void enterExponentialExp(OParser::ExponentialExpContext *ctx) = 0;
  virtual void exitExponentialExp(OParser::ExponentialExpContext *ctx) = 0;

  virtual void enterFunctionExp(OParser::FunctionExpContext *ctx) = 0;
  virtual void exitFunctionExp(OParser::FunctionExpContext *ctx) = 0;

  virtual void enterBinLogOpExp(OParser::BinLogOpExpContext *ctx) = 0;
  virtual void exitBinLogOpExp(OParser::BinLogOpExpContext *ctx) = 0;

  virtual void enterAbs(OParser::AbsContext *ctx) = 0;
  virtual void exitAbs(OParser::AbsContext *ctx) = 0;

  virtual void enterAcos(OParser::AcosContext *ctx) = 0;
  virtual void exitAcos(OParser::AcosContext *ctx) = 0;

  virtual void enterAsin(OParser::AsinContext *ctx) = 0;
  virtual void exitAsin(OParser::AsinContext *ctx) = 0;

  virtual void enterAtan(OParser::AtanContext *ctx) = 0;
  virtual void exitAtan(OParser::AtanContext *ctx) = 0;

  virtual void enterAtan2(OParser::Atan2Context *ctx) = 0;
  virtual void exitAtan2(OParser::Atan2Context *ctx) = 0;

  virtual void enterCeil(OParser::CeilContext *ctx) = 0;
  virtual void exitCeil(OParser::CeilContext *ctx) = 0;

  virtual void enterCos(OParser::CosContext *ctx) = 0;
  virtual void exitCos(OParser::CosContext *ctx) = 0;

  virtual void enterCastToDouble(OParser::CastToDoubleContext *ctx) = 0;
  virtual void exitCastToDouble(OParser::CastToDoubleContext *ctx) = 0;

  virtual void enterEval(OParser::EvalContext *ctx) = 0;
  virtual void exitEval(OParser::EvalContext *ctx) = 0;

  virtual void enterExponential(OParser::ExponentialContext *ctx) = 0;
  virtual void exitExponential(OParser::ExponentialContext *ctx) = 0;

  virtual void enterExponential2(OParser::Exponential2Context *ctx) = 0;
  virtual void exitExponential2(OParser::Exponential2Context *ctx) = 0;

  virtual void enterCastToFloat(OParser::CastToFloatContext *ctx) = 0;
  virtual void exitCastToFloat(OParser::CastToFloatContext *ctx) = 0;

  virtual void enterFloor(OParser::FloorContext *ctx) = 0;
  virtual void exitFloor(OParser::FloorContext *ctx) = 0;

  virtual void enterGraph(OParser::GraphContext *ctx) = 0;
  virtual void exitGraph(OParser::GraphContext *ctx) = 0;

  virtual void enterGraph2(OParser::Graph2Context *ctx) = 0;
  virtual void exitGraph2(OParser::Graph2Context *ctx) = 0;

  virtual void enterIf(OParser::IfContext *ctx) = 0;
  virtual void exitIf(OParser::IfContext *ctx) = 0;

  virtual void enterIf2(OParser::If2Context *ctx) = 0;
  virtual void exitIf2(OParser::If2Context *ctx) = 0;

  virtual void enterIf3(OParser::If3Context *ctx) = 0;
  virtual void exitIf3(OParser::If3Context *ctx) = 0;

  virtual void enterIf4(OParser::If4Context *ctx) = 0;
  virtual void exitIf4(OParser::If4Context *ctx) = 0;

  virtual void enterCastToInt(OParser::CastToIntContext *ctx) = 0;
  virtual void exitCastToInt(OParser::CastToIntContext *ctx) = 0;

  virtual void enterIsNull(OParser::IsNullContext *ctx) = 0;
  virtual void exitIsNull(OParser::IsNullContext *ctx) = 0;

  virtual void enterLog(OParser::LogContext *ctx) = 0;
  virtual void exitLog(OParser::LogContext *ctx) = 0;

  virtual void enterLog2(OParser::Log2Context *ctx) = 0;
  virtual void exitLog2(OParser::Log2Context *ctx) = 0;

  virtual void enterMax(OParser::MaxContext *ctx) = 0;
  virtual void exitMax(OParser::MaxContext *ctx) = 0;

  virtual void enterMedian(OParser::MedianContext *ctx) = 0;
  virtual void exitMedian(OParser::MedianContext *ctx) = 0;

  virtual void enterMin(OParser::MinContext *ctx) = 0;
  virtual void exitMin(OParser::MinContext *ctx) = 0;

  virtual void enterMathMode(OParser::MathModeContext *ctx) = 0;
  virtual void exitMathMode(OParser::MathModeContext *ctx) = 0;

  virtual void enterNmax(OParser::NmaxContext *ctx) = 0;
  virtual void exitNmax(OParser::NmaxContext *ctx) = 0;

  virtual void enterNmedian(OParser::NmedianContext *ctx) = 0;
  virtual void exitNmedian(OParser::NmedianContext *ctx) = 0;

  virtual void enterNmin(OParser::NminContext *ctx) = 0;
  virtual void exitNmin(OParser::NminContext *ctx) = 0;

  virtual void enterNmode(OParser::NmodeContext *ctx) = 0;
  virtual void exitNmode(OParser::NmodeContext *ctx) = 0;

  virtual void enterNot(OParser::NotContext *ctx) = 0;
  virtual void exitNot(OParser::NotContext *ctx) = 0;

  virtual void enterRand(OParser::RandContext *ctx) = 0;
  virtual void exitRand(OParser::RandContext *ctx) = 0;

  virtual void enterRound(OParser::RoundContext *ctx) = 0;
  virtual void exitRound(OParser::RoundContext *ctx) = 0;

  virtual void enterRound2(OParser::Round2Context *ctx) = 0;
  virtual void exitRound2(OParser::Round2Context *ctx) = 0;

  virtual void enterRound3(OParser::Round3Context *ctx) = 0;
  virtual void exitRound3(OParser::Round3Context *ctx) = 0;

  virtual void enterSin(OParser::SinContext *ctx) = 0;
  virtual void exitSin(OParser::SinContext *ctx) = 0;

  virtual void enterSqrt(OParser::SqrtContext *ctx) = 0;
  virtual void exitSqrt(OParser::SqrtContext *ctx) = 0;

  virtual void enterTan(OParser::TanContext *ctx) = 0;
  virtual void exitTan(OParser::TanContext *ctx) = 0;

  virtual void enterXor(OParser::XorContext *ctx) = 0;
  virtual void exitXor(OParser::XorContext *ctx) = 0;

  virtual void enterNull(OParser::NullContext *ctx) = 0;
  virtual void exitNull(OParser::NullContext *ctx) = 0;

  virtual void enterSql(OParser::SqlContext *ctx) = 0;
  virtual void exitSql(OParser::SqlContext *ctx) = 0;

  virtual void enterExpr_list(OParser::Expr_listContext *ctx) = 0;
  virtual void exitExpr_list(OParser::Expr_listContext *ctx) = 0;

  virtual void enterIntConst(OParser::IntConstContext *ctx) = 0;
  virtual void exitIntConst(OParser::IntConstContext *ctx) = 0;

  virtual void enterFloatConst(OParser::FloatConstContext *ctx) = 0;
  virtual void exitFloatConst(OParser::FloatConstContext *ctx) = 0;

  virtual void enterDoubleConst(OParser::DoubleConstContext *ctx) = 0;
  virtual void exitDoubleConst(OParser::DoubleConstContext *ctx) = 0;

  virtual void enterIdentifier(OParser::IdentifierContext *ctx) = 0;
  virtual void exitIdentifier(OParser::IdentifierContext *ctx) = 0;

  virtual void enterStringConst(OParser::StringConstContext *ctx) = 0;
  virtual void exitStringConst(OParser::StringConstContext *ctx) = 0;

  virtual void enterNeighborMod(OParser::NeighborModContext *ctx) = 0;
  virtual void exitNeighborMod(OParser::NeighborModContext *ctx) = 0;

  virtual void enterNegOp(OParser::NegOpContext *ctx) = 0;
  virtual void exitNegOp(OParser::NegOpContext *ctx) = 0;

  virtual void enterCompOp(OParser::CompOpContext *ctx) = 0;
  virtual void exitCompOp(OParser::CompOpContext *ctx) = 0;

  virtual void enterNotOp(OParser::NotOpContext *ctx) = 0;
  virtual void exitNotOp(OParser::NotOpContext *ctx) = 0;


};

}  // namespace OpLanguage
