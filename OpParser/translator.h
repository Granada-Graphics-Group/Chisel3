#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "OParserBaseListener.h"
#include "chisel.h"

#include <ParseTreeProperty.h>
#include <CommonTokenStream.h>
#include <string>
#include <deque>
#include <tuple>


class Translator : public OpLanguage::OParserBaseListener
{
public:    
    struct NodeInfo
    {
        std::string data;
        std::string mask;
        Layer::Type type;
        std::pair<int, int> resolution;
    };    
    
    Translator(antlr4::CommonTokenStream* tokens, Chisel *chisel);
    
    void exitProgram(OpLanguage::OParser::ProgramContext* ctx) override;
    void exitDefs(OpLanguage::OParser::DefsContext* ctx) override;
    void exitDef(OpLanguage::OParser::DefContext* ctx) override;
    void exitParExp(OpLanguage::OParser::ParExpContext* ctx) override;
    void exitFunctionExp(OpLanguage::OParser::FunctionExpContext* ctx) override;
    void exitConstantExp(OpLanguage::OParser::ConstantExpContext* ctx) override;
    void exitExponentialExp(OpLanguage::OParser::ExponentialExpContext* ctx) override;
    void exitUnitaryExp(OpLanguage::OParser::UnitaryExpContext * ctx) override;
    void exitBinaryOpExp(OpLanguage::OParser::BinaryOpExpContext* ctx) override;
    void exitBinLogOpExp(OpLanguage::OParser::BinLogOpExpContext* ctx) override;
    void exitLogAndExp(OpLanguage::OParser::LogAndExpContext* ctx) override;
    void exitLogOrExp(OpLanguage::OParser::LogOrExpContext* ctx) override;
    void exitShiftExp(OpLanguage::OParser::ShiftExpContext* ctx) override;
    void exitCondExp(OpLanguage::OParser::CondExpContext* ctx) override;

    void exitAbs(OpLanguage::OParser::AbsContext* ctx) override;
    void exitAcos(OpLanguage::OParser::AcosContext* ctx) override;
    void exitAsin(OpLanguage::OParser::AsinContext* ctx) override;
    void exitAtan(OpLanguage::OParser::AtanContext* ctx) override;
    void exitAtan2(OpLanguage::OParser::Atan2Context* ctx) override;
    void exitCos(OpLanguage::OParser::CosContext* ctx) override;
    void exitExponential(OpLanguage::OParser::ExponentialContext* ctx) override;
    void exitExponential2(OpLanguage::OParser::Exponential2Context* ctx) override;
    void exitCastToFloat(OpLanguage::OParser::CastToFloatContext* ctx) override;
    void exitIf(OpLanguage::OParser::IfContext* ctx) override;
    void exitIf2(OpLanguage::OParser::If2Context* ctx) override;
    void exitIf3(OpLanguage::OParser::If3Context* ctx) override;
    void exitIf4(OpLanguage::OParser::If4Context* ctx) override;
    void exitCastToInt(OpLanguage::OParser::CastToIntContext* ctx) override;
    void exitIsNull(OpLanguage::OParser::IsNullContext* ctx) override;
    void exitLog(OpLanguage::OParser::LogContext* ctx) override;
    void exitLog2(OpLanguage::OParser::Log2Context* ctx) override;
    void exitMax(OpLanguage::OParser::MaxContext * ctx) override;
    void exitMedian(OpLanguage::OParser::MedianContext * ctx) override;
    void exitMin(OpLanguage::OParser::MinContext * ctx) override;
    void exitMathMode(OpLanguage::OParser::MathModeContext * ctx) override;
    void exitNot(OpLanguage::OParser::NotContext * ctx) override;
    void exitRand(OpLanguage::OParser::RandContext * ctx) override;
    void exitRound(OpLanguage::OParser::RoundContext * ctx) override;
    void exitRound2(OpLanguage::OParser::Round2Context * ctx) override;
    void exitRound3(OpLanguage::OParser::Round3Context * ctx) override;
    void exitSin(OpLanguage::OParser::SinContext* ctx) override;
    void exitSqrt(OpLanguage::OParser::SqrtContext* ctx) override;
    void exitTan(OpLanguage::OParser::TanContext* ctx) override;
    void exitNull(OpLanguage::OParser::NullContext* ctx) override;
    void exitSql(OpLanguage::OParser::SqlContext * ctx) override;
    
    void exitIdentifier(OpLanguage::OParser::IdentifierContext* ctx) override;    
    void exitIntConst(OpLanguage::OParser::IntConstContext* ctx) override;
    void exitFloatConst(OpLanguage::OParser::FloatConstContext* ctx) override;
    
    NodeInfo nodeInfo(antlr4::tree::ParseTree* node){ return mNodeInfos.get(node); }
    void setNodeInfo(antlr4::tree::ParseTree* node, const NodeInfo& glslString){ mNodeInfos.put(node, glslString); }

    const std::vector<Layer*>& newLayers() const { return mNewLayers; }
    const std::vector<Layer*>& tempLayers() const { return mTempLayers; }
    std::string source() const { return mImageDeclarations + mInitCode + mDeclarationCode + mSourceString + mEndCode; }
    const std::vector<std::string>& sources() const{ return  mSources; }
    
    const std::map<int, antlr4::Token*, std::greater<int>>& offendingSymbols() const { return mOffendingSymbols; }
    const std::string& semanticErrorMessages() const { return mSemanticErrorMessages; }
    
private:    
    unsigned int mMaskCount = 0;
    unsigned int mDataCount = 0;

    std::string toGLSLType(Layer::Type type);
//     std::string toGLSLImageFormat(int32_t format);
//     std::string prefixImageFormat(int32_t format);
    Layer::Type implicitTypeConversion(Layer::Type operandA, Layer::Type operandB);
    void updateImageDeclarations(const std::string& dataImageName, const std::string& maskImageName, const Layer* layer);
    
    antlr4::CommonTokenStream* mTokens;
    Chisel* mChisel;    
    std::vector<std::string> mIdentifiers;
    std::vector<std::string> mImages;
    const std::vector<std::string> mTypeStringList = {{"bool", "int", "float", "int"}};
    
    std::string mInitCode;
    std::string mImageDeclarations;
    std::string mDeclarationCode;
    std::string mSourceString;
    std::string mEndCode;
    
    std::vector<Layer*> mNewLayers;
    std::vector<Layer*> mTempLayers;
    std::vector<std::string> mSources;
    
    std::map<int, antlr4::Token*, std::greater<int>> mOffendingSymbols;
    std::string mSemanticErrorMessages;
    
    antlr4::tree::ParseTreeProperty<std::string> mGLSL;
    antlr4::tree::ParseTreeProperty<NodeInfo> mNodeInfos;
};

#endif
