
// Generated from /media/loki/Niflheim/Tesis/CHISel/OpParser/OParser.g4 by ANTLR 4.7

#pragma once


#include "antlr4-runtime.h"
#include "OParserListener.h"


namespace OpLanguage {

/**
 * This class provides an empty implementation of OParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  OParserBaseListener : public OParserListener {
public:

  virtual void enterProgram(OParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(OParser::ProgramContext * /*ctx*/) override { }

  virtual void enterDefs(OParser::DefsContext * /*ctx*/) override { }
  virtual void exitDefs(OParser::DefsContext * /*ctx*/) override { }

  virtual void enterDef(OParser::DefContext * /*ctx*/) override { }
  virtual void exitDef(OParser::DefContext * /*ctx*/) override { }

  virtual void enterLogAndExp(OParser::LogAndExpContext * /*ctx*/) override { }
  virtual void exitLogAndExp(OParser::LogAndExpContext * /*ctx*/) override { }

  virtual void enterUnitaryExp(OParser::UnitaryExpContext * /*ctx*/) override { }
  virtual void exitUnitaryExp(OParser::UnitaryExpContext * /*ctx*/) override { }

  virtual void enterShiftExp(OParser::ShiftExpContext * /*ctx*/) override { }
  virtual void exitShiftExp(OParser::ShiftExpContext * /*ctx*/) override { }

  virtual void enterCondExp(OParser::CondExpContext * /*ctx*/) override { }
  virtual void exitCondExp(OParser::CondExpContext * /*ctx*/) override { }

  virtual void enterLogOrExp(OParser::LogOrExpContext * /*ctx*/) override { }
  virtual void exitLogOrExp(OParser::LogOrExpContext * /*ctx*/) override { }

  virtual void enterParExp(OParser::ParExpContext * /*ctx*/) override { }
  virtual void exitParExp(OParser::ParExpContext * /*ctx*/) override { }

  virtual void enterBinaryOpExp(OParser::BinaryOpExpContext * /*ctx*/) override { }
  virtual void exitBinaryOpExp(OParser::BinaryOpExpContext * /*ctx*/) override { }

  virtual void enterConstantExp(OParser::ConstantExpContext * /*ctx*/) override { }
  virtual void exitConstantExp(OParser::ConstantExpContext * /*ctx*/) override { }

  virtual void enterExponentialExp(OParser::ExponentialExpContext * /*ctx*/) override { }
  virtual void exitExponentialExp(OParser::ExponentialExpContext * /*ctx*/) override { }

  virtual void enterFunctionExp(OParser::FunctionExpContext * /*ctx*/) override { }
  virtual void exitFunctionExp(OParser::FunctionExpContext * /*ctx*/) override { }

  virtual void enterBinLogOpExp(OParser::BinLogOpExpContext * /*ctx*/) override { }
  virtual void exitBinLogOpExp(OParser::BinLogOpExpContext * /*ctx*/) override { }

  virtual void enterAbs(OParser::AbsContext * /*ctx*/) override { }
  virtual void exitAbs(OParser::AbsContext * /*ctx*/) override { }

  virtual void enterAcos(OParser::AcosContext * /*ctx*/) override { }
  virtual void exitAcos(OParser::AcosContext * /*ctx*/) override { }

  virtual void enterAsin(OParser::AsinContext * /*ctx*/) override { }
  virtual void exitAsin(OParser::AsinContext * /*ctx*/) override { }

  virtual void enterAtan(OParser::AtanContext * /*ctx*/) override { }
  virtual void exitAtan(OParser::AtanContext * /*ctx*/) override { }

  virtual void enterAtan2(OParser::Atan2Context * /*ctx*/) override { }
  virtual void exitAtan2(OParser::Atan2Context * /*ctx*/) override { }

  virtual void enterCos(OParser::CosContext * /*ctx*/) override { }
  virtual void exitCos(OParser::CosContext * /*ctx*/) override { }

  virtual void enterCastToDouble(OParser::CastToDoubleContext * /*ctx*/) override { }
  virtual void exitCastToDouble(OParser::CastToDoubleContext * /*ctx*/) override { }

  virtual void enterEval(OParser::EvalContext * /*ctx*/) override { }
  virtual void exitEval(OParser::EvalContext * /*ctx*/) override { }

  virtual void enterExponential(OParser::ExponentialContext * /*ctx*/) override { }
  virtual void exitExponential(OParser::ExponentialContext * /*ctx*/) override { }

  virtual void enterExponential2(OParser::Exponential2Context * /*ctx*/) override { }
  virtual void exitExponential2(OParser::Exponential2Context * /*ctx*/) override { }

  virtual void enterCastToFloat(OParser::CastToFloatContext * /*ctx*/) override { }
  virtual void exitCastToFloat(OParser::CastToFloatContext * /*ctx*/) override { }

  virtual void enterIf(OParser::IfContext * /*ctx*/) override { }
  virtual void exitIf(OParser::IfContext * /*ctx*/) override { }

  virtual void enterIf2(OParser::If2Context * /*ctx*/) override { }
  virtual void exitIf2(OParser::If2Context * /*ctx*/) override { }

  virtual void enterIf3(OParser::If3Context * /*ctx*/) override { }
  virtual void exitIf3(OParser::If3Context * /*ctx*/) override { }

  virtual void enterIf4(OParser::If4Context * /*ctx*/) override { }
  virtual void exitIf4(OParser::If4Context * /*ctx*/) override { }

  virtual void enterCastToInt(OParser::CastToIntContext * /*ctx*/) override { }
  virtual void exitCastToInt(OParser::CastToIntContext * /*ctx*/) override { }

  virtual void enterIsNull(OParser::IsNullContext * /*ctx*/) override { }
  virtual void exitIsNull(OParser::IsNullContext * /*ctx*/) override { }

  virtual void enterLog(OParser::LogContext * /*ctx*/) override { }
  virtual void exitLog(OParser::LogContext * /*ctx*/) override { }

  virtual void enterLog2(OParser::Log2Context * /*ctx*/) override { }
  virtual void exitLog2(OParser::Log2Context * /*ctx*/) override { }

  virtual void enterMax(OParser::MaxContext * /*ctx*/) override { }
  virtual void exitMax(OParser::MaxContext * /*ctx*/) override { }

  virtual void enterMedian(OParser::MedianContext * /*ctx*/) override { }
  virtual void exitMedian(OParser::MedianContext * /*ctx*/) override { }

  virtual void enterMin(OParser::MinContext * /*ctx*/) override { }
  virtual void exitMin(OParser::MinContext * /*ctx*/) override { }

  virtual void enterMathMode(OParser::MathModeContext * /*ctx*/) override { }
  virtual void exitMathMode(OParser::MathModeContext * /*ctx*/) override { }

  virtual void enterNmax(OParser::NmaxContext * /*ctx*/) override { }
  virtual void exitNmax(OParser::NmaxContext * /*ctx*/) override { }

  virtual void enterNmedian(OParser::NmedianContext * /*ctx*/) override { }
  virtual void exitNmedian(OParser::NmedianContext * /*ctx*/) override { }

  virtual void enterNmin(OParser::NminContext * /*ctx*/) override { }
  virtual void exitNmin(OParser::NminContext * /*ctx*/) override { }

  virtual void enterNmode(OParser::NmodeContext * /*ctx*/) override { }
  virtual void exitNmode(OParser::NmodeContext * /*ctx*/) override { }

  virtual void enterNot(OParser::NotContext * /*ctx*/) override { }
  virtual void exitNot(OParser::NotContext * /*ctx*/) override { }

  virtual void enterRand(OParser::RandContext * /*ctx*/) override { }
  virtual void exitRand(OParser::RandContext * /*ctx*/) override { }

  virtual void enterRound(OParser::RoundContext * /*ctx*/) override { }
  virtual void exitRound(OParser::RoundContext * /*ctx*/) override { }

  virtual void enterRound2(OParser::Round2Context * /*ctx*/) override { }
  virtual void exitRound2(OParser::Round2Context * /*ctx*/) override { }

  virtual void enterRound3(OParser::Round3Context * /*ctx*/) override { }
  virtual void exitRound3(OParser::Round3Context * /*ctx*/) override { }

  virtual void enterSin(OParser::SinContext * /*ctx*/) override { }
  virtual void exitSin(OParser::SinContext * /*ctx*/) override { }

  virtual void enterSqrt(OParser::SqrtContext * /*ctx*/) override { }
  virtual void exitSqrt(OParser::SqrtContext * /*ctx*/) override { }

  virtual void enterTan(OParser::TanContext * /*ctx*/) override { }
  virtual void exitTan(OParser::TanContext * /*ctx*/) override { }

  virtual void enterXor(OParser::XorContext * /*ctx*/) override { }
  virtual void exitXor(OParser::XorContext * /*ctx*/) override { }

  virtual void enterNull(OParser::NullContext * /*ctx*/) override { }
  virtual void exitNull(OParser::NullContext * /*ctx*/) override { }

  virtual void enterSql(OParser::SqlContext * /*ctx*/) override { }
  virtual void exitSql(OParser::SqlContext * /*ctx*/) override { }

  virtual void enterExpr_list(OParser::Expr_listContext * /*ctx*/) override { }
  virtual void exitExpr_list(OParser::Expr_listContext * /*ctx*/) override { }

  virtual void enterIntConst(OParser::IntConstContext * /*ctx*/) override { }
  virtual void exitIntConst(OParser::IntConstContext * /*ctx*/) override { }

  virtual void enterFloatConst(OParser::FloatConstContext * /*ctx*/) override { }
  virtual void exitFloatConst(OParser::FloatConstContext * /*ctx*/) override { }

  virtual void enterDoubleConst(OParser::DoubleConstContext * /*ctx*/) override { }
  virtual void exitDoubleConst(OParser::DoubleConstContext * /*ctx*/) override { }

  virtual void enterIdentifier(OParser::IdentifierContext * /*ctx*/) override { }
  virtual void exitIdentifier(OParser::IdentifierContext * /*ctx*/) override { }

  virtual void enterStringConst(OParser::StringConstContext * /*ctx*/) override { }
  virtual void exitStringConst(OParser::StringConstContext * /*ctx*/) override { }

  virtual void enterNegOp(OParser::NegOpContext * /*ctx*/) override { }
  virtual void exitNegOp(OParser::NegOpContext * /*ctx*/) override { }

  virtual void enterCompOp(OParser::CompOpContext * /*ctx*/) override { }
  virtual void exitCompOp(OParser::CompOpContext * /*ctx*/) override { }

  virtual void enterNotOp(OParser::NotOpContext * /*ctx*/) override { }
  virtual void exitNotOp(OParser::NotOpContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace OpLanguage
