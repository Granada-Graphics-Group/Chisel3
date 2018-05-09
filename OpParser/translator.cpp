#include "translator.h"
#include "resourcemanager.h"
#include "texturearray.h"
#include "registerlayer.h"
#include "glutils.h"

#include "logger.hpp"

using namespace OpLanguage;
using namespace antlr4::tree;


Translator::Translator(antlr4::CommonTokenStream* tokens, Chisel *chisel)
:
    mTokens(tokens),
    mChisel(chisel)
{
    mImageDeclarations +=    "#version 450\n"
                             "#extension GL_ARB_compute_variable_group_size : enable\n\n"
                             "layout(local_size_variable) in;\n";
                    
    mInitCode +=    "void main()\n"
                    "{\n"
                    "    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);\n";                 
}


// *** Public Methods *** //

void Translator::exitProgram(OParser::ProgramContext* ctx)
{    
    //mEndCode +=   "}\n";
}

void Translator::exitDefs(OParser::DefsContext* ctx)
{
    //setNodeInfo(ctx, nodeInfo(ctx->def()));
}

void Translator::exitDef(OParser::DefContext* ctx)
{
    auto info = nodeInfo(ctx->expr());
    const auto& layers = mChisel->layers();
    const auto& palettes = mChisel->resourceManager()->palettes();
    auto targetLayerName = ctx->NAME(0)->getText();
    Palette* palette = palettes[0];
    
    if(ctx->NAME().size() > 1)
    {        
        auto paletteName = ctx->NAME(1)->getText();
        auto search = std::find_if(begin(palettes), end(palettes), [&](Palette *palette){ return (paletteName == palette->name()) ? true : false;});
        
        if(search != end(palettes))
        {
            palette = *search;
        }
        else
        {
            auto token = ctx->NAME(1)->getSymbol();
            mOffendingSymbols[static_cast<int>(token->getStartIndex())] = token;
            mSemanticErrorMessages += "line " + std::to_string(token->getLine()) + " : position " + std::to_string(token->getCharPositionInLine()) + ": " + paletteName + " is not a valid palette";            
        }
    }
    
    
    if(mOffendingSymbols.size() == 0)
    {
        auto search = std::find_if(begin(layers), end(layers), [&](Layer *layer){ return (targetLayerName == layer->name()) ? true : false;});
        
        const Layer* targetLayer = nullptr;
        
        if(search == end(layers))    
        {
            auto newLayer = mChisel->resourceManager()->createLayer(targetLayerName, info.type, {2048, 2048}, {}, {}, palette);
            mNewLayers.push_back(newLayer);
            targetLayer = newLayer;
            mChisel->resourceManager()->commitFreeImageUnit(targetLayer->dataTexture()->textureArray());
            mChisel->resourceManager()->commitFreeImageUnit(targetLayer->maskTexture()->textureArray());            
        }
        else
            targetLayer = *search;

        std::string dataImageName = "Data" + std::to_string(targetLayer->dataTexture()->textureArray()->imageUnit());
        std::string maskImageName = "Mask" + std::to_string(targetLayer->maskTexture()->textureArray()->imageUnit());
        
        updateImageDeclarations(dataImageName, maskImageName, targetLayer);
            
        mEndCode += "    if(" + info.mask + ")\n"
                    "    {\n"
                    "        imageStore(" + dataImageName + ", ivec3(pixelCoords, " + std::to_string(targetLayer->dataTexture()->textureArrayLayerIndex()) + "), " + prefixImageFormat(targetLayer->dataTexture()->internalFormat()) + "vec4(" + info.data + "));\n"
                    "        imageStore(" + maskImageName + ", ivec3(pixelCoords, " + std::to_string(targetLayer->maskTexture()->textureArrayLayerIndex()) + "), vec4(" + info.mask + "));\n"
                    "    }\n" 
                    "}\n";
        
        mSources.push_back(mImageDeclarations + mInitCode + mDeclarationCode + mSourceString + mEndCode);
        
        mDeclarationCode.clear();
        mSourceString.clear();
        mEndCode.clear();
        
        mImages.clear();
        mIdentifiers.clear();
        
        mImageDeclarations =    "#version 450\n"
                                "#extension GL_ARB_compute_variable_group_size : enable\n\n"
                                "layout(local_size_variable) in;\n";
    }
}

void Translator::exitParExp(OpLanguage::OParser::ParExpContext* ctx)
{
    auto info = nodeInfo(ctx->expr());
    setNodeInfo(ctx, {"(" + info.data + ")", "(" + info.mask + ")", info.type, info.resolution});
}

void Translator::exitFunctionExp(OParser::FunctionExpContext* ctx)
{
    setNodeInfo(ctx, nodeInfo(ctx->function()));
}

void Translator::exitConstantExp(OParser::ConstantExpContext* ctx)
{
    setNodeInfo(ctx, nodeInfo(ctx->constant()));
}

void Translator::exitUnitaryExp(OpLanguage::OParser::UnitaryExpContext* ctx)
{
    auto unaryMask = "expMask" + std::to_string(mMaskCount);
    auto unaryData = "expData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + unaryMask + ";\n" +
                     toGLSLType(info.type) + " " + unaryData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + unaryData + " = " + ctx->unitaryOp()->toString() + info.data + "; \n" + 
                        "    " + unaryMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + unaryMask + " = false;\n";
                        
    setNodeInfo(ctx, {unaryData, unaryMask, info.type, info.resolution});
    mDataCount++; mMaskCount++;      
}

void Translator::exitExponentialExp(OpLanguage::OParser::ExponentialExpContext* ctx)
{
    auto exp2Mask = "expMask" + std::to_string(mMaskCount);
    auto exp2Data = "expData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoY = nodeInfo(ctx->expr(1));
    
    mSourceString += "bool " + exp2Mask + ";\n" +
                     toGLSLType(infoX.type) + " " + exp2Data + ";\n";
                     
    mSourceString +=    "if(" + infoX.mask + " && " + infoY.mask + " && " + infoX.data + " >= 0 && floor(" + infoY.data + ") == " + infoY.data +")\n" +
                        "{\n" +                    
                        "    " + exp2Data + " = pow(" + infoY.data + ", " + infoX.data + "); \n" + 
                        "    " + exp2Mask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + exp2Mask + " = false;\n";
                        
    setNodeInfo(ctx, {exp2Data, exp2Mask, infoX.type, infoX.resolution});
    mDataCount++; mMaskCount++;     
}

void Translator::exitBinaryOpExp(OpLanguage::OParser::BinaryOpExpContext* ctx)
{     
    auto expMask = "expMask" + std::to_string(mMaskCount);    
    auto expData = "expData" + std::to_string(mDataCount);
    auto infoA = nodeInfo(ctx->expr(0));
    auto infoB = nodeInfo(ctx->expr(1));
    auto binOpLType = implicitTypeConversion(infoA.type, infoB.type);
    auto binOpResolution = std::max(infoA.resolution, infoB.resolution);
    
    mSourceString += "bool " + expMask + ";\n" +
                     toGLSLType(binOpLType) + " " + expData + ";\n";
    
    mSourceString += "if(" + infoA.mask + " && " + infoB.mask + ")\n" +
                    "{\n" +
                    "    " + expData + " = " + infoA.data + " " + ctx->op->getText() + " " + infoB.data + ";\n" +
                    "    " + expMask + " = " + infoA.mask + " && " + infoB.mask + ";\n" +
                    "}\n" +                    
                    "else\n" +
                    "    " + expMask + " = false;\n";
    
    setNodeInfo(ctx, {expData, expMask, binOpLType, binOpResolution});
    mDataCount++; mMaskCount++;
}

void Translator::exitBinLogOpExp(OpLanguage::OParser::BinLogOpExpContext* ctx)
{
    auto expMask = "expMask" + std::to_string(mMaskCount);    
    auto expData = "expData" + std::to_string(mDataCount);
    
    mSourceString += "bool " + expMask + ";\n" +
                     "bool " + expData + ";\n";
    
    mSourceString += "if(" + nodeInfo(ctx->expr(0)).mask + " && " + nodeInfo(ctx->expr(1)).mask + ")\n" +
                    "{\n" +
                    "    " + expData + " = " + nodeInfo(ctx->expr(0)).data + " " + ctx->op->getText() + " " + nodeInfo(ctx->expr(1)).data + ";\n" +
                    "    " + expMask + " = " + nodeInfo(ctx->expr(0)).data + " " + ctx->op->getText() + " " + nodeInfo(ctx->expr(1)).data + ";\n" +
                    "}\n" +                    
                    "else\n" +
                    "    " + expMask + " = false;\n";
    
    setNodeInfo(ctx, {expData, expMask});
    mDataCount++; mMaskCount++;
}


void Translator::exitLogAndExp(OpLanguage::OParser::LogAndExpContext* ctx)
{
    auto expMask = "logAndMask" + std::to_string(mMaskCount);    
    auto expData = "logAndData" + std::to_string(mDataCount);
    
    mSourceString += "bool " + expMask + ";\n" +
                     "bool " + expData + ";\n";
    
    mSourceString += "if(" + nodeInfo(ctx->expr(0)).mask + " && " + nodeInfo(ctx->expr(1)).mask + ")\n" +
                    "{\n" +
                    "    " + expData + " = " + nodeInfo(ctx->expr(0)).data + " && " + nodeInfo(ctx->expr(1)).data + ";\n" +
                    "    " + expMask + " = " + nodeInfo(ctx->expr(0)).data + " && " + nodeInfo(ctx->expr(1)).data + ";\n" +
                    "}\n" +                    
                    "else\n" +
                    "    " + expMask + " = false;\n";
    
    setNodeInfo(ctx, {expData, expMask});
    mDataCount++; mMaskCount++;    
}

void Translator::exitLogOrExp(OpLanguage::OParser::LogOrExpContext* ctx)
{
    auto expMask = "logOrMask" + std::to_string(mMaskCount);    
    auto expData = "logOrData" + std::to_string(mDataCount);
    
    mSourceString += "bool " + expMask + ";\n" +
                     "bool " + expData + ";\n";
    
    mSourceString += "if(" + nodeInfo(ctx->expr(0)).mask + " || " + nodeInfo(ctx->expr(1)).mask + ")\n" +
                    "{\n" +
                    "    " + expData + " = " + nodeInfo(ctx->expr(0)).data + " || " + nodeInfo(ctx->expr(1)).data + ";\n" +
                    "    " + expMask + " = " + nodeInfo(ctx->expr(0)).data + " || " + nodeInfo(ctx->expr(1)).data + ";\n" +
                    "}\n" +                    
                    "else\n" +
                    "    " + expMask + " = false;\n";
    
    setNodeInfo(ctx, {expData, expMask});
    mDataCount++; mMaskCount++;        
}

void Translator::exitShiftExp(OParser::ShiftExpContext* ctx)
{
    auto shiftMask = "shiftMask" + std::to_string(mMaskCount);    
    auto shiftData = "shiftData" + std::to_string(mDataCount);
    auto infoA = nodeInfo(ctx->expr(0));
    auto infoB = nodeInfo(ctx->expr(1));
    
    if(infoA.type < Layer::Type::Float16)
    {
        if(infoB.type < Layer::Type::Float16)
        {
            mSourceString += "bool " + shiftMask + ";\n" +
                            "int " + shiftData + ";\n";
            
            mSourceString += "if(" + infoA.mask + " && " + infoB.mask + ")\n" +
                            "{\n" +
                            "    " + shiftData + " = " + infoA.data + ctx->op->getText() + infoB.data + ";\n" +
                            "    " + shiftMask + " = " + infoA.mask + " && " + infoB.mask + ";\n" +
                            "}\n" +                    
                            "else\n" +
                            "    " + shiftMask + " = false;\n";
            
            setNodeInfo(ctx, {shiftData, shiftMask});
            mDataCount++; mMaskCount++;
        }
        else
        {        
            auto tokenB = ctx->expr(1)->getStart();
            auto tokenBEnd = ctx->expr(1)->getStop();
            
            for(auto i = tokenB->getTokenIndex(); i <= tokenBEnd->getTokenIndex(); ++i)
            {
                auto token = mTokens->get(i);
                mOffendingSymbols[static_cast<int>(token->getStartIndex())] = token;
            }
                        
            mSemanticErrorMessages += "line " + std::to_string(tokenB->getLine()) + " : position " + std::to_string(tokenB->getCharPositionInLine()) + ": The type of the right operand needs to be an integer";            
        }
    }
    else
    {
        auto tokenA = ctx->expr(0)->getStart();
        auto tokenAEnd = ctx->expr(0)->getStop();
        
        for(auto i = tokenA->getTokenIndex(); i <= tokenAEnd->getTokenIndex(); ++i)
        {
            auto token = mTokens->get(i);
            mOffendingSymbols[static_cast<int>(token->getStartIndex())] = token;
        }
        
        mSemanticErrorMessages += "line " + std::to_string(tokenA->getLine()) + " : position " + std::to_string(tokenA->getCharPositionInLine()) + ": The type of the left operand needs to be an integer";
    }
}

void Translator::exitCondExp(OpLanguage::OParser::CondExpContext* ctx)
{
    // expr '?' expr ':' expr
    auto ternaryMask = "ternaryMask" + std::to_string(mMaskCount);
    auto ternaryData = "ternaryData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoA = nodeInfo(ctx->expr(1));
    auto infoB = nodeInfo(ctx->expr(2));
    auto ternaryType = implicitTypeConversion(infoA.type, infoB.type);
    
    
    mSourceString +=    "bool " + ternaryMask + ";\n" +
                        toGLSLType(ternaryType) + " " + ternaryData + ";\n";    
    
    mSourceString +=    "if(" + infoX.mask + ")\n" +
                        "{\n" +                    
                        "    if (" + infoX.data + ")\n" + 
                        "    {\n" +                    
                        "        " + ternaryData + " = " + infoA.data + ";\n" + 
                        "        " + ternaryMask + " = " + infoA.mask +";\n" +
                        "    }\n" +
                        "    else\n" +
                        "    {\n" +                    
                        "        " + ternaryData + " = " + infoB.data + ";\n" + 
                        "        " + ternaryMask + " = " + infoB.mask +";\n" +
                        "    }\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + ternaryMask + " = false;\n";                     
        
    setNodeInfo(ctx, {ternaryData, ternaryMask, ternaryType});
    mDataCount++; mMaskCount++;       
}


void Translator::exitAbs(OParser::AbsContext* ctx)
{
    auto absMask = "absMask" + std::to_string(mMaskCount);
    auto absData = "absData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + absMask + ";\n" +
                     toGLSLType(info.type) + " " + absData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + absData + " = abs(" + info.data + "); \n" + 
                        "    " + absMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + absMask + " = false;\n";
                        
    setNodeInfo(ctx, {absData, absMask, info.type, info.resolution});
    mDataCount++; mMaskCount++;                          
}

void Translator::exitAcos(OpLanguage::OParser::AcosContext* ctx)
{
    auto acosMask = "acosMask" + std::to_string(mMaskCount);
    auto acosData = "acosData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + acosMask + ";\n" +
                     "int " + acosData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + " && abs(" + info.data + ") <= 1.0)\n" +
                        "{\n" +                    
                        "    " + acosData + " = degrees(acos(" + info.data + ")); \n" + 
                        "    " + acosMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + acosMask + " = false;\n";
                        
    setNodeInfo(ctx, {acosData, acosMask, Layer::Type::Int32, info.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitAsin(OpLanguage::OParser::AsinContext* ctx)
{
    auto asinMask = "asinMask" + std::to_string(mMaskCount);
    auto asinData = "asinData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + asinMask + ";\n" +
                     "int " + asinData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + " && abs(" + info.data + ") <= 1.0)\n" +
                        "{\n" +                    
                        "    " + asinData + " = degrees(asin(" + info.data + ")); \n" + 
                        "    " + asinMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + asinMask + " = false;\n";
                        
    setNodeInfo(ctx, {asinData, asinMask, Layer::Type::Int32, info.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitAtan(OpLanguage::OParser::AtanContext* ctx)
{
    auto atanMask = "asinMask" + std::to_string(mMaskCount);
    auto atanData = "asinData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + atanMask + ";\n" +
                     "int " + atanData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + atanData + " = degrees(asin(" + info.data + ")); \n" + 
                        "    " + atanMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + atanMask + " = false;\n";
                        
    setNodeInfo(ctx, {atanData, atanMask, Layer::Type::Int32, info.resolution});
    mDataCount++; mMaskCount++;        
}

void Translator::exitAtan2(OpLanguage::OParser::Atan2Context* ctx)
{
    auto atan2Mask = "atanMask" + std::to_string(mMaskCount);
    auto atan2Data = "atanData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoY = nodeInfo(ctx->expr(1));
    
    mSourceString += "bool " + atan2Mask + ";\n" +
                     "int " + atan2Data + ";\n";
                     
    mSourceString +=    "if(" + infoX.mask + " && " + infoY.mask + " && " + infoX.data + " != 0.0)\n" +
                        "{\n" +                    
                        "    " + atan2Data + " = degrees(atan(" + infoY.data + ", " + infoX.data + ")); \n" + 
                        "    " + atan2Mask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + atan2Mask + " = false;\n";
                        
    setNodeInfo(ctx, {atan2Data, atan2Mask, Layer::Type::Int32, infoX.resolution});
    mDataCount++; mMaskCount++;        
}

void Translator::exitCos(OpLanguage::OParser::CosContext* ctx)
{
    auto cosMask = "cosMask" + std::to_string(mMaskCount);
    auto cosData = "cosData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + cosMask + ";\n" +
                     "float " + cosData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + cosData + " = cos(" + info.data + "); \n" + 
                        "    " + cosMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + cosMask + " = false;\n";
                        
    setNodeInfo(ctx, {cosData, cosMask, Layer::Type::Float32, info.resolution});
    mDataCount++; mMaskCount++;     
}

void Translator::exitExponential(OpLanguage::OParser::ExponentialContext* ctx)
{
    auto expMask = "expMask" + std::to_string(mMaskCount);
    auto expData = "expData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + expMask + ";\n" +
                     toGLSLType(info.type) + " " + expData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + expData + " = exp(" + info.data + "); \n" + 
                        "    " + expMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + expMask + " = false;\n";
                        
    setNodeInfo(ctx, {expData, expMask, info.type, info.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitExponential2(OpLanguage::OParser::Exponential2Context* ctx)
{
    auto exp2Mask = "expMask" + std::to_string(mMaskCount);
    auto exp2Data = "expData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoY = nodeInfo(ctx->expr(1));
    
    mSourceString += "bool " + exp2Mask + ";\n" +
                     toGLSLType(infoX.type) + " " + exp2Data + ";\n";
                     
    mSourceString +=    "if(" + infoX.mask + " && " + infoY.mask + " && " + infoX.data + " >= 0 && floor(" + infoY.data + ") == " + infoY.data +")\n" +
                        "{\n" +                    
                        "    " + exp2Data + " = pow(" + infoY.data + ", " + infoX.data + "); \n" + 
                        "    " + exp2Mask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + exp2Mask + " = false;\n";
                        
    setNodeInfo(ctx, {exp2Data, exp2Mask, infoX.type, infoX.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitCastToFloat(OpLanguage::OParser::CastToFloatContext* ctx)
{
    auto floatData = "floatData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());

    mSourceString += "float " + floatData + " = float(" + info.data + ");\n";    
    
    setNodeInfo(ctx, {floatData, info.mask, info.type, info.resolution});
    mDataCount++; mMaskCount++;
}


void Translator::exitIf(OParser::IfContext* ctx)
{
    auto ifMask = "ifMask" + std::to_string(mMaskCount);
    auto ifData = "ifData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());

    mSourceString += "bool " + ifMask + ";\n" +
                     "int " + ifData + ";\n";    
    
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + ifData + " = (" + info.data + ") ? 1 : 0; \n" + 
                        "    " + ifMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + ifMask + " = false;\n";                     
    
    setNodeInfo(ctx, {ifData, ifMask, Layer::Type::Int8});
    mDataCount++; mMaskCount++;    
}

void Translator::exitIf2(OParser::If2Context* ctx)
{
    auto ifMask = "ifMask" + std::to_string(mMaskCount);
    auto ifData = "ifData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoA = nodeInfo(ctx->expr(1));    
    
    mSourceString += "bool " + ifMask + ";\n" +
                     toGLSLType(infoA.type) + " " + ifData + ";\n";    
    
    mSourceString +=    "if(" + infoX.mask + ")\n" +
                        "{\n" +                    
                        "    if (" + infoX.data + ")\n" + 
                        "    {\n" +                    
                        "        " + ifData + " = " + infoA.data + ";\n" + 
                        "        " + ifMask + " = " + infoA.mask +";\n" +
                        "    }\n" +
                        "    else\n" +
                        "    {\n" +                    
                        "        " + ifData + " = 0;\n" + 
                        "        " + ifMask + " = true;\n" +
                        "    }\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + ifMask + " = false;\n";                     
        
    setNodeInfo(ctx, {ifData, ifMask, infoA.type});
    mDataCount++; mMaskCount++;     
}

void Translator::exitIf3(OParser::If3Context* ctx)
{
    auto ifMask = "ifMask" + std::to_string(mMaskCount);
    auto ifData = "ifData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoA = nodeInfo(ctx->expr(1));
    auto infoB = nodeInfo(ctx->expr(2));
    auto ifType = implicitTypeConversion(infoA.type, infoB.type);
    
    
    mSourceString +=    "bool " + ifMask + ";\n" +
                        toGLSLType(ifType) + " " + ifData + ";\n";    
    
    mSourceString +=    "if(" + infoX.mask + ")\n" +
                        "{\n" +                    
                        "    if (" + infoX.data + ")\n" + 
                        "    {\n" +                    
                        "        " + ifData + " = " + infoA.data + ";\n" + 
                        "        " + ifMask + " = " + infoA.mask +";\n" +
                        "    }\n" +
                        "    else\n" +
                        "    {\n" +                    
                        "        " + ifData + " = " + infoB.data + ";\n" + 
                        "        " + ifMask + " = " + infoB.mask +";\n" +
                        "    }\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + ifMask + " = false;\n";                     
        
    setNodeInfo(ctx, {ifData, ifMask, ifType});
    mDataCount++; mMaskCount++;     
}

void Translator::exitIf4(OParser::If4Context* ctx)
{
    auto ifMask = "ifMask" + std::to_string(mMaskCount);
    auto ifData = "ifData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoA = nodeInfo(ctx->expr(1));
    auto infoB = nodeInfo(ctx->expr(2));
    auto infoC = nodeInfo(ctx->expr(3));
    auto ifType = implicitTypeConversion(implicitTypeConversion(infoA.type, infoB.type), infoB.type);
            
    mSourceString +=    "bool " + ifMask + ";\n" +
                        toGLSLType(ifType) + " " + ifData + ";\n";    
    
    mSourceString +=    "if(" + infoX.mask + ")\n" +
                        "{\n" +                    
                        "    if (" + infoX.data + " > 0)\n" + 
                        "    {\n" +                    
                        "        " + ifData + " = " + infoA.data + ";\n" + 
                        "        " + ifMask + " = " + infoA.mask +";\n" +
                        "    }\n" +
                        "    else if(" + nodeInfo(ctx->expr(0)).data + " == 0)\n" + 
                        "    {\n" +                    
                        "        " + ifData + " = " + infoB.data + ";\n" + 
                        "        " + ifMask + " = " + infoB.mask +";\n" +
                        "    }\n" +
                        "    else if(" + nodeInfo(ctx->expr(0)).data + " < 0)\n" + 
                        "    {\n" +                    
                        "        " + ifData + " = " + infoC.data + ";\n" + 
                        "        " + ifMask + " = " + infoC.mask +";\n" +
                        "    }\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + ifMask + " = false;\n";                     
        
    setNodeInfo(ctx, {ifData, ifMask, ifType});
    mDataCount++; mMaskCount++; 
}

void Translator::exitCastToInt(OpLanguage::OParser::CastToIntContext* ctx)
{
    auto intData = "intData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());

    mSourceString += "int " + intData + " = int(" + info.data + ");\n";    
    
    setNodeInfo(ctx, {intData, info.mask, info.type, info.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitIsNull(OpLanguage::OParser::IsNullContext* ctx)
{
    auto isNullMask = "isNullMask" + std::to_string(mMaskCount);
    auto isNullData = "isNullData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString +=    "bool " + isNullData + ";\n";
                     
    mSourceString +=    "(" + info.mask + ") ? false : true;\n";
                        
    setNodeInfo(ctx, {isNullData, "true", Layer::Type::Int8, info.resolution});
    mDataCount++; mMaskCount++;       
}

void Translator::exitLog(OpLanguage::OParser::LogContext* ctx)
{
    auto logMask = "logMask" + std::to_string(mMaskCount);
    auto logData = "logData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + logMask + ";\n" +
                     "float " + logData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + " && " + info.data + " > 0)\n" +
                        "{\n" +                    
                        "    " + logData + " = log(" + info.data + "); \n" + 
                        "    " + logMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + logMask + " = false;\n";
                        
    setNodeInfo(ctx, {logData, logMask, Layer::Type::Float32, info.resolution});
    mDataCount++; mMaskCount++;     
}

void Translator::exitLog2(OpLanguage::OParser::Log2Context* ctx)
{
    auto log2Mask = "logXBMask" + std::to_string(mMaskCount);
    auto log2Data = "logXBData" + std::to_string(mDataCount);
    auto infoX = nodeInfo(ctx->expr(0));
    auto infoB = nodeInfo(ctx->expr(1));
    
    mSourceString += "bool " + log2Mask + ";\n" +
                     "float " + log2Data + ";\n";
                     
    mSourceString +=    "if(" + infoX.mask + " && " + infoB.mask + " && " + infoX.data + " > 0 && floor(" + infoB.data + ") == " + infoB.data +")\n" +
                        "{\n" +                    
                        "    " + log2Data + " = log(" + infoX.data + ")/log(" + infoB.data + "); \n" + 
                        "    " + log2Mask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + log2Mask + " = false;\n";
                        
    setNodeInfo(ctx, {log2Data, log2Mask, Layer::Type::Float32, infoX.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitMax(OpLanguage::OParser::MaxContext* ctx)
{
    auto maxMask = "maxMask" + std::to_string(mMaskCount);
    auto maxData = "maxData" + std::to_string(mDataCount);
    auto param0Info = nodeInfo(ctx->expr(0));
    auto maxType = param0Info.type; 
    
    std::string ifCondition = param0Info.mask ;
    std::string prependMax = maxData + " = ";
    std::string appendMax = param0Info.data;
    
    for(auto i = 1; i < ctx->expr().size(); ++i)
    {
        auto paramInfo = nodeInfo(ctx->expr(i));        
        ifCondition += " && " + paramInfo.mask;
        prependMax += "max(";
        appendMax += ", " + paramInfo.data + ")";
        maxType = implicitTypeConversion(maxType, paramInfo.type);
    }
    
    mSourceString +=    "bool " + maxMask + ";\n" +
                        toGLSLType(maxType) + " " + maxData + ";\n"
                        "if(" + ifCondition + ")\n"
                        "{\n" +
                        "    " + prependMax + appendMax + ";\n"
                        "    " + maxMask + " = true;\n"
                        "}\n"                    
                        "else\n"
                        "    " + maxMask + " = false;\n";
                        
    setNodeInfo(ctx, {maxData, maxMask, maxType, param0Info.resolution});
    mDataCount++; mMaskCount++;         
}

void Translator::exitMedian(OpLanguage::OParser::MedianContext* ctx)
{
    auto medianMask = "medianMask" + std::to_string(mMaskCount);
    auto medianVector = "medianVector" + std::to_string(mDataCount);
    auto medianSize = std::to_string(ctx->expr().size());
    auto medianData = "medianData" + std::to_string(mDataCount);
    auto param0Info = nodeInfo(ctx->expr(0));
    auto medianType = param0Info.type; 
    
    std::string ifCondition = param0Info.mask ;
    std::string medianOperands =  param0Info.data;
    
    for(auto i = 1; i < ctx->expr().size(); ++i)
    {
        auto paramInfo = nodeInfo(ctx->expr(i));        
        ifCondition += " && " + paramInfo.mask;
        medianOperands += " , " + paramInfo.data;
        medianType = implicitTypeConversion(medianType, paramInfo.type);
    }
    
    mSourceString +=    "bool " + medianMask + ";\n" +
                        toGLSLType(medianType) + " " + medianVector + "[" + medianSize + "] = {" + medianOperands + "};\n" +
                        "float " + medianData + ";\n"
                        "if(" + ifCondition + ")\n"
                        "{\n"
                        "    " + toGLSLType(medianType) + " temp;\n"
                        "    for (int j = 0; j < " + medianSize + "-1; ++j)\n"
                        "    {\n"
                        "        " + toGLSLType(medianType) + " swap = j;\n"
                        "        for (int i = j+1; i < " +  medianSize + "; ++i)\n"
                        "        {\n"
                        "            if (" + medianVector + "[swap] > " + medianVector + "[i])\n"
                        "                swap = i;\n"
                        "        }\n"
                        "        temp = " + medianVector + "[swap];\n"
                        "        " + medianVector + "[swap] = " + medianVector + "[j];\n"
                        "        " + medianVector + "[j] = temp;\n"
                        "    }\n"
                        "    if(" + medianSize + " % 2 == 0)\n"
                        "        " + medianData + " = (" + medianVector + "[" + medianSize + "/2] + " + medianVector + "[" + medianSize + "/2 - 1])/2.0;\n"
                        "    else\n"
                        "        " + medianData + " = " + medianVector + "[" + medianSize + "/2];\n"
                        "    " + medianMask + " = true;\n"
                        "}\n"                    
                        "else\n"
                        "    " + medianMask + " = false;\n";
                        
    setNodeInfo(ctx, {medianData, medianMask, Layer::Type::Float32, param0Info.resolution});
    mDataCount++; mMaskCount++;     
}

void Translator::exitMin(OpLanguage::OParser::MinContext* ctx)
{
    auto minMask = "minMask" + std::to_string(mMaskCount);
    auto minData = "minData" + std::to_string(mDataCount);
    auto param0Info = nodeInfo(ctx->expr(0));
    auto minType = param0Info.type; 
    
    std::string ifCondition = param0Info.mask ;
    std::string prependMin = minData + " = ";
    std::string appendMin = param0Info.data;
    
    for(auto i = 1; i < ctx->expr().size(); ++i)
    {
        auto paramInfo = nodeInfo(ctx->expr(i));        
        ifCondition += " && " + paramInfo.mask;
        prependMin += "min(";
        appendMin += ", " + paramInfo.data + ")";
        minType = implicitTypeConversion(minType, paramInfo.type);
    }
    
    mSourceString +=    "bool " + minMask + ";\n" +
                        toGLSLType(minType) + " " + minData + ";\n"
                        "if(" + ifCondition + ")\n"
                        "{\n" +
                        "    " + prependMin + appendMin + ";\n"
                        "    " + minMask + " = true;\n"
                        "}\n"                    
                        "else\n"
                        "    " + minMask + " = false;\n";
                        
    setNodeInfo(ctx, {minData, minMask, minType, param0Info.resolution});
    mDataCount++; mMaskCount++;       
}

void Translator::exitMathMode(OpLanguage::OParser::MathModeContext* ctx)
{
    auto modeMask = "modeMask" + std::to_string(mMaskCount);
    auto modeVector = "modeVector" + std::to_string(mDataCount);
    auto modeSize = std::to_string(ctx->expr().size());
    auto modeData = "modeData" + std::to_string(mDataCount);
    auto param0Info = nodeInfo(ctx->expr(0));
    auto modeType = param0Info.type; 
    
    std::string ifCondition = param0Info.mask ;
    std::string modeOperands =  param0Info.data;
    
    for(auto i = 1; i < ctx->expr().size(); ++i)
    {
        auto paramInfo = nodeInfo(ctx->expr(i));        
        ifCondition += " && " + paramInfo.mask;
        modeOperands += " , " + paramInfo.data;
        modeType = implicitTypeConversion(modeType, paramInfo.type);
    }
    
    mSourceString +=    toGLSLType(modeType) + " " + modeVector + "[" + modeSize + "] = {" + modeOperands + "};\n" +
                        "bool " + modeMask + ";\n" +
                        toGLSLType(modeType) + " " + modeData + ";\n"
                        "if(" + ifCondition + ")\n"
                        "{\n"
                        "    " + toGLSLType(modeType) + " temp;\n"
                        "    for (int j = 0; j < " + modeSize + "-1; ++j)\n"
                        "    {\n"
                        "        " + toGLSLType(modeType) + " swap = j;\n"
                        "        for (int i = j+1; i < " +  modeSize + "; ++i)\n"
                        "        {\n"
                        "            if (" + modeVector + "[swap] > " + modeVector + "[i])\n"
                        "                swap = i;\n"
                        "        }\n"
                        "        temp = " + modeVector + "[swap];\n"
                        "        " + modeVector + "[swap] = " + modeVector + "[j];\n"
                        "        " + modeVector + "[j] = temp;\n"
                        "    }\n"
                        "    " + toGLSLType(modeType) + " number = " + modeVector + "[0];\n"
                        "    " + toGLSLType(modeType) + " mode = number;\n"
                        "    int count = 1;\n"
                        "    int countMode = 1;\n\n"
                        
                        "    for(int i = 1; i < " + modeSize + "; i++)\n"
                        "    {\n"
                        "        if (" + modeVector +" [i] == number)\n" 
                        "        {\n"
                        "            ++count;\n"
                        "        }\n"
                        "        else\n"
                        "        {\n"
                        "            if (count > countMode)\n"
                        "            {\n"
                        "                countMode = count;\n"
                        "                mode = number;\n"
                        "            }\n"
                        "            count = 1;\n"
                        "            number = " + modeVector + "[i];\n"
                        "        }\n"
                        "    }\n"
                        "    " + modeData + " = mode;\n"
                        "    " + modeMask + " = true;\n"
                        "}\n"                    
                        "else\n"
                        "    " + modeMask + " = false;\n";
                        
    setNodeInfo(ctx, {modeData, modeMask, modeType, param0Info.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitNot(OpLanguage::OParser::NotContext* ctx)
{
}

void Translator::exitRand(OpLanguage::OParser::RandContext* ctx)
{
    auto randMask = "randMask" + std::to_string(mMaskCount);
    auto randData = "randData" + std::to_string(mDataCount);
    auto infoA = nodeInfo(ctx->expr(0));
    auto infoB = nodeInfo(ctx->expr(1));
    auto randType = implicitTypeConversion(infoA.type, infoB.type);
    
    mSourceString += "bool " + randMask + ";\n" +
                     toGLSLType(randType) + " " + randData + ";\n";
                     
    mSourceString +=    "if(" + infoA.mask + " && " + infoB.mask + ")\n" +
                        "{\n" +
                        "    float a = 12.9898;\n"
                        "    float b = 78.233;\n"
                        "    float c = 43758.5453;\n"
                        "    float dt = dot(pixelCoords.xy, vec2(a, b));\n"
                        "    float sn = mod(dt, 3.14);\n"
                        "    " + randData + " = " + toGLSLType(randType) + "((" + infoB.data + " - " + infoA.data + ") * fract(sin(sn) * c)); \n" + 
                        "    " + randMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + randMask + " = false;\n";
                        
    setNodeInfo(ctx, {randData, randMask, randType, infoA.resolution});
    mDataCount++; mMaskCount++;    
}

void Translator::exitRound(OpLanguage::OParser::RoundContext* ctx)
{
    auto roundMask = "roundMask" + std::to_string(mMaskCount);
    auto roundData = "roundData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + roundMask + ";\n" +
                     "int " + roundData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + " )\n" +
                        "{\n" +                    
                        "    " + roundData + " = int(round(" + info.data + ")); \n" + 
                        "    " + roundMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + roundMask + " = false;\n";
                        
    setNodeInfo(ctx, {roundData, roundMask, Layer::Type::Int32, info.resolution});
    mDataCount++; mMaskCount++;     
}

void Translator::exitRound2(OpLanguage::OParser::Round2Context* ctx)
{
}

void Translator::exitRound3(OpLanguage::OParser::Round3Context* ctx)
{
}

void Translator::exitSin(OpLanguage::OParser::SinContext* ctx)
{
    auto sinMask = "sinMask" + std::to_string(mMaskCount);
    auto sinData = "sinData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + sinMask + ";\n" +
                     "float " + sinData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + sinData + " = sin(" + info.data + "); \n" + 
                        "    " + sinMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + sinMask + " = false;\n";
                        
    setNodeInfo(ctx, {sinData, sinMask, Layer::Type::Float32, {0, 0}});
    mDataCount++; mMaskCount++;    
}


void Translator::exitSqrt(OParser::SqrtContext* ctx)
{
    auto sqrtMask = "absMask" + std::to_string(mMaskCount);
    auto sqrtData = "absData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + sqrtMask + ";\n" +
                     "float " + sqrtData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + " && " + info.data + ">= 0)\n" +
                        "{\n" +                    
                        "    " + sqrtData + " = sqrt(" + info.data + "); \n" + 
                        "    " + sqrtMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + sqrtMask + " = false;\n";
                        
    setNodeInfo(ctx, {sqrtData, sqrtMask, Layer::Type::Float32, {0, 0}});
    mDataCount++; mMaskCount++;                          
}

void Translator::exitTan(OpLanguage::OParser::TanContext* ctx)
{
    auto tanMask = "tanMask" + std::to_string(mMaskCount);
    auto tanData = "tanData" + std::to_string(mDataCount);
    auto info = nodeInfo(ctx->expr());
    
    mSourceString += "bool " + tanMask + ";\n" +
                     "float " + tanData + ";\n";
                     
    mSourceString +=    "if(" + info.mask + ")\n" +
                        "{\n" +                    
                        "    " + tanData + " = tan(" + info.data + "); \n" + 
                        "    " + tanMask + " = true;\n" +
                        "}\n" +                    
                        "else\n" +
                        "    " + tanMask + " = false;\n";
                        
    setNodeInfo(ctx, {tanData, tanMask, Layer::Type::Float32, {0, 0}});
    mDataCount++; mMaskCount++;    
}


void Translator::exitNull(OParser::NullContext* ctx)
{
    setNodeInfo(ctx, {"0", "false", Layer::Type::Int8, {0, 0}});
}

void Translator::exitSql(OpLanguage::OParser::SqlContext* ctx)
{
    auto layer = mChisel->resourceManager()->layer(ctx->NAME(0)->getText());
    
    if(layer != nullptr && layer->registerType())
    {
        auto regLayer = dynamic_cast<const RegisterLayer *>(layer);
        
        if(regLayer->tableSchema()->containField(ctx->NAME(1)->toString()))        
        {
            auto field = regLayer->tableSchema()->field(ctx->NAME(1)->getText());
            auto sqlLayer = mChisel->resourceManager()->createLayerFromTableField(regLayer, field);
            
            mTempLayers.push_back(sqlLayer);
            
            std::string dataImageName = "Data" + std::to_string(sqlLayer->dataTexture()->textureArray()->imageUnit());        
            std::string maskImageName = "Mask" + std::to_string(sqlLayer->maskTexture()->textureArray()->imageUnit());

            updateImageDeclarations(dataImageName, maskImageName, sqlLayer);

            auto parsedSearch = std::find_if(begin(mIdentifiers), end(mIdentifiers), [&](const std::string& id){ return (ctx->NAME(0)->getText() == id) ? true : false;});
            
            if(parsedSearch == end(mIdentifiers))
            {
                mDeclarationCode += "    " + toGLSLType(sqlLayer->type()) + " " + sqlLayer->name() + "Data = imageLoad(" + dataImageName + ", ivec3(pixelCoords, " + std::to_string(sqlLayer->dataTexture()->textureArrayLayerIndex()) + ")).x;\n";
                mDeclarationCode += "    float " + sqlLayer->name() + "Mask = imageLoad(" + maskImageName + ", ivec3(pixelCoords, " + std::to_string(sqlLayer->maskTexture()->textureArrayLayerIndex()) + ")).x;\n";
                mIdentifiers.push_back(ctx->NAME(0)->getText());
            }
            
            if(field.mType == DataBaseField::Type::Integer)
                setNodeInfo(ctx, {sqlLayer->name() + "Data", sqlLayer->name() + "Mask > 0", Layer::Type::Int32, sqlLayer->resolution()});
            else
                setNodeInfo(ctx, {sqlLayer->name() + "Data", sqlLayer->name() + "Mask > 0", Layer::Type::Float32, sqlLayer->resolution()});
        }
        else
        {
            auto token = ctx->NAME(1)->getSymbol();
            mOffendingSymbols[static_cast<int>(token->getStartIndex())] = token;
            mSemanticErrorMessages += "line " + std::to_string(token->getLine()) + " : position " + std::to_string(token->getCharPositionInLine()) + ": " + ctx->NAME(1)->toString() + " is not a valid field of the layer " + ctx->NAME(0)->getText() + "\n";                    
        }
    }
    else
    {
        auto token = ctx->NAME(0)->getSymbol();
        mOffendingSymbols[static_cast<int>(token->getStartIndex())] = token;
        mSemanticErrorMessages += "line " + std::to_string(token->getLine()) + " : position " + std::to_string(token->getCharPositionInLine()) + ": " + ctx->NAME(0)->getText() + " is not a valid register layer\n";        
    }
}



void Translator::exitIdentifier(OParser::IdentifierContext* ctx)
{
    const auto& layers = mChisel->resourceManager()->layers();
    auto search = std::find_if(begin(layers), end(layers), [&](Layer *layer){ return (ctx->getText() == layer->name()) ? true : false;});
        
    if(search != end(layers))
    {
        mChisel->resourceManager()->commitFreeImageUnit((*search)->dataTexture()->textureArray());
        mChisel->resourceManager()->commitFreeImageUnit((*search)->maskTexture()->textureArray());
        std::string dataImageName = "Data" + std::to_string( (*search)->dataTexture()->textureArray()->imageUnit());        
        std::string maskImageName = "Mask" + std::to_string( (*search)->maskTexture()->textureArray()->imageUnit());
        
        updateImageDeclarations(dataImageName, maskImageName, *search);
                        
        auto parsedSearch = std::find_if(begin(mIdentifiers), end(mIdentifiers), [&](const std::string& id){ return (ctx->getText() == id) ? true : false;});
        
        if(parsedSearch == end(mIdentifiers))
        {
            mDeclarationCode += "    " + toGLSLType((*search)->type()) + " " + (*search)->name() + "Data = imageLoad(" + dataImageName + ", ivec3(pixelCoords, " + std::to_string((*search)->dataTexture()->textureArrayLayerIndex()) + ")).x;\n";
            mDeclarationCode += "    float " + (*search)->name() + "Mask = imageLoad(" + maskImageName + ", ivec3(pixelCoords, " + std::to_string((*search)->maskTexture()->textureArrayLayerIndex()) + ")).x;\n";
            mIdentifiers.push_back(ctx->getText());
        }

        setNodeInfo(ctx, {ctx->getText() + "Data", ctx->getText() + "Mask > 0", (*search)->type(), (*search)->resolution()});
    }
    else
    {
        auto token = ctx->NAME()->getSymbol();
        mOffendingSymbols[static_cast<int>(token->getStartIndex())] = token;
        mSemanticErrorMessages += "line " + std::to_string(token->getLine()) + " : position " + std::to_string(token->getCharPositionInLine()) + ": " + ctx->getText() + " is not a valid layer\n";
    }
}

void Translator::exitIntConst(OParser::IntConstContext* ctx)
{
    setNodeInfo(ctx, {ctx->getText(), "true", Layer::Type::Int32, {0, 0}});
}

void Translator::exitFloatConst(OParser::FloatConstContext* ctx)
{
    setNodeInfo(ctx, {ctx->getText(), "true", Layer::Type::Float32, {0, 0}});
}


// *** Private Methods *** //

std::string Translator::toGLSLType(Layer::Type type)
{
    return (type >= Layer::Type::Float16) ? "float" : "int";        
}

// std::string Translator::toGLSLImageFormat(int32_t format)
// {
//     switch(format)
//     {
//         case GL_RGBA32F         : return "rgba32f";
//         case GL_RGBA16F         : return "rgba16f";
//         case GL_RG32F           : return "rg32f";
//         case GL_RG16F           : return "rg16f";
//         case GL_R11F_G11F_B10F  : return "rg11f_g11f_b10f";        
//         case GL_R32F            : return "r32f";
//         case GL_R16F            : return "r16f";
//         case GL_RGBA32UI        : return "rgba32ui";
//         case GL_RGBA16UI        : return "rgba16ui";
//         case GL_RGB10_A2UI      : return "rgb10_a2ui";
//         case GL_RGBA8UI         : return "rgba8ui";
//         case GL_RG32UI          : return "rg32ui";
//         case GL_RG16UI          : return "rg16ui";
//         case GL_RG8UI           : return "rg8ui";
//         case GL_R32UI           : return "r32ui";
//         case GL_R16UI           : return "r16ui";
//         case GL_R8UI            : return "r8ui";
//         case GL_RGBA32I         : return "rgba32i";
//         case GL_RGBA16I         : return "rgba16i";
//         case GL_RGBA8I          : return "rgba8i";
//         case GL_RG32I           : return "rg32i";
//         case GL_RG16I           : return "rg16i";
//         case GL_RG8I            : return "rg8i";
//         case GL_R32I            : return "r32i";
//         case GL_R16I            : return "r16i";
//         case GL_R8I             : return "r8i";
//         case GL_RGBA16          : return "rgba16";
//         case GL_RGB10_A2        : return "rgb10_a2";
//         case GL_RGBA8           : return "rgba8";
//         case GL_RG16            : return "rg16";
//         case GL_RG8             : return "rg8";
//         case GL_R16             : return "r16";
//         case GL_R8              : return "r8";
//         case GL_RGBA16_SNORM    : return "rgba16_snorm";
//         case GL_RGBA8_SNORM     : return "rgba8_snorm";
//         case GL_RG16_SNORM      : return "rg16_snorm";
//         case GL_RG8_SNORM       : return "rg8_snorm";
//         case GL_R16_SNORM       : return "r16_snorm";
//         case GL_R8_SNORM        : return "r8_snorm";
//     }
// }
// 
// std::string Translator::prefixImageFormat(int32_t format)
// {
//     switch(format)
//     {
//         case GL_RGBA32F         : 
//         case GL_RGBA16F         : 
//         case GL_RG32F           : 
//         case GL_RG16F           : 
//         case GL_R11F_G11F_B10F  : 
//         case GL_R32F            : 
//         case GL_R16F            : 
//         case GL_RGBA16          : 
//         case GL_RGB10_A2        : 
//         case GL_RGBA8           : 
//         case GL_RG16            : 
//         case GL_RG8             : 
//         case GL_R16             : 
//         case GL_R8              : 
//         case GL_RGBA16_SNORM    : 
//         case GL_RGBA8_SNORM     : 
//         case GL_RG16_SNORM      : 
//         case GL_RG8_SNORM       : 
//         case GL_R16_SNORM       : 
//         case GL_R8_SNORM        : return "";
//         
//         case GL_RGBA32UI        : 
//         case GL_RGBA16UI        : 
//         case GL_RGB10_A2UI      : 
//         case GL_RGBA8UI         : 
//         case GL_RG32UI          : 
//         case GL_RG16UI          : 
//         case GL_RG8UI           : 
//         case GL_R32UI           : 
//         case GL_R16UI           : 
//         case GL_R8UI            : return "u";
//         
//         case GL_RGBA32I         : 
//         case GL_RGBA16I         : 
//         case GL_RGBA8I          : 
//         case GL_RG32I           : 
//         case GL_RG16I           : 
//         case GL_RG8I            : 
//         case GL_R32I            : 
//         case GL_R16I            : 
//         case GL_R8I             : return "i";        
//     }    
// }


Layer::Type Translator::implicitTypeConversion(Layer::Type operandA, Layer::Type operandB)
{
    if(operandA == operandB)
        return operandA;
    else if(operandA < Layer::Type::Float16 && operandB >= Layer::Type::Float16)
        return operandB;
    else if(operandB < Layer::Type::Float16 && operandA >= Layer::Type::Float16)
        return operandA;
    else 
        return std::max(operandA, operandB);
}

void Translator::updateImageDeclarations(const std::string& dataImageName, const std::string& maskImageName, const Layer* layer)
{
    auto parsedImageSearch = std::find_if(begin(mImages), end(mImages), [&](const std::string& image) { return (dataImageName == image) ? true : false; });

    if(parsedImageSearch == end(mImages))
    {
        auto a = "layout(" + toGLSLImageFormat(layer->dataTexture()->internalFormat());
        auto b = std::to_string( layer->dataTexture()->textureArray()->imageUnit()) + ") uniform ";
        auto c = prefixImageFormat(layer->dataTexture()->internalFormat()) + "image2DArray " + dataImageName + ";\n";
        mImageDeclarations += "layout(" + toGLSLImageFormat(layer->dataTexture()->internalFormat()) + ", binding = " + std::to_string( layer->dataTexture()->textureArray()->imageUnit()) + ") uniform " + prefixImageFormat(layer->dataTexture()->internalFormat()) + "image2DArray " + dataImageName + ";\n";
        mImages.push_back(dataImageName);
    }

    parsedImageSearch = std::find_if(begin(mImages), end(mImages), [&](const std::string& image) { return (maskImageName == image) ? true : false; });

    if(parsedImageSearch == end(mImages))
    {
        mImageDeclarations += "layout(" + toGLSLImageFormat(layer->maskTexture()->internalFormat()) + ", binding = " + std::to_string( layer->maskTexture()->textureArray()->imageUnit()) + ") uniform " + prefixImageFormat(layer->maskTexture()->internalFormat()) + "image2DArray " + maskImageName + ";\n";
        mImages.push_back(maskImageName);
    }    
}


