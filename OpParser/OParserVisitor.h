
// Generated from /media/loki/Niflheim/Tesis/CHISel/OpParser/OParser.g4 by ANTLR 4.7

#pragma once


#include "antlr4-runtime.h"
#include "OParser.h"


namespace OpLanguage {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by OParser.
 */
class  OParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by OParser.
   */
    virtual antlrcpp::Any visitProgram(OParser::ProgramContext *context) = 0;

    virtual antlrcpp::Any visitDefs(OParser::DefsContext *context) = 0;

    virtual antlrcpp::Any visitDef(OParser::DefContext *context) = 0;

    virtual antlrcpp::Any visitLogAndExp(OParser::LogAndExpContext *context) = 0;

    virtual antlrcpp::Any visitUnitaryExp(OParser::UnitaryExpContext *context) = 0;

    virtual antlrcpp::Any visitShiftExp(OParser::ShiftExpContext *context) = 0;

    virtual antlrcpp::Any visitCondExp(OParser::CondExpContext *context) = 0;

    virtual antlrcpp::Any visitLogOrExp(OParser::LogOrExpContext *context) = 0;

    virtual antlrcpp::Any visitParExp(OParser::ParExpContext *context) = 0;

    virtual antlrcpp::Any visitBinaryOpExp(OParser::BinaryOpExpContext *context) = 0;

    virtual antlrcpp::Any visitConstantExp(OParser::ConstantExpContext *context) = 0;

    virtual antlrcpp::Any visitExponentialExp(OParser::ExponentialExpContext *context) = 0;

    virtual antlrcpp::Any visitFunctionExp(OParser::FunctionExpContext *context) = 0;

    virtual antlrcpp::Any visitBinLogOpExp(OParser::BinLogOpExpContext *context) = 0;

    virtual antlrcpp::Any visitAbs(OParser::AbsContext *context) = 0;

    virtual antlrcpp::Any visitAcos(OParser::AcosContext *context) = 0;

    virtual antlrcpp::Any visitAsin(OParser::AsinContext *context) = 0;

    virtual antlrcpp::Any visitAtan(OParser::AtanContext *context) = 0;

    virtual antlrcpp::Any visitAtan2(OParser::Atan2Context *context) = 0;

    virtual antlrcpp::Any visitCos(OParser::CosContext *context) = 0;

    virtual antlrcpp::Any visitCastToDouble(OParser::CastToDoubleContext *context) = 0;

    virtual antlrcpp::Any visitEval(OParser::EvalContext *context) = 0;

    virtual antlrcpp::Any visitExponential(OParser::ExponentialContext *context) = 0;

    virtual antlrcpp::Any visitExponential2(OParser::Exponential2Context *context) = 0;

    virtual antlrcpp::Any visitCastToFloat(OParser::CastToFloatContext *context) = 0;

    virtual antlrcpp::Any visitIf(OParser::IfContext *context) = 0;

    virtual antlrcpp::Any visitIf2(OParser::If2Context *context) = 0;

    virtual antlrcpp::Any visitIf3(OParser::If3Context *context) = 0;

    virtual antlrcpp::Any visitIf4(OParser::If4Context *context) = 0;

    virtual antlrcpp::Any visitCastToInt(OParser::CastToIntContext *context) = 0;

    virtual antlrcpp::Any visitIsNull(OParser::IsNullContext *context) = 0;

    virtual antlrcpp::Any visitLog(OParser::LogContext *context) = 0;

    virtual antlrcpp::Any visitLog2(OParser::Log2Context *context) = 0;

    virtual antlrcpp::Any visitMax(OParser::MaxContext *context) = 0;

    virtual antlrcpp::Any visitMedian(OParser::MedianContext *context) = 0;

    virtual antlrcpp::Any visitMin(OParser::MinContext *context) = 0;

    virtual antlrcpp::Any visitMathMode(OParser::MathModeContext *context) = 0;

    virtual antlrcpp::Any visitNmax(OParser::NmaxContext *context) = 0;

    virtual antlrcpp::Any visitNmedian(OParser::NmedianContext *context) = 0;

    virtual antlrcpp::Any visitNmin(OParser::NminContext *context) = 0;

    virtual antlrcpp::Any visitNmode(OParser::NmodeContext *context) = 0;

    virtual antlrcpp::Any visitNot(OParser::NotContext *context) = 0;

    virtual antlrcpp::Any visitRand(OParser::RandContext *context) = 0;

    virtual antlrcpp::Any visitRound(OParser::RoundContext *context) = 0;

    virtual antlrcpp::Any visitRound2(OParser::Round2Context *context) = 0;

    virtual antlrcpp::Any visitRound3(OParser::Round3Context *context) = 0;

    virtual antlrcpp::Any visitSin(OParser::SinContext *context) = 0;

    virtual antlrcpp::Any visitSqrt(OParser::SqrtContext *context) = 0;

    virtual antlrcpp::Any visitTan(OParser::TanContext *context) = 0;

    virtual antlrcpp::Any visitXor(OParser::XorContext *context) = 0;

    virtual antlrcpp::Any visitNull(OParser::NullContext *context) = 0;

    virtual antlrcpp::Any visitSql(OParser::SqlContext *context) = 0;

    virtual antlrcpp::Any visitExpr_list(OParser::Expr_listContext *context) = 0;

    virtual antlrcpp::Any visitIntConst(OParser::IntConstContext *context) = 0;

    virtual antlrcpp::Any visitFloatConst(OParser::FloatConstContext *context) = 0;

    virtual antlrcpp::Any visitDoubleConst(OParser::DoubleConstContext *context) = 0;

    virtual antlrcpp::Any visitIdentifier(OParser::IdentifierContext *context) = 0;

    virtual antlrcpp::Any visitStringConst(OParser::StringConstContext *context) = 0;

    virtual antlrcpp::Any visitNegOp(OParser::NegOpContext *context) = 0;

    virtual antlrcpp::Any visitCompOp(OParser::CompOpContext *context) = 0;

    virtual antlrcpp::Any visitNotOp(OParser::NotOpContext *context) = 0;


};

}  // namespace OpLanguage
