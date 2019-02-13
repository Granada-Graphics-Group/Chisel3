#include "glrenderer.h"
#include "logger.hpp"

#include "glutils.h"
#include "fontmanager.h"
#include "fbxloader.h"
#include "model3d.h"
#include "uniformbufferobject.h"
#include "rendertechnique.h"
#include "computejob.h"
#include "textureview.h"

#include "chisel.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <numeric>
#include <iostream>
#include <string>
#include <unordered_set>
#include <complex>
#include <array>

#include <QApplication>
#include <QDir>
#include <QImage>

#ifndef APIENTRY
 #ifdef _WIN32
  #define APIENTRY __stdcall
 #else
  #define APIENTRY
 #endif
#endif /* APIENTRY */

#ifndef OPENGL_CALLBACK
#define OPENGL_CALLBACK

void APIENTRY openglCallbackFunction(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam)
{
    LOG_ERR(" OpenGL Debug: ", message);
    
    std::string errorType;
    
    switch (type) 
    {
    case GL_DEBUG_TYPE_ERROR:
        errorType = "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        errorType = "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        errorType = "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        errorType = "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        errorType = "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        errorType = "OTHER";
        break;
    }
    LOG_ERR(" - Type: ", errorType);

    std::string severityDegree;
    
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        severityDegree = "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severityDegree = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        severityDegree = "HIGH";
        break;
    }
    LOG_ERR(" - Id: ", id, " severity: ", severityDegree);
}
#endif //OPENGL_CALLBACK

GLRenderer::GLRenderer(unsigned int windowWidth, unsigned int windowHeight, ResourceManager* manager)
:
    mWindowWidth(windowWidth),
    mWindowHeight(windowHeight),
    mManager(manager)
{
    LOG("Setting up OpenGL Renderer:");
    
    if (glewInit() != GLEW_OK)
    {
        LOG_ERR("Failed to initialize GLEW");
        exit(-1);
    }
    else
        LOG("Using GLEW ", glewGetString(GLEW_VERSION));
    
    if (GLEW_VERSION_4_5)
        LOG("OpenGL 4.5 supported");
    else
    {
        LOG_ERR("OpenGL 4.5 not supported");
        exit(-1);
    }
    
    if(GLEW_KHR_debug)
    {
        LOG("OpenGL: KHR_debug supported");
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
            GL_DONT_CARE,
            GL_DONT_CARE,
            0,
            &unusedIds,
            true);
        LOG("Debug callback engaged");
    }
    
    if(GLEW_ARB_sparse_texture)
        LOG("OpenGL: Sparse Textures supported");
    else
    {
        LOG_ERR("OpenGL: Sparse Textures not supported");
        exit(-1);
    }
        
    mManager->initGLResourceData();
    mBlendingStates.resize(mManager->glConstants().maxColorAttachments, false);
    
    glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
    glClearDepth(0.0);    
    
    mManager->createTextureArray(5, {}, GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, 8, false);
    auto FBOColorTexture = mManager->createTexture("DefaultFBOColor", GL_TEXTURE_2D, GL_RGB8, mWindowWidth, mWindowHeight, GL_RGB, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, true);
    auto FBODepthTexture = mManager->createTexture("DefaultFBODepth", GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, false);    
    mFBO = std::make_unique<FBObject>(GL_FRAMEBUFFER, std::vector<Texture*>{FBOColorTexture}, FBODepthTexture);    
}

GLRenderer::~GLRenderer()
{
    LOG("Deleting VAO: ", mVAO);
    glDeleteVertexArrays(1, &mVAO);
}

// *** Public Methods *** //

void GLRenderer::init(GLuint defaultFB)
{
    mDefaultFBIndex = defaultFB;
       
    QDir appDirPath = QDir::current();
    appDirPath.cdUp();
    if (!appDirPath.cd("shaders"))
        appDirPath = QDir::current();
    else
        appDirPath.cdUp();
    QString path(appDirPath.absolutePath() + "/shaders");    
    mManager->loadEffects(path.toStdString(), {"*.xml"});
        
    glClearColor(0.0, 0.0, 0.0, 0.0);                    
           
    mUniformBuffers.resize(GLUniBuffer::Count);
    
    mUniformBuffers[GLUniBuffer::Compute] = std::make_unique<UniformBufferObject>(40 * sizeof(GLint), nullptr);
    mUniformBuffers[GLUniBuffer::Compute]->bindToIndexedBufferTarget(9, 0, mUniformBuffers[GLUniBuffer::Compute]->capacity());
    
    mUniformBuffers[GLUniBuffer::Transient] = std::make_unique<UniformBufferObject>(10 * sizeof(glm::uvec2), nullptr);
    mUniformBuffers[GLUniBuffer::Transient]->bindToIndexedBufferTarget(11, 0, mUniformBuffers[GLUniBuffer::Transient]->capacity());
    
    mUniformBuffers[GLUniBuffer::Material] = std::make_unique<UniformBufferObject>(62 * sizeof(glm::vec4), nullptr);
    mUniformBuffers[GLUniBuffer::Material]->bindToIndexedBufferTarget(12, 0, mUniformBuffers[GLUniBuffer::Material]->capacity());
    LOG("PerMaterialUBO: ", mUniformBuffers[GLUniBuffer::Material]->capacity());

    std::array<GLint, 4> viewport = { 0, 0, mWindowWidth, mWindowHeight }; //VS15

    // Camera
    
    mOrthoCamera = std::make_unique<Camera>("orthoCamera", glm::orthoZO(0.0f, static_cast<float>(mWindowWidth), 0.0f, static_cast<float>(mWindowHeight), 1000.0f, -1.0f));
    auto proyectorCamera = new Camera("proyectorCamera", glm::orthoZO(0.0f, 1.0f, 0.0f, 1.0f, 1000.0f, -5.0f));
    mNormOrthoCamera = std::make_unique<Camera>("paintTexCamera", glm::orthoZO(0.0f, 1.0f, 0.0f, 1.0f, 1000.0f, -5.0f));

    mUniformBuffers[GLUniBuffer::Frame] = std::make_unique<UniformBufferObject>(4 * sizeof(glm::mat4) + 10 * sizeof(glm::vec4), nullptr);    
    mUniformBuffers[GLUniBuffer::Frame]->bindToIndexedBufferTarget(0, 0, mUniformBuffers[GLUniBuffer::Frame]->capacity());    
    mUniformBuffers[GLUniBuffer::Frame]->update(2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proyectorCamera->projectionMatrix()));
    mUniformBuffers[GLUniBuffer::Frame]->update(3 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mNormOrthoCamera->projectionMatrix()));
    
                  
    auto dummyMaterial = mManager->createMaterial("Dummy");    
    auto brushDisk = mManager->createDisk("BrushDisk", 38, 40, 60);
    auto brushCircle = mManager->createCircle("BrushCircle", { 0.5, 0.5, 0.0 }, 0.5, 60);
    
    auto brushModel = new Model3D("BrushModel", brushDisk, {dummyMaterial});
    auto brushTexModel = new Model3D("BrushTexModel", brushCircle, {dummyMaterial});
    brushModel->setModelMatrix(glm::translate(brushModel->modelMatrix(), glm::vec3(mWindowWidth - brushModel->mesh()->width()/2, mWindowHeight - brushModel->mesh()->height()/2, 0.0)));    
    auto brushScene = new Scene3D("BrushScene", {mOrthoCamera.get()}, {brushModel});
    auto brushTexScene = new Scene3D("BrushTexScene", {mNormOrthoCamera.get()}, {brushTexModel});
    mPPScene = brushScene;
    
    auto brushMat = mManager->createMaterial("BrushMat", "PaintTool");
    auto brushPass = mManager->createRenderPass("BrushPass", mPPScene, brushMat);
    brushPass->setAutoClear(false);
    mBrushToolTarget = mManager->createRenderTarget("BrushToolTarget", mManager, viewport, {brushPass}, true, mFBO->id());

    auto brushTexPass = mManager->createRenderPass("BrushTexPass", brushTexScene, brushMat);
    auto brushShapeTextureTarget = mManager->createRenderTarget("BrushTexTarget", mManager, {0, 0, 1024, 1024}, {brushTexPass}, false);
        
    mCurrentPaletteTexture = mManager->createTexture("CurrentPaletteTexture", GL_TEXTURE_1D, GL_RGBA32F, 4096, 1, GL_RGBA, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, false); 
    mCopyValueTexture = mManager->createTexture("CopyValueTexture", GL_TEXTURE_2D, GL_R32F, 4096, 4096, GL_RED, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, false);
    mBrushMaskTexture = mManager->createTexture("BrushMaskTexture", GL_TEXTURE_2D, GL_R8, 2048, 2048, GL_RED, GL_UNSIGNED_BYTE, {}, GL_NEAREST, GL_NEAREST, 0, false);
    
    mFontManager = new FontManager(mManager);
    auto fontTexture = mFontManager->loadFont(appDirPath.absolutePath().toStdString() + "/fonts/", "arial.ttf", 24);
    auto fontMeshMat = mManager->createMaterial("FontTex");
    fontMeshMat->setDiffuseTexture(fontTexture);
    auto fontMesh = mFontManager->generateMeshText("arial.ttf","Alpha Version 0.6.3", 20, 30);
    auto fontModel = new Model3D("FontModel", fontMesh, { fontMeshMat });
    mFontScene = new Scene3D("FontScene", {mOrthoCamera.get()}, {fontModel});
    mFontScene->setRenderer(this);
    auto fontMat = mManager->createMaterial("FontMat", "Font");
    auto fontPass = mManager->createRenderPass("FontPass", mFontScene, fontMat);
    fontPass->setAutoClear(false);
    fontPass->enableBlending(0);
    auto fontTarget = mManager->createRenderTarget("FontTarget", mManager, viewport, {fontPass}, true, mFBO->id());
                  
    mBrushShapeTexture = brushShapeTextureTarget->colorTexOutputs()[0];//  mBrushTex = brushTexTarget->colorOutputs()[0].get();
        
    mUITech = mManager->createRenderTechnique("UITech", {fontTarget});
    mBrushShapeTech = mManager->createRenderTechnique("brushShapeTech", {brushShapeTextureTarget});
    
    insertTechnique(mBrushShapeTech, 1);
    insertTechnique(mUITech);

    initGLBuffers();
    updateGLBuffers();
    
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glEnable(GL_DEPTH_TEST);
    
    glEnable (GL_BLEND); 
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for(size_t i = 0; i < mBlendingStates.size(); ++i)
        mBlendingStates[i] = true;
}

float GLRenderer::markToolRadius()
{
    auto markToolModel = mPPScene->findModel3D("BrushModel");
    auto markToolMesh = static_cast<Disk *>(markToolModel->mesh());

    return markToolMesh->outerRadius();
}

void GLRenderer::clear(bool color, bool depth, bool stencil)
{
    GLbitfield bits = 0;
    if(color) bits |= GL_COLOR_BUFFER_BIT;
    if(depth) bits |= GL_DEPTH_BUFFER_BIT;
    if(stencil) bits |= GL_STENCIL_BUFFER_BIT;
    
    glClear(bits);
}

void GLRenderer::clearColorBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::clearStencilBuffer()
{
    glClear(GL_STENCIL_BUFFER_BIT);
}

void GLRenderer::setClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    mClearColor = glm::vec4(red, green, blue, alpha);
    glClearColor(red, green, blue, alpha);
}

void GLRenderer::setClearColor(glm::vec4 color)
{
    mClearColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

void GLRenderer::setAutoClear(bool clear)
{
    mAutoClear = clear;
}


void GLRenderer::setAutoClearColor(bool clear)
{
    mAutoClearColor = clear;
}

void GLRenderer::setAutoClearDepth(bool clear)
{
    mAutoClearDepth = clear;
}

void GLRenderer::setAutoClearStencil(bool clear)
{
    mAutoClearStencil = clear;
}

void GLRenderer::setRenderMode()
{
    mMode = Mode::Render;
    
    mUITech->removeTarget(mBrushToolTarget);
    mProjTech->removeTarget(mSlicePlaneTarget);
    useSliceVersionShaders(false);
}

void GLRenderer::setMarkMode(bool mark)
{        
    if(mark)
    {
        mMode = Mode::Mark;
        mPPScene->setModelDirty("BrushModel");
        
        //mUITech->addTarget(mBrushToolTarget);        
        mProjTech->removeTarget(mSlicePlaneTarget);        
    }
    else
    {
        mMode = Mode::Render;
        mUITech->removeTarget(mBrushToolTarget);
    }
}

void GLRenderer::setEraseMode(bool erase)
{
    if(erase)
    {
        mMode = Mode::Erase;
        
        //mUITech->addTarget(mBrushToolTarget);        
        mProjTech->removeTarget(mSlicePlaneTarget);
    }
    else
    {
        mMode = Mode::Render;
        mUITech->removeTarget(mBrushToolTarget);
    }
}

void GLRenderer::setReadMode(bool read)
{
    if(read)
    {
        mMode = Mode::Read;
        
        mUITech->removeTarget(mBrushToolTarget);        
        mProjTech->removeTarget(mSlicePlaneTarget);        
    }
    else
        mMode = Mode::Render;
}


void GLRenderer::setPickMode(bool pick)
{
    if(pick)
    {
        mMode = Mode::Pick;
        
        mUITech->removeTarget(mBrushToolTarget);
        mProjTech->removeTarget(mSlicePlaneTarget);        
    }
    else
        mMode = Mode::Render;
}

void GLRenderer::setSliceMode(bool slice)
{
    if(slice)
    {
        mMode = Mode::Slice;

        auto model = mMainScene->models()[0];

        if (!mSliced)
        {
            mSlicePlaneScene->setOrientation(glm::inverse(mCamera->orientation()));
            mSlicePlaneScene->setPosition(glm::vec3(model->modelMatrix() * glm::vec4(model->mesh()->boundingBox().center, 1.0)));
            updateSlicePlaneUniformData();
        }
                
        if(model->mesh()->indexBuffer2Used())
        {
            std::vector<uint32_t> subMeshes = {static_cast<unsigned int>(model->mesh()->buffer(GLBuffer::Index).size() / sizeof(uint32_t))};
            model->mesh()->updateSubMeshData(subMeshes, 0);
            model->mesh()->useIndexBuffer2(false);
            mMainScene->setMeshDirty(model->mesh());
        }

        mProjTech->addTarget(mSlicePlaneTarget);
        useSliceVersionShaders(true);
    }
    else
    {
        mMode = Mode::Render;
        mProjTech->removeTarget(mSlicePlaneTarget);
        //useSliceVersionShaders(false);
    }
}

void GLRenderer::alignMainCameraToModel()
{
    alignCameraToModel(mMainScene->camera(), mMainScene->models()[0]);
    
    mMainScene->setProjCameraNeedUpdate(true);
    mMainScene->setViewCameraNeedUpdate(true);

    updateSlicePlaneUniformData();
}

void GLRenderer::alignCameraToModel(Camera *camera, Model3D * model)
{
    auto matrix = model->modelMatrix();
    auto bbox = model->mesh()->boundingBox();
    auto bboxMax = matrix * glm::vec4(bbox.max, 1.0);
    auto bboxMin = matrix * glm::vec4(bbox.min, 1.0);
    auto bboxCenter = matrix * glm::vec4(bbox.center, 1.0);
    auto widthZ = glm::length(bboxMax - bboxMin);

    camera->setFarPlane(widthZ * 20);
    camera->setNearPlane(0.0999);

    auto glmPerspective = MakeInfReversedZProjRH(glm::radians(camera->fieldOfViewY()), camera->aspectRatio(), camera->nearPlane());
    camera->setProjectionMatrix(glmPerspective);

    LOG("Matrix: ", glm::to_string(matrix));

    //auto distance = bbox.height() / (2.0 / tan(glm::radians(camera->fieldOfViewY()) / 2.0));
    auto distance = glm::length(bboxMax - bboxCenter) / glm::tan(glm::radians(camera->fieldOfViewY()) / 2.0);
    auto position = glm::vec3(bboxCenter) - glm::abs(distance) * glm::vec3(0.0f, 0.0f, -1.0f);

    auto up = glm::vec3(0, 1, 0);
    auto view = glm::lookAt(position, glm::vec3(bboxCenter), up);
    LOG("GLM View: ", glm::to_string(view));

    camera->setOrientation(glm::quat_cast(view));
    camera->setTarget(glm::vec3(matrix * glm::vec4(bbox.center, 1.0)));
    camera->setPosition(glm::vec3(-view[3][0], -view[3][1], -view[3][2]));
    camera->setUp(up);

    LOG("After: ", glm::to_string(camera->viewMatrix()));
}

void GLRenderer::insertMarkTool()
{
    mUITech->addTarget(mBrushToolTarget);    
}

void GLRenderer::removeMarkTool()
{
    mUITech->removeTarget(mBrushToolTarget);
}


void GLRenderer::updateMarkToolPosition(int x, int y)
{
    auto markToolModel = mPPScene->findModel3D("BrushModel");
    markToolModel->translate(x - markToolModel->mesh()->width()/2.0, mWindowHeight - y - markToolModel->mesh()->height()/2.0, 0.0);
}

void GLRenderer::updateMarkToolRadius(float radius)
{
    auto markToolModel = mPPScene->findModel3D("BrushModel");
    auto markToolMesh = static_cast<Disk *>(markToolModel->mesh());
    auto delta = radius - markToolMesh->outerRadius();
    markToolModel->translate(-delta, -delta, 0.0);
    markToolMesh->changeRadius(radius - markToolMesh->radiusWidth(), radius);
}

void GLRenderer::toggleVertexColorVisibility(bool checked)
{
    mVertexColorVisibility = checked;
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::setDefaultModelColor(glm::vec4 color)
{
    mDefaultModelColor = color;
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::setSpecularColor(glm::vec3 color)
{
    mSpecularColor = color;
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::setSpecularPower(float power)
{
    mSpecularPower = power;
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::sliceModelWithPlane()
{
    planeIntersectObject(mMainScene->models()[0], mMainScene->models()[0]);
}

using clock_type = typename std::conditional< std::chrono::high_resolution_clock::is_steady,
                                              std::chrono::high_resolution_clock,
                                              std::chrono::steady_clock >::type ;

void GLRenderer::render()
{    
        update();
        
        processComputeJobs();
        
        for(auto tech : mRenderQueue)
        {
            tech->decreaseLife();
            
            LOG("GLRenderer::render:: Technique: ", tech->name(), ", life: ", tech->life());
            updateGenericUniformData();
                        
            for(auto target : tech->targets())
            {
                //LOG("Target: ", target->name());                
                updateTransientUniformData(target);

                if (target == mDilationTarget || target == mEraseDilationTarget)
                {
                    auto srcData = mPaintTexTarget->colorTexOutputs()[0];
                    auto dstData = mCopyValueTexture;
//                     glCopyImageSubData(srcData->textureArrayId(), srcData->textureArray()->target(), 0, 0, 0, srcData->textureArrayIndex(), dstData->textureArrayId(), dstData->textureArray()->target(), 0, 0, 0, dstData->textureArrayIndex(), srcData->width(), srcData->height(), 1);

                    int Offset = 1;
                    
                    mUniformBuffers[GLUniBuffer::Transient]->updateCache(0, sizeof(glm::uvec2), glm::value_ptr(dstData->textureArrayIndices()));
                    mUniformBuffers[GLUniBuffer::Transient]->updateCache(sizeof(glm::uvec2), sizeof(int), &Offset);
                    mUniformBuffers[GLUniBuffer::Transient]->updateGPU();
                }
                
                auto fbo = target->fbo();
                const auto& viewport = target->viewport();                
                                              
                fbo->bindFBO();
                glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
                
                for(auto pass : target->passes())
                {
                    //LOG("Pass: ", pass->name());
                                        
                    bool depthTest = pass->isDepthTestEnabled(); 
                    
                    if(depthTest != mDepthState)
                    {
                        (depthTest) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
                        
                        mDepthState = depthTest;
                    }
                    
                    if(mDepthState) glDepthFunc(pass->depthFunc());
                                        
                    
                    const auto& blendingStates = pass->blendingStates();
                    
                    for(int index = 0; index < blendingStates.size(); ++index)
                    {
                        if(mBlendingStates[index] != blendingStates[index].mEnabled)
                        {
                            (blendingStates[index].mEnabled) ? glEnablei(GL_BLEND, index) : glDisablei(GL_BLEND, index); 
                            
                            mBlendingStates[index] = blendingStates[index].mEnabled;
                        }
                        
                        if(mBlendingStates[index])
                        {
                            glBlendEquationSeparatei(index, blendingStates[index].mBlendEquation[0], blendingStates[index].mBlendEquation[0]);
                            glBlendFuncSeparatei(index, blendingStates[index].mBlendFunc[0], blendingStates[index].mBlendFunc[1], blendingStates[index].mBlendFunc[2], blendingStates[index].mBlendFunc[3]);
                        }
                    }
                   
                    Scene3D* scene = pass->scene();

                    if(mScene != scene)
                    {
                        mScene = scene;
                        updateSceneUniformData(scene, true);
                    }
                    else
                    {
                        updateSceneUniformData(scene);
                    }

                    if(scene->models().size())
                    {
                        LOG("GLRenderer::render::scene size: ", scene->models().size());
                        
                        glBindVertexArray(mVAO);

                        if(pass->autoClear())
                            clear(pass->autoClearColor(), pass->autoClearDepth(), pass->autoClearStencil());
                        
                       // LOG("clear Color: ", pass->autoClearColor(), ", depth: ", pass->autoClearDepth(), ", stencil: ", pass->autoClearStencil());
                        
                        if(pass->usingSceneMaterial())
                        {                            
                            //LOG("- Using Scene Material ", pass->sceneMaterial()->name());
                                                        
                            pass->sceneMaterial()->shader()->use();
                            
                            auto shader = pass->sceneMaterial()->shader();
                            
                            if(shader->uniform("Offset") >= 0)
                                glUniform1i(shader->uniform("Offset"), 1); 

                            
                            const auto& drawCommands = scene->drawCommands();
                            
                            glMemoryBarrier(GL_ALL_BARRIER_BITS);
                            //glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &mDrawIndirectCommands[0], mDrawIndirectCommands.size(), 0 );
                            glMultiDrawElementsIndirect(static_cast<GLenum>(scene->primitive()), GL_UNSIGNED_INT, &drawCommands[0], drawCommands.size(), 0 );                            
                            
                            glMemoryBarrier(GL_ALL_BARRIER_BITS);
//                             glMemoryBarrier(GL_ALL_BARRIER_BITS);
//                             if(saveDisk)
//                             {
//                                 saveDisk = false;
//                                 glBindTexture(GL_TEXTURE_2D_ARRAY, paint->originalTexture()->textureArrayId());
//                                 glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
//                             }

                            //if (saveDisk)
                            //{
                            //    saveDisk = false;
                            //    glBindTexture(GL_TEXTURE_2D, paint->id());
                            //    glGetTexImage(paint->target(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mPixelData.get());
                            //    QImage buffer(mPixelData.get(), 2048, 2048, QImage::Format_RGBA8888);
                            //    buffer = buffer.mirrored();
                            //    std::string name("layerTex.png");
                            //    buffer.save(name.c_str());
                            //}
//                            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &mDrawIndirectCommands[0] + 1, mDrawIndirectCommands.size() - 1, 0 );
//                             mPrograms["BillBoarding"]->use();          
//                             glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &mDrawIndirectCommands[0], 1, 0 );
                            
                            //glReadBuffer(GL_COLOR_ATTACHMENT0);

                            //double xPos, yPos;
                            //glfwGetCursorPos(mWindow, &xPos, &yPos);
                            
                           /* xPos = 360; yPos = 640;
                            auto data = reinterpret_cast<float *>(mPBO->dataCache().data());
                            
                            //float x = data[ xPos * 1280 * 2 + yPos * 2];
                            //float y = data[ xPos * 1280 * 2 + yPos * 2 + 1];
                            
                            for(int i = xPos - 20; i < (xPos + 20); ++i)
                                for(int j = yPos - 20; j < (yPos + 20); ++j)
                                {
                                   float x = data[ i * 1280 * 2 + j * 2 ];
                                   float y = data[ i * 1280 * 2 + j * 2 + 1 ];
                                   
                                   int coordX = static_cast<int>(roundf(x * 512));
                                   int coordY = static_cast<int>(roundf(y * 512));
                                   imagen[coordX * 512 * 3 + coordY * 3] = 0;
                                   imagen[coordX * 512 * 3 + coordY * 3 + 1] = 0;
                                   imagen[coordX * 512 * 3 + coordY * 3 + 2] = 255;
                                }
*/                          
//                            if(saveDisk && pass->name().compare("ExpandPass") == 0)
//                             {
//                                 saveDisk = false;
//                                 glReadPixels(0, 0, viewport[2], viewport[3], GL_RGBA, GL_UNSIGNED_BYTE, mPixelData.get());
//                                 QImage buffer(mPixelData.get(), viewport[2], viewport[3], QImage::Format_RGBA8888);
//                                 buffer = buffer.mirrored();
//                                 std::string name("outputCoord.png");
//                                 buffer.save(name.c_str());
//                             }

                            
                            
                            /*for(int i = 0; i < fbo->drawBuffers().size(); i++)
                            {
                                glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
                                glReadPixels(0, 0, 1280, 720, GL_RGB, GL_UNSIGNED_BYTE, mPixelData.get());
                                QImage buffer(mPixelData.get(), 1280, 720, QImage::Format_RGB888);
                                buffer = buffer.mirrored();
                                std::string name("output" + std::to_string(i) + ".png");
                                buffer.save(name.c_str());
                            }
                            
                            if(!fbo->drawBuffers().size())
                            {
                                glReadPixels(0, 0, 1280, 720, GL_RGB, GL_UNSIGNED_BYTE, mPixelData.get());
                                QImage buffer(mPixelData.get(), 1280, 720, QImage::Format_RGB888);
                                buffer = buffer.mirrored();
                                std::string name("outputStd.png");
                                buffer.save(name.c_str());                                
                            }*/
                        }
                        else                        
                            for(auto material: scene->materials())
                            {
                                material->shader()->use();
                                const auto& matDrawCommands = scene->drawCommands(material->name());
//                             LOG("Drawing Material: ", material->name());
//                             
//                             std::vector<DrawElementsIndirectCommand> drawCommands = mDrawIndirectCommands2[material->name()];
//                             
//                             for(auto command: drawCommands)
//                             {                                
//                                 LOG("- Count: ", command.mCount);
//                                 LOG("- InstanceCount: ", command.mInstanceCount);
//                                 LOG("- FirstIndex: ", command.mFirstIndex);
//                                 LOG("- BaseVertex: ", command.mBaseVertex);
//                                 LOG("- BaseInstance: ", command.mBaseInstance);                                
//                             }                            
                                //glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &mDrawIndirectCommands2[material->name()][0], mDrawIndirectCommands2[material->name()].size(), 0 );
                                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &matDrawCommands[0], matDrawCommands.size(), 0 );
                            }
                            
                        glBindVertexArray(0);
                    }
                   
                }
                
                
                if(mAreaPerPixelTexture != nullptr && glIsTexture(mAreaPerPixelTexture->textureArray()->id()))
                {
                    glMemoryBarrier(GL_ALL_BARRIER_BITS);
                    glGenerateTextureMipmap(mAreaPerPixelTexture->textureArrayId());
                    //glGenerateTextureMipmap(mLockTexture->textureArrayId());
                }
                
//                 if(!target->isDefaultTargetUsed())
//                 {
//                     for(auto colorAttachment: fbo->colorAttachments())
//                         if(glIsTexture(colorAttachment->id()))
//                             glGenerateTextureMipmap(colorAttachment->id());
//                         else
//                             glGenerateTextureMipmap(colorAttachment->textureArrayId());
//                 }

                //if (target == mDilationTarget)
                //{
                //    glReadBuffer(GL_COLOR_ATTACHMENT0);
                //    glReadPixels(0, 0, 2048, 2048, GL_RGB, GL_UNSIGNED_BYTE, mPixelData.data());
                //    QImage buffer(mPixelData.data(), 2048, 2048, QImage::Format_RGB888);
                //    buffer = buffer.mirrored();
                //    std::string name("outputStd.png");
                //    buffer.save(name.c_str());
                //}

                if(mReadData && tech == mReadFBTech)
                {
                    auto layer = mLayers[mCurrentLayerIndex];
                    
                    if(layer.mSamplerType == 0)
                    {                    
                        glm::ivec2 value;                    
                        glReadPixels(mLastXPos, mWindowHeight - mLastYPos, 1, 1, GL_RG_INTEGER, GL_INT, glm::value_ptr(value));
                        if(mState == State::Reading)
                        {
                            (value.y) ? mChisel->showReadValue(value.x) : mChisel->hideReadValue();
                        }
                        else if(mState == State::Picking && value.y)
                        {
                            mChisel->setPickedValue(value.x);
                            setCurrentPaintingValue(value.x);
                            mState = State::Marking;
                        }                            
                    }
                    else if(layer.mSamplerType == 1)
                    {
                        glm::uvec2 value;                    
                        glReadPixels(mLastXPos, mWindowHeight - mLastYPos, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, glm::value_ptr(value));
                        if(mState == State::Reading)
                        {
                            (value.y) ? mChisel->showReadValue(value.x) : mChisel->hideReadValue();
                        }
                        else if(mState == State::Picking && value.y)
                        {
                            mChisel->setPickedValue(value.x);
                            setCurrentPaintingValue(value.x);
                            mState = State::Marking;
                        }
                    }
                    else
                    {
                        glm::vec2 value;                    
                        glReadPixels(mLastXPos, mWindowHeight - mLastYPos, 1, 1, GL_RG, GL_FLOAT, glm::value_ptr(value));
                        if(mState == State::Reading)
                        {
                            (value.y) ? mChisel->showReadValue(value.x) : mChisel->hideReadValue();
                        }
                        else if(mState == State::Picking && value.y)
                        {
                            mChisel->setPickedValue(value.x);
                            setCurrentPaintingValue(value.x);
                            mState = State::Marking;
                        }
                    }
                        
                    mReadData = false;
                }
                
                //fbo->unbindFBO();                
            }

            if (tech == mSeamMaskTech)
            {
                mMainScene->updateCamera(mCamera);
                mMainScene->setProjCameraNeedUpdate(true);
                mMainScene->setViewCameraNeedUpdate(true);
                
                mSeamMaskTexture = mSeamMaskTarget->colorTexOutputs()[0];
                std::vector<glm::byte> textureData(mSeamMaskTexture->height() * mSeamMaskTexture->width() * sizeOfGLInternalFormat(mSeamMaskTexture->internalFormat())); 
                glGetTextureSubImage(mSeamMaskTexture->textureArrayId(), 0, 0, 0, mSeamMaskTexture->textureArrayLayerIndex(), mSeamMaskTexture->width(), mSeamMaskTexture->height(), 1, mSeamMaskTexture->format(), mSeamMaskTexture->type(), textureData.size(), textureData.data());
                
                QImage layerImage(textureData.data(), mSeamMaskTexture->width(), mSeamMaskTexture->height(), QImage::Format_RGBA8888);
                layerImage = layerImage.mirrored(false, true);
                layerImage.save("./seamMask.png");
            }
            
            if(tech == mNeighborsTech)
            {
                mManager->exportImage2("./neighbors.png", mNeighborhoodTexture->width(), mNeighborhoodTexture->height(), readFloatTexture(mNeighborhoodTexture));
            }
        }

        mRenderQueue.erase(std::remove_if(begin(mRenderQueue), end(mRenderQueue),[](RenderTechnique *element) -> bool {return (element->life() == 0);}), end(mRenderQueue));
        
        auto t_end = clock_type::now();
        if (std::chrono::duration<double, std::milli>(t_end - t_start).count() > 1000)
        {
            std::cout << "Time per frame: " << 1000.0 / static_cast<double>(frameCounter) << " milliseconds" << std::endl;
            frameCounter = 0;
            t_start = clock_type::now();
        }
        else
            frameCounter++;
        
    glBlitNamedFramebuffer( mFBO->id(), mDefaultFBIndex,
                            0, 0, mWindowWidth, mWindowHeight,
                            0, 0, mWindowWidth, mWindowHeight,
                            GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GLRenderer::resize(int width, int height)
{
    LOG("1A Renderer::Resize window: [", width, ", ", height, "]");
    mWindowWidth = width;
    mWindowHeight = height;
    auto newAspectRatio = static_cast<float>(width)/static_cast<float>(height);
    
    glViewport(0, 0, mWindowWidth, mWindowHeight);
    
    if(mFontScene != nullptr)
    {
        mBrushToolTarget->setViewport(0, 0, width, height);
        mUITech->targets()[0]->setViewport(0, 0, width, height);
        
        mOrthoCamera->setAspectHeight(height);
        mOrthoCamera->setAspectWidth(width);
        mOrthoCamera->setAspectRatio(newAspectRatio);

        auto newOrtho = glm::orthoZO(0.0f, static_cast<float>(mWindowWidth), 0.0f, static_cast<float>(mWindowHeight), 1000.0f, -1.0f);
        mOrthoCamera->setProjectionMatrix(newOrtho);

        mFontScene->setProjCameraNeedUpdate(true);
        mPPScene->setProjCameraNeedUpdate(true);
    }
    
    if(mMainScene != nullptr)
    {           
        mDepthTarget->setViewport(0, 0, width, height);
        mReadFBTarget->setViewport(0, 0, width, height);
        mProjTech->targets()[0]->setViewport(0, 0, width, height);
        mSlicePlaneTarget->setViewport(0, 0, width, height);
        
        mCamera->setAspectHeight(height);
        mCamera->setAspectWidth(width);
        mCamera->setAspectRatio(newAspectRatio);
        
        auto newPerspective = MakeInfReversedZProjRH(glm::radians(mCamera->fieldOfViewY()), mCamera->aspectRatio(), mCamera->nearPlane());
        mCamera->setProjectionMatrix(newPerspective);
        //mSlicePlaneScene->camera()->setProjectionMatrix(newPerspective);
        
        mMainScene->setProjCameraNeedUpdate(true);
        //mSlicePlaneScene->setProjCameraNeedUpdate(true);
        
        mResized = true;
    }
    
    mManager->deleteTexture(mFBO->colorAttachment(0));
    mManager->deleteTexture(mFBO->depthAttachment());

    mManager->createTextureArray(5, {}, GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, 8, false);
    auto FBOColorTexture = mManager->createTexture("DefaultFBOColor", GL_TEXTURE_2D, GL_RGBA8, mWindowWidth, mWindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, true);
    auto FBODepthTexture = mManager->createTexture("DefaultFBODepth", GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, false);    
    mFBO->setcolorAttachment(FBOColorTexture, 0);
    mFBO->setDepthAttachment(FBODepthTexture);    
}


void GLRenderer::onMouseButtons(int button, int action, int mods, double xPos, double yPos)
{
    if( button == 0 )
    {
        if(action == 1)
        {
            mLeftButtonPressed = true;
            
            if(mLayerCount > 0)
            {
                if(mMode == Mode::Mark)
                {
                    mState = State::Marking;
                    
                    float value = 0;
                    glClearTexSubImage(mBrushMaskTexture->textureArrayId(), 0, 0, 0, mBrushMaskTexture->textureArrayLayerIndex(), mBrushMaskTexture->width(), mBrushMaskTexture->height(), 1, mBrushMaskTexture->format(), mBrushMaskTexture->type(), &value);
                    insertTechnique(mPaintTexTech, 1);            
                }
                else if(mMode == Mode::Read)
                {
                    mState = State::Reading;
                    mReadData = true;
                    
                    insertTechnique(mReadFBTech, 1);
                }
                else if(mMode == Mode::Erase)
                {
                    mState = State::Erasing;

                    float value = 0;
                    glClearTexSubImage(mBrushMaskTexture->textureArrayId(), 0, 0, 0, mBrushMaskTexture->textureArrayLayerIndex(), mBrushMaskTexture->width(), mBrushMaskTexture->height(), 1, mBrushMaskTexture->format(), mBrushMaskTexture->type(), &value);
                    insertTechnique(mEraseTexTech, 1);
                }
                else if(mMode == Mode::Pick)
                {
                    mState = State::Picking;
                    mReadData = true;
                    
                    insertTechnique(mReadFBTech, 1);
                }
            }
        }
        else
        {
            mLeftButtonPressed = false;
            mState = State::Rendering;
            if(mLayerCount > 0)
            {
               if(!mChisel->isLayerDirty(mCurrentLayerIndex)) mChisel->setLayerDirty(mCurrentLayerIndex);
               if(!mChisel->isLayerAreaFieldDirty(mCurrentLayerIndex)) mChisel->setLayerAreaFieldDirty(mCurrentLayerIndex);
            }
        }
    }
    else if ( button == 1 )
    {
        if(action == 1)
            mRightButtonPressed = true;
        else
        { 
            mRightButtonPressed = false;
            if(mMainScene != nullptr) insertTechnique(mDepthTech, 1);
        };         
    }
    else if (button == 2)
    {
        if(action == 1)
            mMiddleButtonPressed = true;
        else
        {
            mMiddleButtonPressed = false;
            if(mMainScene != nullptr) insertTechnique(mDepthTech, 1);            
        }
    }
    
    mLastXPos = xPos;
    mLastYPos = yPos;
}

glm::vec3 GLRenderer::rayIntersectPlane(glm::vec2 position, float z, glm::mat4 projection, glm::mat4 view)
{
    auto pointNear = glm::unProject(glm::vec3(position.x, position.y, 0.0), view, projection, glm::ivec4(0, 0, mWindowWidth, mWindowHeight));        
    auto pointFar = glm::unProject(glm::vec3(position.x, position.y, 1.0), view, projection, glm::ivec4(0, 0, mWindowWidth, mWindowHeight));
        
    float t = (z - pointNear.z) / (pointFar.z - pointNear.z);
    float x = pointNear.x + t * (pointFar.x - pointNear.x);
    float y = pointNear.y + t * (pointFar.y - pointNear.y);

    return glm::vec3(x, y, z);
}

void GLRenderer::planeIntersectObject(Model3D* plane, Model3D* model)
{
    auto meshVertices = reinterpret_cast<const float*>(model->mesh()->vertexBuffer().data());
    auto meshIndices = reinterpret_cast<const uint32_t*>(model->mesh()->indexBuffer().data());
    auto indexCount = model->mesh()->elementCount();
    
//     auto planeNormal = glm::vec3(0.0, 0.0, 1.0);
//     auto planePoint = glm::vec3(0.0, 0.0, 0.0);
    auto modelMatrix = model->modelMatrix();
    auto viewMatrix = mCamera->viewMatrix();
    
    auto planeNormal = glm::vec3(mPlaneNormal);
    auto planePoint = glm::vec3(viewMatrix * mPlanePoint);
    
    std::vector<uint32_t> newMeshIndices;
    
    for(auto i = 0; i < indexCount; i+=3)
    {
        auto index1 = meshIndices[i];
        auto index2 = meshIndices[i + 1];
        auto index3 = meshIndices[i + 2];
        
        glm::vec3 vertex1(meshVertices[3 * index1], meshVertices[3 * index1 + 1], meshVertices[3 * index1 + 2]);
        glm::vec3 vertex2(meshVertices[3 * index2], meshVertices[3 * index2 + 1], meshVertices[3 * index2 + 2]);
        glm::vec3 vertex3(meshVertices[3 * index3], meshVertices[3 * index3 + 1], meshVertices[3 * index3 + 2]);
        
        vertex1 = glm::vec3(viewMatrix * modelMatrix * glm::vec4(vertex1, 1.0));
        vertex2 = glm::vec3(viewMatrix * modelMatrix * glm::vec4(vertex2, 1.0));
        vertex3 = glm::vec3(viewMatrix * modelMatrix * glm::vec4(vertex3, 1.0));
        
        float distance1 = glm::dot(planeNormal, (vertex1 - planePoint));
        float distance2 = glm::dot(planeNormal, (vertex2 - planePoint));
        float distance3 = glm::dot(planeNormal, (vertex3 - planePoint));
        
        if(distance1 < 0 || distance2 < 0 || distance3 < 0)
        {
            newMeshIndices.push_back(index1);
            newMeshIndices.push_back(index2);
            newMeshIndices.push_back(index3);
        }
    }

    
    model->mesh()->updateData(GLBuffer::Index2, 0, newMeshIndices.size() * sizeof(uint32_t), newMeshIndices.data());
    std::vector<uint32_t> subMeshes = {static_cast<uint32_t>(newMeshIndices.size())};
    model->mesh()->updateSubMeshData(subMeshes, 0);
    model->mesh()->useIndexBuffer2(true);
//     model->mesh()->updateIndexData(newMeshIndices, 0);
//     std::vector<uint32_t> subMeshes = {static_cast<uint32_t>(newMeshIndices.size())};
//     model->mesh()->updateSubMeshData(subMeshes, 0);
}

void GLRenderer::useSliceVersionShaders(bool use)
{
    mSliced = use;
    
    if(mSliced)
        mProjPass->sceneMaterial()->setShader(mManager->shaderProgram("GeneralSlice"));
    else
        mProjPass->sceneMaterial()->setShader(mManager->shaderProgram("ProjectiveTex"));
    
    if(mLayers.size() > 0)
        updateTechniqueDataWithLayer(mLayers[mCurrentLayerIndex]);    
}

void GLRenderer::clearSlice()
{               
    auto model = mMainScene->models()[0];

    if(model->mesh()->indexBuffer2Used())
    {
        std::vector<uint32_t> subMeshes = {static_cast<unsigned int>(model->mesh()->buffer(GLBuffer::Index).size() / sizeof(uint32_t))};
        model->mesh()->updateSubMeshData(subMeshes, 0);
        model->mesh()->useIndexBuffer2(false);
        mMainScene->setMeshDirty(model->mesh());
    }

    for(auto& model : mSlicePlaneScene->models())
        model->setModelMatrix(glm::mat4{1.0});
    
    updateSlicePlaneUniformData();

    mProjTech->removeTarget(mSlicePlaneTarget);
    useSliceVersionShaders(false);
}

void GLRenderer::onMousePosition(double xPos, double yPos)
{
    double deltaX, deltaY;
    
    deltaX = xPos - mLastXPos;
    deltaY = yPos - mLastYPos;
    
    if (mLeftButtonPressed == true)
    {
        if(mLayerCount > 0)
        {
            if(mMode == Mode::Mark)
            {
                mState = State::Marking;

                insertTechnique(mPaintTexTech, 1);
            }
            else if(mMode == Mode::Erase)
            {
                mState = State::Erasing;

                insertTechnique(mEraseTexTech, 1);
            }
        }
        
        if(mMode == Mode::Slice)
        {
            auto sliceModel = mSlicePlaneScene->models()[0];

//             auto newMatrix = glm::rotate(sliceModel->modelMatrix(), static_cast<float>(glm::radians(deltaX / 5.0)), glm::vec3{0.0, 1.0, 0.0});
//             newMatrix = glm::rotate(newMatrix, static_cast<float>(glm::radians(deltaY / 5.0)), glm::vec3{1.0, 0.0, 0.0});    
//             sliceModel->setModelMatrix(newMatrix);
            
//             sliceModel->rotate(glm::radians(deltaX / 5.0), glm::vec3{0.0, 1.0, 0.0});
//             sliceModel->rotate(glm::radians(deltaY / 5.0), glm::vec3{1.0, 0.0, 0.0});
            
            auto mat = glm::mat4_cast(mCamera->orientation());
            mat = glm::inverse(mat);
            auto up = mat * glm::vec4(0.0, 1.0, 0.0, 1.0);
            auto right = mat * glm::vec4(1.0, 0.0, 0.0, 1.0);

               mSlicePlaneScene->rotate(glm::radians(deltaX / 5.0), glm::vec3(up)); 
               mSlicePlaneScene->rotate(glm::radians(deltaY / 5.0), glm::vec3(right));
            
//             sliceModel->rotateRelativeY(static_cast<float>(glm::radians(deltaX / 5.0)));
//             sliceModel->rotateRelativeX(static_cast<float>(glm::radians(deltaY / 5.0)));
            
            updateSlicePlaneUniformData();
        }
    }

    if (mMode == Mode::Mark || mMode == Mode::Erase)
    {
        auto brushModel = mPPScene->findModel3D("BrushModel");
        brushModel->setPosition({xPos - brushModel->mesh()->width() / 2.0, mWindowHeight - yPos - brushModel->mesh()->height() / 2.0, 0.0});
        
        mPPScene->setModelDirty("BrushModel");
    }
    
    if(mRightButtonPressed && mCamera != nullptr)
    {
        mUpdateCamera = true;
        mScene->setViewCameraNeedUpdate(true);
    
        mCamera->rotate(glm::radians(deltaX / 5.0), glm::vec3{0.0, 1.0, 0.0});
        mCamera->rotate(glm::radians(deltaY / 5.0), glm::vec3{1.0, 0.0, 0.0});
//         auto sliceModel = mSlicePlaneScene->models()[0];
//         sliceModel->rotate(glm::radians(deltaX / 5.0), glm::vec3{0.0, 1.0, 0.0});
//         sliceModel->rotate(glm::radians(deltaY / 5.0), glm::vec3{1.0, 0.0, 0.0});
        //if (mMode == Mode::Slice)
        //{
        //    mSlicePlaneScene->rotate(glm::radians(deltaX / 5.0), glm::vec3{ 0.0, 1.0, 0.0 });
        //    mSlicePlaneScene->rotate(glm::radians(deltaY / 5.0), glm::vec3{ 1.0, 0.0, 0.0 });

        //            updateSlicePlaneUniformData();
        //}

        updateSlicePlaneUniformData();
    }
    
    if(mMiddleButtonPressed && mCamera != nullptr)
    {
        mUpdateCamera = true;
        mScene->setViewCameraNeedUpdate(true);
        auto camera = mMainScene->camera();
        auto model = mMainScene->models()[0];
        auto center = model->mesh()->boundingBox().center;
        auto bbox = model->mesh()->boundingBox();
        auto dimensions = glm::abs(model->modelMatrix() * glm::vec4(bbox.max, 1.0) - model->modelMatrix() * glm::vec4(bbox.min, 1.0));
        auto width = (dimensions.x + dimensions.y + dimensions.z) / 3.0;
        auto viewCenter = mMainScene->camera()->viewMatrix() * model->modelMatrix() * glm::vec4(center, 1.0);
        auto stepSize = glm::max(fabs(viewCenter.z)/40.0, width/40.0);        
        
//         auto nearP = 0.1f;
//         auto farP = 50.0f;
//         auto projection = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, nearP, farP);
//         auto mv = glm::lookAt(glm::vec3(0.0, 0.0, 10.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
//         
//         auto centerProj = projection * mv * glm::vec4(4.5, 3.0, 0.2, 1.0);
//         auto ndcCenterProj = centerProj/centerProj.w;
//         auto equis = ndcCenterProj.x * mWindowWidth / 2.0 + mWindowWidth / 2.0;
//         auto ygriega = ndcCenterProj.y * mWindowHeight / 2.0 + mWindowHeight / 2.0;
//         auto zeta = ndcCenterProj.z * (0 - 1.0) / 2.0 + (1.0 + 0.0)/ 2.0;
//         auto win = glm::vec3(equis, ygriega, zeta);
//         
//         
//         
//         auto pointNear = glm::unProject(glm::vec3(win.x, win.y, 0.0), mv, projection, glm::ivec4(0, 0, mWindowWidth, mWindowHeight));
//         
//         auto pointFar = glm::unProject(glm::vec3(win.x, win.y, 1.0), mv, projection, glm::ivec4(0, 0, mWindowWidth, mWindowHeight));
//         
//         float t = (0.2 - pointNear.z) / (pointFar.z - pointNear.z);
//         float x = pointNear.x + t * (pointFar.x - pointNear.x);
//         float y = pointNear.y + t * (pointFar.y - pointNear.y);
        
//         auto temp = camera->viewMatrix();
//         
//         auto pos = temp * model->modelMatrix();
//         
//         auto currentPos = rayIntersectPlane(glm::vec2(xPos, mWindowHeight - yPos), -temp[3][2], camera->projectionMatrix(), temp);
//         auto lastPos = rayIntersectPlane(glm::vec2(mLastXPos, mWindowHeight - mLastYPos), -temp[3][2], camera->projectionMatrix(), temp);
//         
//         auto amount = camera->viewMatrix() * glm::vec4(currentPos, 1.0) - camera->viewMatrix() * glm::vec4(lastPos, 1.0);
//         
//         auto worldUp = glm::vec3(0.0, 1.0, 0.0);
//         auto tempLeft = glm::normalize(glm::cross(camera->position(), worldUp));
//         auto tempUp = glm::normalize(glm::cross(tempLeft, camera->position()));
//         
//         auto convRot = glm::mat3(tempLeft, tempUp, camera->forwardDirection());
//         auto m4 = glm::mat4(convRot);
//         m4[3][3] = 1.0;
//         auto newAmount = m4 * glm::vec4(glm::vec3(amount), 1.0);
// 
//         LOG("Current: ", glm::to_string(currentPos), " Last: ", glm::to_string(lastPos));
//         LOG("CurrentT: ", glm::to_string(camera->viewMatrix() * glm::vec4(currentPos, 1.0)), " LastT: ", glm::to_string(camera->viewMatrix() * glm::vec4(lastPos, 1.0)), ",  amount: ", glm::to_string(amount));
        
/*        LOG("width/10.0: ", width / 15.0, " ViewCenterZ: ", viewCenter.z, ", StepSize: ", stepSize, ", deltaX: ", deltaX, ", deltaX/10.0: ", deltaX / 10.0); */       
        
        mCamera->translateRelative(deltaX/20.0 * stepSize, deltaY/20.0 * stepSize, 0.0);
        //mSlicePlaneScene->camera()->translateRelative(deltaX/20.0 * stepSize, deltaY/20.0 * stepSize, 0.0);
        
        updateSlicePlaneUniformData();
//         mCamera->translateRelative(amount.x, amount.y, 0.0);
        //mCamera->translate(amount.x, amount.y, 0.0);        
    }
    
    mLastXPos = xPos;
    mLastYPos = yPos;
}

void GLRenderer::onMouseWheel(double xOffset, double yOffset)
{
    if(mMainScene != nullptr)
    {
        int numDegrees = yOffset / 8;
        int numSteps = numDegrees / 15;

        mUpdateCamera = true;
        mScene->setViewCameraNeedUpdate(true);

        auto model = mMainScene->models()[0];
        auto center = model->mesh()->boundingBox().center;
        auto bbox = model->mesh()->boundingBox();
        auto dimensions = glm::abs(model->modelMatrix() * glm::vec4(bbox.max, 1.0) - model->modelMatrix() * glm::vec4(bbox.min, 1.0));
        auto width = (dimensions.x + dimensions.y + dimensions.z) / 3.0;
        auto viewCenter = mMainScene->camera()->viewMatrix() * model->modelMatrix() * glm::vec4(center, 1.0);
        auto stepSize = glm::max(fabs(viewCenter.z)/15.0, width/15.0);

        //         LOG("width/10.0: ", width / 15.0, " ViewCenterZ: ", viewCenter.z, ", StepSize: ", stepSize, ", yOffset: ", yOffset, ", offset/10.0: ", yOffset / 15.0);
        if(mMode != Mode::Slice)
        {
            mCamera->translate({ 0.0, 0.0, yOffset/10.0 * stepSize });
            //mSlicePlaneScene->camera()->translate({ 0.0, 0.0, yOffset/10.0 * stepSize });
            //mSlicePlaneScene->camera()->setPosition(mCamera->position());

            updateSlicePlaneUniformData();

            insertTechnique(mDepthTech, 1);
        }
        else
        {
            auto planeMatrix = glm::mat4_cast(mSlicePlaneScene->models()[2]->orientation());
            auto axis = planeMatrix * glm::vec4(0, 0, -1, 1.0);
            //auto axis = glm::vec3(mPlaneNormal.x, mPlaneNormal.y, mPlaneNormal.z);
            //auto viewAxis = mSlicePlaneScene->camera()->viewMatrix() * glm::vec4(axis, 1.0);
            //axis = glm::normalize(viewAxis);
//                 auto max = mMainScene->camera()->viewMatrix() * model->modelMatrix() * glm::vec4(bbox.max, 1.0);
//                 auto min =  mMainScene->camera()->viewMatrix() * model->modelMatrix() * glm::vec4(bbox.min, 1.0);
//                 auto newPoint = glm::translate(mSlicePlaneScene->camera()->viewMatrix(), glm::vec3(axis)) * mSlicePlaneScene->models()[0]->modelMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0);
//                 auto less = glm::lessThan(glm::vec3(newPoint), glm::vec3(max));
//                 auto greater = glm::greaterThan(glm::vec3(newPoint), glm::vec3(min));
//                 if(less.x && less.y && less.z && greater.x && greater.y && greater.z)
//                 {

            LOG("Plane Axis: ", glm::to_string(axis), " distance: ", yOffset/40.0 * stepSize);


            //mSlicePlaneScene->models()[0]->translate(glm::vec3(axis) * yOffset/40.0 * stepSize);
            mSlicePlaneScene->translate(glm::vec3(axis) * yOffset/40.0 * stepSize);
            updateSlicePlaneUniformData();
//                 }
        }

        LOG("On Mouse Wheel");
    }
}


void GLRenderer::updateTexArrayIndexBuffer()
{
    std::vector<GLint> texArrayIndexBuffer;
    
    std::map <std::string, std::vector<Model3D* > > instancesByMesh { mScene->modelsByGeometry() }; 
    std::vector <Mesh* > meshes = mScene->meshes();
    
    for(auto index = 0; index < meshes.size(); ++index)
    {   
        Mesh* mesh = meshes[index];
        
        for(auto instances: instancesByMesh[mesh->name()])
        {
            if(instances->material(0)->diffuseTexture() != nullptr)
            {
                unsigned int a = instances->material(0)->diffuseTexture()->textureArrayIndex();
                unsigned int b = instances->material(0)->diffuseTexture()->textureArrayLayerIndex();
                texArrayIndexBuffer.push_back(a);
                texArrayIndexBuffer.push_back(b);
            }
            else
            {
                texArrayIndexBuffer.push_back(0);
                texArrayIndexBuffer.push_back(0);                
            }
        }
    }
    
    if(mVBO[3] == 0)    
        glGenBuffers(1, &mVBO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[3]);            
    glBufferData(GL_ARRAY_BUFFER, texArrayIndexBuffer.size()*sizeof(GLint), &texArrayIndexBuffer.front(), GL_STATIC_DRAW);
    
    glVertexAttribIPointer((GLuint)3, 2, GL_INT, 0, 0);
    
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
}

void GLRenderer::updateInstanceMatrixBuffer()
{
    std::vector<glm::mat4> matBuffer;

    std::map <std::string, std::vector<Model3D* > > instancesByMesh { mScene->modelsByGeometry() }; 
    std::vector <Mesh* > meshes = mScene->meshes();    
    
    for(auto index = 0; index < meshes.size(); ++index)
    {   
        Mesh* mesh = meshes[index];
                
        for(auto instances: instancesByMesh[mesh->name()])
            matBuffer.push_back(instances->modelMatrix());
    }

    
    //mModelUFO = new UniformBufferObject(matBuffer.size() * sizeof(glm::mat4), nullptr);        
    for(unsigned int index = 0; index < matBuffer.size(); ++index)
        mUniformBuffers[GLUniBuffer::Model]->update(index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matBuffer[index]));
 
    unsigned int slices = static_cast<unsigned int>(floor(matBuffer.size() / 1000));
    auto lastSliceSize = (matBuffer.size() % 1000) * sizeof(glm::mat4);
        
    for(unsigned int slice = 0; slice < slices; ++slice)
        mUniformBuffers[GLUniBuffer::Model]->bindToIndexedBufferTarget(1 + slice, slice * 1000 * sizeof(glm::mat4), 1000 * sizeof(glm::mat4));
    
    if (lastSliceSize) 
        mUniformBuffers[GLUniBuffer::Model]->bindToIndexedBufferTarget(1 + slices , slices * 1000 * sizeof(glm::mat4), lastSliceSize); 
}

void GLRenderer::updateSizeDependentResources()
{
    mResized = false;
    
    mManager->deleteTexture(mFBO->colorAttachment(0));
    mManager->deleteTexture(mFBO->depthAttachment());

    mManager->deleteTexture(mReadFBTexture);
    mManager->deleteTexture(mReadFBTextureI);
    mManager->deleteTexture(mReadFBTextureUI);
    mManager->deleteTexture(const_cast<Texture*>(mReadFBTarget->depthTexOutput()));
    mReadFBTarget->setDepthTexture(nullptr);
    mManager->deleteTexture(const_cast<Texture*>(mDepthTarget->depthTexOutput()));
    //mManager->deleteTextureArray(mDepthTextArray);

    mManager->createTextureArray(5, {}, GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, 8, false);
    mReadFBTextureUI = mManager->createTexture("readFBTextureUI", GL_TEXTURE_2D, GL_RG32UI, mWindowWidth, mWindowHeight, GL_RG_INTEGER, GL_UNSIGNED_INT, {}, GL_NEAREST, GL_NEAREST, 0, true);
    mReadFBTextureI = mManager->createTexture("readFBTextureI", GL_TEXTURE_2D, GL_RG32I, mWindowWidth, mWindowHeight, GL_RG_INTEGER, GL_INT, {}, GL_NEAREST, GL_NEAREST, 0, true);
    mReadFBTexture = mManager->createTexture("readFBTexture", GL_TEXTURE_2D, GL_RG32F, mWindowWidth, mWindowHeight, GL_RG, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, true);

    auto FBOColorTexture = mManager->createTexture("DefaultFBOColor", GL_TEXTURE_2D, GL_RGB8, mWindowWidth, mWindowHeight, GL_RGB, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, true);
    auto FBODepthTexture = mManager->createTexture("DefaultFBODepth", GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, false);    
    mFBO->setcolorAttachment(FBOColorTexture, 0);
    mFBO->setDepthAttachment(FBODepthTexture);
    
    auto depthTex = mManager->createTexture("DepthTargetDepth", GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mWindowWidth, mWindowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, true);
    mDepthTarget->setDepthTexture(depthTex);
    
    mPaintTexTarget->setDepthInputIndices({depthTex->textureArrayIndex(), depthTex->textureArrayLayerIndex()});
    
    if(mLayers.size())
        updateTechniqueDataWithLayer(mLayers[mCurrentLayerIndex]);
    
    mUniformBuffers[GLUniBuffer::App]->updateCache(0, sizeof(glm::uvec2), glm::value_ptr(mDepthTarget->depthTexOutput()->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateGPU();
        

    insertTechnique(mDepthTech, 1);
}

void GLRenderer::setCurrentLayer(unsigned int index)
{
    LOG("GLRenderer::Set Current layer: ", index);
    
    mCurrentLayerIndex = index;
        
    updateTechniqueDataWithLayer(mLayers[mCurrentLayerIndex]);
}

void GLRenderer::changeLayerPosition(int previous, int current)
{
    auto layer = mLayers[previous];
    
    if(current == mLayerCount)
        mLayers.push_back(layer);
    else
        mLayers.insert(begin(mLayers) + current, layer);
    
    if (current < previous)
    {
        mCurrentLayerIndex = current;
        mLayers.erase(begin(mLayers) + previous + 1);
    }
    else
    {
        mCurrentLayerIndex = current - 1;
        mLayers.erase(begin(mLayers) + previous);
    }

    mPaintingDataNeedUpdate = true;
}

void GLRenderer::insertLayer(unsigned int index, Texture* data, Texture* mask, Texture* palette)
{    
    mCurrentLayerIndex = index;
    LOG("Insert layer: ", index, ", CurrentIndex: ", mCurrentLayerIndex);    
    
    mLayers.emplace(begin(mLayers) + index, data, mask, palette);        
    mLayerCount++;    
    
    updateTechniqueDataWithLayer(mLayers[mCurrentLayerIndex]);
}


void GLRenderer::setCurrentPaintTextures(Texture* data, Texture* mask, Texture* palette)
{
    LOG("GLRenderer::Set textures current layer: Index: ", mCurrentLayerIndex);
    
    mLayers[mCurrentLayerIndex] = GLLayer(data, mask, palette);
    
    updateTechniqueDataWithLayer(mLayers[mCurrentLayerIndex]);
}

void GLRenderer::updateTechniqueDataWithLayer(const GLLayer& layer)
{
    if(!mSliced)
    {
        LOG("GLRenderer::UpdateVisualizationDataWithLayer - normal version shaders");
        
        if(layer.mSamplerType == 0)
        {
            mPaintTexPass->sceneMaterial()->setShader(mManager->shaderProgram("iColorPainting"));
            mDilationPass->sceneMaterial()->setShader(mManager->shaderProgram("iDilation"));
            mReadFBPass->sceneMaterial()->setShader(mManager->shaderProgram("iDataReading"));
            mReadFBTarget->setColorTextures({mReadFBTextureI}, true);
            mReadFBTech->settingUP();        
        }
        else if(layer.mSamplerType == 1)
        {
            mPaintTexPass->sceneMaterial()->setShader(mManager->shaderProgram("uColorPainting"));
            mDilationPass->sceneMaterial()->setShader(mManager->shaderProgram("uDilation"));        
            mReadFBPass->sceneMaterial()->setShader(mManager->shaderProgram("uDataReading"));
            mReadFBTarget->setColorTextures({mReadFBTextureUI}, true);
            mReadFBTech->settingUP();        
        }
        else 
        {
            mPaintTexPass->sceneMaterial()->setShader(mManager->shaderProgram("ColorPainting"));
            mDilationPass->sceneMaterial()->setShader(mManager->shaderProgram("Dilation"));        
            mReadFBPass->sceneMaterial()->setShader(mManager->shaderProgram("DataReading"));
            mReadFBTarget->setColorTextures({mReadFBTexture}, true);
            mReadFBTech->settingUP();        
        }
        
        mEraseTexPass->sceneMaterial()->setShader(mManager->shaderProgram("DataErasing"));
    }
    else
    {
        LOG("GLRenderer::UpdateVisualizationDataWithLayer - Sliced version shaders");
        
        if(layer.mSamplerType == 0)
        {
            mPaintTexPass->sceneMaterial()->setShader(mManager->shaderProgram("iMarkSlice"));
            mDilationPass->sceneMaterial()->setShader(mManager->shaderProgram("iDilation"));
            mReadFBPass->sceneMaterial()->setShader(mManager->shaderProgram("iDataReadSlice"));
            mReadFBTarget->setColorTextures({mReadFBTextureI}, true);
            mReadFBTech->settingUP();        
        }
        else if(layer.mSamplerType == 1)
        {
            mPaintTexPass->sceneMaterial()->setShader(mManager->shaderProgram("uMarkSlice"));
            mDilationPass->sceneMaterial()->setShader(mManager->shaderProgram("uDilation"));        
            mReadFBPass->sceneMaterial()->setShader(mManager->shaderProgram("uDataReadSlice"));
            mReadFBTarget->setColorTextures({mReadFBTextureUI}, true);
            mReadFBTech->settingUP();        
        }
        else 
        {
            mPaintTexPass->sceneMaterial()->setShader(mManager->shaderProgram("MarkSlice"));
            mDilationPass->sceneMaterial()->setShader(mManager->shaderProgram("Dilation"));        
            mReadFBPass->sceneMaterial()->setShader(mManager->shaderProgram("DataReadSlice"));
            mReadFBTarget->setColorTextures({mReadFBTexture}, true);
            mReadFBTech->settingUP();        
        }
        
        mEraseTexPass->sceneMaterial()->setShader(mManager->shaderProgram("DataEraseSlice"));
    }
    
    if(mLayerSize != layer.mValue->dimensions())
    {
        mLayerSize = layer.mValue->dimensions();

        mManager->deleteTexture(mBrushMaskTexture);        
        mBrushMaskTexture = mManager->createTexture("BrushMaskTexture", GL_TEXTURE_2D, GL_R8, mLayerSize.x, mLayerSize.y, GL_RED, GL_UNSIGNED_BYTE, {}, GL_NEAREST, GL_NEAREST, 0, false);
        mManager->deleteTexture(mSeamMaskTexture);
        mSeamMaskTexture = mManager->createTexture("SeamMaskTexture", GL_TEXTURE_2D, GL_RGBA8, mLayerSize.x, mLayerSize.y, GL_RGBA, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false);
        mManager->deleteTexture(mDepthTexTexture);
        mDepthTexTexture = mManager->createTexture("DepthTexTexture", GL_TEXTURE_2D, GL_RGBA8, mLayerSize.x, mLayerSize.y, GL_RGBA, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false);
        mManager->deleteTexture(mAreaPerPixelTexture);
        mAreaPerPixelTexture = mManager->createTexture("AreaPerPixel", GL_TEXTURE_2D, GL_R32F, mLayerSize.x, mLayerSize.y, GL_RED, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, true);
        mManager->deleteTexture(mLockTexture);
        mLockTexture = mManager->createTexture("LockPerPixel", GL_TEXTURE_2D, GL_R32UI, mLayerSize.x, mLayerSize.y, GL_RED_INTEGER, GL_UNSIGNED_INT, {}, GL_NEAREST, GL_NEAREST, 0, false);
        mManager->deleteTexture(mNeighborhoodTexture);
        mNeighborhoodTexture = mManager->createTexture("Neighborhood", GL_TEXTURE_2D, GL_R16UI, mLayerSize.x, mLayerSize.y, GL_RED_INTEGER, GL_UNSIGNED_INT, {}, GL_NEAREST, GL_NEAREST, 0, false);
        
        mSeamMaskTarget->setColorTextures({mSeamMaskTexture});
        mDepthTexTarget->setColorTextures({mDepthTexTexture});
        mSeamMaskTech->settingUP();

        mAreaPerPixelTarget->setColorTextures({mAreaPerPixelTexture});
        mAreaPerPixelTech->settingUP();
        
        //mImmediateNeighborsTarget->setColorTextures({mNeighborhoodTexture});
        //mImmediateNeighborsTech->settingUP();
        //
        //mNeighborsTarget->setColorTextures({mNeighborhoodTexture});    
        //mNeighborsTech->settingUP();
        //
        //mCornerCapTarget->setColorTextures({mNeighborhoodTexture});
        //mCornerCapTech->settingUP();
                
        mUniformBuffers[GLUniBuffer::App]->updateCache(sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mSeamMaskTarget->colorTexOutputs()[0]->textureArrayIndices()));
        mUniformBuffers[GLUniBuffer::App]->updateCache(3 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mDepthTexTarget->depthTexOutput()->textureArrayIndices()));
        mUniformBuffers[GLUniBuffer::App]->updateCache(4 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mBrushMaskTexture->textureArrayIndices()));
        mUniformBuffers[GLUniBuffer::App]->updateCache(5 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mAreaPerPixelTexture->textureArrayIndices()));

        mUniformBuffers[GLUniBuffer::App]->updateGPU();

        insertTechnique(mSeamMaskTech, 1);
        //insertTechnique(mNeighborsTech, 1);
        //insertTechnique(mCornerCapTech, 1);
        //insertTechnique(mImmediateNeighborsTech, 1);
        insertTechnique(mAreaPerPixelTech,1);
    }
    
    mPaintTexTarget->setColorTextures({layer.mValue, layer.mMask, mBrushMaskTexture});
    mDilationTarget->setColorTextures({layer.mValue, layer.mMask});
    
    mPaintTexTech->settingUP();

    mEraseTexTarget->setColorTextures({layer.mMask, mBrushMaskTexture});
    mEraseDilationTarget->setColorTextures({layer.mMask});
    mEraseTexTech->settingUP();
    
    mPaintingDataNeedUpdate = true;    
}


void GLRenderer::setCurrentPaletteTexture(Texture* palette)
{
    LOG("GLRenderer::Set current palette texture: CurrentIndex", mCurrentLayerIndex);
    mLayers[mCurrentLayerIndex].mPalette = palette;    
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::eraseLayer(unsigned int layerIndex)
{
    mLayers.erase(begin(mLayers) + layerIndex);
    if(mLayerCount > 0) mLayerCount--;
    if(mCurrentLayerIndex > 0) mCurrentLayerIndex--;
}


void GLRenderer::setCurrentPaintingValue(float value)
{
    mCurrentPaintingValue = value;
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::updateControlPointText(const std::vector<std::tuple<std::string, int, int>>& data)
{
    if(mFontScene != nullptr)
    {
        LOG("GLRenderer::Update Control Point Text:: Resize: ", mWindowWidth);
        mFontScene->clearScene();
        
        auto textMesh = mFontManager->generateMeshText("arial.ttf","Alpha Version 0.6.3", 20, 30);
        auto fontMeshMat = mManager->material("FontTex");
        auto fontModel = new Model3D(textMesh->name(), textMesh, { fontMeshMat });
        mFontScene->insertModel(fontModel);
        
        for(const auto& pair: data)
        {   
            LOG("Control Point x: ", mWindowWidth - 2, ", y: " , std::get<1>(pair));
            auto fontMesh = mFontManager->generateMeshText("arial.ttf", std::get<0>(pair), mWindowWidth - 2, std::get<1>(pair), std::get<2>(pair));
            auto fontModel = mFontScene->findModel3D(fontMesh->name());
            
            if(fontModel != nullptr)
            {
                fontModel->mesh()->updateData(GLBuffer::UV, 0, fontMesh->buffer(GLBuffer::UV).size(), fontMesh->buffer(GLBuffer::UV).data());
                fontModel->mesh()->updateData(GLBuffer::Vertex, 0, fontMesh->buffer(GLBuffer::Vertex).size(), fontMesh->buffer(GLBuffer::Vertex).data());
                fontModel->mesh()->updateData(GLBuffer::Normal, 0, fontMesh->buffer(GLBuffer::Normal).size(), fontMesh->buffer(GLBuffer::Normal).data());
                fontModel->mesh()->updateData(GLBuffer::Color, 0, fontMesh->buffer(GLBuffer::Color).size(), fontMesh->buffer(GLBuffer::Color).data());
                fontModel->mesh()->updateData(GLBuffer::Index, 0, fontMesh->buffer(GLBuffer::Index).size(), fontMesh->buffer(GLBuffer::Index).data());
                fontModel->mesh()->updateSubMeshData(fontMesh->subMeshes(), 0);
            }
            else
            {
                auto fontMeshMat = mManager->material("FontTex");
                fontModel = new Model3D(fontMesh->name(), fontMesh, { fontMeshMat });
                mFontScene->insertModel(fontModel);
            }
        }
    }
}

void GLRenderer::updatePaletteTexture(unsigned int index, const std::array<float, 4096 * 4>& data, std::pair<float, float> range)
{
    LOG("GLRenderer::Update current palette data: Index: ", index);
    
    if(index < mLayers.size())
    {
        auto palette = mLayers[index].mPalette;
//         LOG("Update palette ", palette->name(), ", arrayId: ", palette->textureArrayId(), ", layer: ", palette->textureArrayLayerIndex(), ", width: ", palette->width(), ", format: ",  palette->format());
        glTextureSubImage2D(palette->textureArrayId(), 0, 0, palette->textureArrayLayerIndex(), palette->width(), 1, palette->format(), palette->type(), data.data());
        glGenerateTextureMipmap(palette->textureArrayId());
        
        mLayers[index].mPaletteRange = glm::vec2(range.first, range.second);
        
        mPaintingDataNeedUpdate = true;
    }
}


void GLRenderer::updateCurrentPaletteTexture(const std::array< float, int(16384) >& data, std::pair< float, float > range)
{
    LOG("GLRenderer::Update current palette data: Current Index: ", mCurrentLayerIndex);
    updatePaletteTexture(mCurrentLayerIndex, data, range);
}

void GLRenderer::setOpacity(unsigned int layerIndex, float opacity)
{
    mLayers[layerIndex].mOpacity = opacity;
    mPaintingDataNeedUpdate = true;
}

void GLRenderer::computeExpression(const std::vector<std::string>& expression)
{
    
    for(auto shader: expression)
    {
        auto compJob = new ComputeJob(shader, mManager->shaderProgram(shader), {64, 64, 1}, {32, 32, 1});

        std::vector<GLint> glIndices;
        
        for(auto index: {0})
        {
            if(index > -1)
            {
                auto value = mLayers[index].mValue;
                auto mask = mLayers[index].mMask;            
                
                glIndices.push_back(value->textureArrayLayerIndex());
                glIndices.push_back(mask->textureArrayLayerIndex());
            }
        }
        
        compJob->setIndices(glIndices);
        compJob->setMemoryBarriers(GL_ALL_BARRIER_BITS);
        
        insertComputeJob(compJob);
    }
}

void GLRenderer::computeShader(Program* shader, const std::vector<glm::byte>& uniformData)
{    
    auto layer = mLayers[0];
    auto compJob = new ComputeJob(shader->name(), shader, {static_cast<uint32_t>(layer.mValue->width()/32), static_cast<uint32_t>(layer.mValue->height()/32), 1}, {32, 32, 1});
    
    std::vector<GLint> glIndices;
    glIndices.push_back(layer.mValue->textureArrayLayerIndex());
    glIndices.push_back(layer.mMask->textureArrayLayerIndex());
    
    compJob->setIndices(glIndices);
    compJob->setMemoryBarriers(GL_ALL_BARRIER_BITS);
    
    if(uniformData.size())
        compJob->updateData(0, uniformData.size(), uniformData.data());

    insertComputeJob(compJob); 
}


std::vector<glm::byte> GLRenderer::readTexture(Texture* texture)
{ 
    std::vector<glm::byte> textureData(texture->height() * texture->width() * sizeOfGLInternalFormat(texture->internalFormat()));    
    glGetTextureSubImage(texture->textureArrayId(), 0, 0, 0, texture->textureArrayLayerIndex(), texture->width(), texture->height(), 1, texture->format(), texture->type(), textureData.size(), textureData.data());
    return textureData;
}

template<typename T>
std::vector<T> GLRenderer::readTexture(Texture* texture)
{    
    std::vector<T> textureData(texture->height() * texture->width() * 2);    
    glGetTextureSubImage(texture->textureArrayId(), 0, 0, 0, texture->textureArrayLayerIndex(), texture->width(), texture->height(), 1, texture->format(), texture->type(), textureData.size() * sizeOfGLType(texture->type()), textureData.data());
    return textureData;    
}

std::vector<char> GLRenderer::readCharTexture(Texture* texture)
{
    return readTexture<char>(texture);
}

std::vector<uint16_t> GLRenderer::readUShortTexture(Texture* texture)
{
    return readTexture<uint16_t>(texture);
}

std::vector<int16_t> GLRenderer::readShortTexture(Texture* texture)
{
    return readTexture<int16_t>(texture);
}

std::vector<uint32_t> GLRenderer::readUIntTexture(Texture* texture)
{
    return readTexture<uint32_t>(texture);
}

std::vector<int32_t> GLRenderer::readIntTexture(Texture* texture)
{
    return readTexture<int32_t>(texture);
}

std::vector<half_float::half> GLRenderer::readHalfFloatTexture(Texture* texture)
{
    return readTexture<half_float::half>(texture);
}

std::vector<float> GLRenderer::readFloatTexture(Texture* texture)
{
    return readTexture<float>(texture);
}

std::vector<glm::byte> GLRenderer::readLayerData(unsigned int layerIndex)
{
    return readTexture(mLayers[layerIndex].mValue);
}

std::vector<glm::byte> GLRenderer::readLayerMask(unsigned int layerIndex)
{
    return readTexture(mLayers[layerIndex].mMask);
}


void GLRenderer::insertTechnique(RenderTechnique* tech, int life)
{
    tech->setRenderer(this);
    
    (tech->life() > 0) ? tech->setLife(tech->life() + life) : tech->setLife(life);

    auto search = std::find(begin(mRenderQueue), end(mRenderQueue), tech);
    if(search == end(mRenderQueue)) mRenderQueue.push_back(tech);
    
//     if(life != 0) tech->setLife(life);
//     mRenderQueue.push_back(tech);

    for(const auto target: tech->targets())
        for(const auto pass: target->passes())
            insertScene(pass->scene());
}

void GLRenderer::insertComputeJob(ComputeJob* tech, int life)
{
    auto search = std::find(begin(mComputeQueue), end(mComputeQueue), tech);
    if(search == end(mComputeQueue)) mComputeQueue.push_back(tech);
}


void GLRenderer::insertScene(Scene3D* scene)
{
    if(scene->index() < 0)
    {
        scene->setRenderer(this);
        scene->setIndex(static_cast<int>(mScenes.size()));
        mScenes.push_back(scene);
        
        mDirtyScenes.insert(scene->index());
    }
}

void GLRenderer::removeScene(Scene3D* scene)
{
    if(scene->index() >= 0)
    {
        auto sceneIndex = scene->index();
        auto meshOffsets =  scene->drawMeshOffset(0);
        invalidateVertexDataFrom(meshOffsets.attrib);
        invalidateIndexDataFrom(meshOffsets.index);
        invalidateInstanceDataFrom(meshOffsets.instance);
                    
        invalidateScenesFrom(scene->index());
        
        mScenes.erase(begin(mScenes) + sceneIndex);
    }
}

void GLRenderer::swapScene(Scene3D* oldScene, Scene3D* newScene)
{    
    mScene = newScene;
    mMainScene = newScene;   
    mCamera = mMainScene->camera();
    mCameraInitPosition = mCamera->position();
    mMainScene->setViewCameraNeedUpdate(true);
    mMainScene->setProjCameraNeedUpdate(true);

    mMainScene->updateCamera(mNormOrthoCamera.get());
    
    mSlicePlaneScene->clearCameras();
    mSlicePlaneScene->insertCamera(mCamera);
    
    auto modelBBox = mMainScene->models()[0]->mesh()->boundingBox();
    auto diagonal = glm::length(glm::vec3(mMainScene->models()[0]->modelMatrix() * glm::vec4(modelBBox.max - modelBBox.min, 1.0)));    
    auto slicePlaneModel = mSlicePlaneScene->findModel3D("SlicePlaneModel");
    auto arrowTorsoModel = mSlicePlaneScene->findModel3D("ArrowTorso");
    auto arrowHeadModel = mSlicePlaneScene->findModel3D("ArrowHead");
    
    static_cast<Quad*>(slicePlaneModel->mesh())->resize({-diagonal/2.0, -diagonal/2.0}, {diagonal, diagonal});
    static_cast<Cylinder*>(arrowTorsoModel->mesh())->resize(diagonal/120, diagonal/120, diagonal/4);    
    static_cast<Cylinder*>(arrowHeadModel->mesh())->resize(diagonal/40, 0, diagonal/25);
            
    glm::mat4 mat(1.0);
    mat = glm::rotate(mat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    mat = glm::translate(mat, {0, diagonal/8, 0});
    arrowTorsoModel->applyTransform(mat, 0, false);

    mat = glm::mat4(1.0);
    mat = glm::rotate(mat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    mat = glm::translate(mat, {0, diagonal/4 + diagonal/60, 0});
    arrowHeadModel->applyTransform(mat, 0, false);
        
    mSliced = false;
    mProjPass->sceneMaterial()->setShader(mManager->shaderProgram("ProjectiveTex"));
    slicePlaneModel->setModelMatrix(glm::mat4(1.0));
    arrowHeadModel->setModelMatrix(glm::mat4(1.0));
    arrowTorsoModel->setModelMatrix(glm::mat4(1.0));
    updateSlicePlaneUniformData();
        
    mProjPass->setScene(mMainScene);

    mDepthTexTarget->passes().back()->setScene(mMainScene);        
    mDepthTarget->passes().back()->setScene(mMainScene);
    mPaintTexPass->setScene(mMainScene);
    mReadFBPass->setScene(mMainScene);
    mEraseTexTarget->passes().back()->setScene(mMainScene); 
    mAreaPerPixelTech->targets()[0]->passes().back()->setScene(mMainScene);

    mLayerCount = 0;
    mLayers.clear();

    invalidateScenesFrom(0);
    mRenderQueue.clear();
    mScenes.clear();    
    mFontScene->clearScene();

    insertTechnique(mSeamMaskTech, 1);
    insertTechnique(mDepthTech, 1);
    insertTechnique(mProjTech);
    insertTechnique(mAreaPerPixelTech, 1);
    //insertTechnique(mViewTexTech);    
    insertTechnique(mUITech);
}

void GLRenderer::loadChiselScene(Scene3D* scene)
{
    if(mMainScene != nullptr)
       swapScene(mMainScene, scene);
    else
    {
        std::array<GLint, 4> viewport = { 0, 0, mWindowWidth, mWindowHeight }; //VS15
        
        mScene = scene;
        mMainScene = scene;
        mCamera = scene->camera();
        mCameraInitPosition = mCamera->position();
        mMainScene->setViewCameraNeedUpdate(true);
        mMainScene->setProjCameraNeedUpdate(true);
                            
        mMainScene->updateCamera(mNormOrthoCamera.get());
                        
        mReadFBTexture = mManager->createTexture("readFBTexture", GL_TEXTURE_2D, GL_RG32F, mWindowWidth, mWindowHeight, GL_RG, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, true);
        mReadFBTextureI = mManager->createTexture("readFBTextureI", GL_TEXTURE_2D, GL_RG32I, mWindowWidth, mWindowHeight, GL_RG_INTEGER, GL_INT, {}, GL_NEAREST, GL_NEAREST, 0, true);
        mReadFBTextureUI = mManager->createTexture("readFBTextureUI", GL_TEXTURE_2D, GL_RG32UI, mWindowWidth, mWindowHeight, GL_RG_INTEGER, GL_UNSIGNED_INT, {}, GL_NEAREST, GL_NEAREST, 0, true);
        mAreaPerPixelTexture = mManager->createTexture("AreaPerPixel", GL_TEXTURE_2D, GL_R32F, 2048, 2048, GL_RED, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, true);
        mLockTexture = mManager->createTexture("LockPerPixel", GL_TEXTURE_2D, GL_R32UI, 2048, 2048, GL_RED_INTEGER, GL_UNSIGNED_INT, {}, GL_NEAREST, GL_NEAREST, 0, false);
        mNeighborhoodTexture = mManager->createTexture("Neighborhood", GL_TEXTURE_2D, GL_RG32F, 2048, 2048, GL_RG, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 0, false);
        
        auto quad = mManager->createQuad("PPQuad", {0.0, 0.0}, {2048, 2048});
        auto ppQuadModel = new Model3D("ppModel", quad, {mManager->material("Dummy")});
        auto ppScene = new Scene3D("ppScene", {mNormOrthoCamera.get()}, {ppQuadModel});        
        
        //TODO Soportar quizas una camara por defecto en la pasada al igual que el material
        auto depthTexMat = mManager->createMaterial("DepthTexMat", "DepthTex");
        auto depthTexPass = mManager->createRenderPass("DepthTexPass", mMainScene, depthTexMat);
        mDepthTexTarget = mManager->createRenderTarget("DepthTexTarget", mManager, {0, 0, 2048, 2048}, {depthTexPass}, false);
        
        auto seamMaskMat = mManager->createMaterial("SeamMaskMat", "SeamMask");
        auto seamMaskPass = mManager->createRenderPass("SeamMaskPass", ppScene, seamMaskMat);
        mSeamMaskTarget = mManager->createRenderTarget("SeamMaskTarget", mManager, { 0, 0, 2048, 2048 }, {seamMaskPass}, false);
                                    
        auto depthMat = mManager->createMaterial("DepthMat", "Depth");
        auto depthPass = mManager->createRenderPass("DepthPass", mMainScene, depthMat);
        auto depthColor = mManager->createTexture("DepthCTargetDepth", GL_TEXTURE_2D, GL_R32F, viewport[2], viewport[3], GL_RED, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, true);
        auto depthTex = mManager->createTexture("DepthTargetDepth", GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, viewport[2], viewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, true);
        mDepthTarget = mManager->createRenderTarget("DepthTarget", mManager, viewport, { depthPass }, {}, depthTex, false);

        auto paintTexMat = mManager->createMaterial("PaintTexMat", "ColorPainting");
        mPaintTexPass = mManager->createRenderPass("PaintTexPass", mMainScene, paintTexMat);
        mPaintTexPass->setAutoClearColor(false);
        mPaintTexTarget = mManager->createRenderTarget("PaintTexTarget", mManager, { 0, 0, 2048, 2048 }, { mPaintTexPass }, {}, nullptr, false);

        auto dilationMat = mManager->createMaterial("dilationMat", "Dilation");
        mDilationPass = mManager->createRenderPass("dilationPass", ppScene, dilationMat);
        mDilationPass->setAutoClearColor(false);
        mDilationTarget = mManager->createRenderTarget("dilationTarget", mManager, { 0, 0, 2048, 2048 }, { mDilationPass }, {}, nullptr, false);
                
        auto projMat = mManager->createMaterial("projMat", "ProjectiveTex");
        mProjPass = mManager->createRenderPass("projPass", mMainScene, projMat);
        mProjTarget = mManager->createRenderTarget("projTarget", mManager, viewport, {mProjPass}, true, mFBO->id());
        
        auto readFBMat = mManager->createMaterial("readFBMat", "DataReading");
        mReadFBPass = mManager->createRenderPass("readFBPass", mMainScene, readFBMat);
        std::vector< Texture* > readTextures;
        readTextures.push_back(mReadFBTexture);
        mReadFBTarget = mManager->createRenderTarget("readFBTarget", mManager, viewport, {mReadFBPass}, readTextures, false);
        
        auto eraseTexMat = mManager->createMaterial("eraseTexMat", "DataErasing");
        mEraseTexPass = mManager->createRenderPass("eraseTexPass", mMainScene, eraseTexMat);
        mEraseTexPass->setAutoClearColor(false);
        mEraseTexTarget = mManager->createRenderTarget("eraseTexTarget", mManager, {0, 0, 2048, 2048}, {mEraseTexPass}, false);

        auto eraseDilationMat = mManager->createMaterial("eraseDilationMat", "EraseDilation");
        auto eraseDilationPass = mManager->createRenderPass("eraseDilationPass", ppScene, eraseDilationMat);
        eraseDilationPass->setAutoClearColor(false);
        mEraseDilationTarget = mManager->createRenderTarget("eraseDilationTarget", mManager, { 0, 0, 2048, 2048 }, { eraseDilationPass }, {}, nullptr, false);
        
        auto viewTexMat = mManager->createMaterial("ViewTexMat", "ViewTex");
        auto viewTexPass = mManager->createRenderPass("viewTexPass", ppScene, viewTexMat);
        viewTexPass->setAutoClear(false);
        auto viewTexTarget = mManager->createRenderTarget("vieTexTarget", mManager, { 0, 0, 512, 512 }, { viewTexPass }, true, mFBO->id());
        mViewTexTech = mManager->createRenderTechnique("viewTexTech", {viewTexTarget});

        auto areaPerPixelMat = mManager->createMaterial("AreaPerPixel", "AreaPerPixel");
        auto areaPerPixelPass = mManager->createRenderPass("AreaPerPixel", mMainScene, areaPerPixelMat);
        areaPerPixelPass->disableDepthTest();
        //auto dummyTex = mManager->createTexture("dummy", GL_TEXTURE_2D, GL_RGBA8, 2048, 2048, GL_RGBA, GL_UNSIGNED_BYTE, {}, GL_NEAREST, GL_NEAREST, 0, true);
        mAreaPerPixelTarget = mManager->createRenderTarget("AreaPerPixelTarget", mManager, { 0, 0, 2048, 2048 }, { areaPerPixelPass }, {mAreaPerPixelTexture}, nullptr, false);    
        mAreaPerPixelTech = mManager->createRenderTechnique("AreaPerPixelTech", {mAreaPerPixelTarget});
                
        mManager->commitFreeImageUnit(mAreaPerPixelTexture->textureArray());
        mManager->commitFreeImageUnit(mLockTexture->textureArray());
        
        
        //auto immediateNeighborsMat = mManager->createMaterial("ImmediateNeighborsMat", "ImmediateNeighbors");
        //auto immediateNeighborsPass = mManager->createRenderPass("ImmediateNeighborsPass", ppScene, immediateNeighborsMat);
        //mImmediateNeighborsTarget = mManager->createRenderTarget("ImmediateNeighborsTarget", mManager, { 0, 0, 2048, 2048 }, { immediateNeighborsPass }, {mNeighborhoodTexture}, nullptr, false);
        //mImmediateNeighborsTech = mManager->createRenderTechnique("ImmediateNeighborsTech", {mImmediateNeighborsTarget});
        //
        //auto neighborhoodMesh = mManager->copyMesh(*mMainScene->meshes()[0], "neighborhoodMesh");
        //neighborhoodMesh->generateAdjacencyInformation();
        //auto neighborhoodModel = new Model3D("neighborhoodModel", neighborhoodMesh, {mManager->material("Dummy")});
        //auto neighborhoodScene = new Scene3D("neighborhoodScene", {mNormOrthoCamera.get()}, {neighborhoodModel});
        ////neighborhoodScene->setPrimitive(Primitive::Lines);
        //
        //auto neighborsMat = mManager->createMaterial("NeighborsMat", "Neighbors");
        //auto neighborsPass = mManager->createRenderPass("NeighborsPass", neighborhoodScene, neighborsMat);
        ////neighborsPass->disableDepthTest();
        //neighborsPass->setAutoClear(false);
        //mNeighborsTarget = mManager->createRenderTarget("NeighborsTarget", mManager, { 0, 0, 2048, 2048 }, { neighborsPass }, {mNeighborhoodTexture}, nullptr, false);
        //mNeighborsTech = mManager->createRenderTechnique("NeighborsTech", {mNeighborsTarget});
        //
        //auto cornerCapMat = mManager->createMaterial("CornerCapMat", "CornerCap");
        //auto cornerCapPass = mManager->createRenderPass("CornerCapPass", ppScene, cornerCapMat);
        //mCornerCapTarget = mManager->createRenderTarget("CornerCapTarget", mManager, { 0, 0, 2048, 2048 }, { cornerCapPass }, {mNeighborhoodTexture}, nullptr, false);
        //mCornerCapTech = mManager->createRenderTechnique("CornerCapTech", {mCornerCapTarget});
        
        auto modelBBox = mMainScene->models()[0]->mesh()->boundingBox();
        auto diagonal = glm::length(glm::vec3(mMainScene->models()[0]->modelMatrix() * glm::vec4(modelBBox.max - modelBBox.min, 1.0)));
        auto slicePlaneMesh = mManager->createQuad("SlicePlaneModel", {-diagonal/2.0, -diagonal/2.0}, {diagonal, diagonal}, {0.5, 0.5, 0.5, 0.3});
        auto slicePlaneModel = new Model3D("SlicePlaneModel", slicePlaneMesh, {mManager->material("Dummy")});
        auto arrowTorsoMesh = mManager->createCylinder("ArrowTorso", diagonal/120, diagonal/120, diagonal/4, 20, 1, false, {0.8, 0.0, 0.0, 1.0}); 
        auto arrowHeadMesh = mManager->createCylinder("ArrowHead", diagonal/40, 0, diagonal/25, 20, 1, false, {0.8, 0.0, 0.0, 1.0});
        auto arrowTorsoModel = new Model3D("ArrowTorso", arrowTorsoMesh, {mManager->material("Dummy")});
        auto arrowHeadModel = new Model3D("ArrowHead", arrowHeadMesh, {mManager->material("Dummy")});
//         arrowTorsoModel->translate(0, 75, 0);
//         arrowTorsoModel->rotate(glm::radians(-90.0), glm::vec3(1, 0, 0));
        glm::mat4 mat(1.0);
        mat = glm::rotate(mat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        mat = glm::translate(mat, {0, diagonal/8, 0});
        arrowTorsoModel->applyTransform(mat, 0, true);
//         arrowTorsoModel->translate(0, 75, 0);
//         arrowTorsoModel->rotate(glm::radians(-90.0), glm::vec3(1, 0, 0));
        
        mat = glm::mat4(1.0);
        mat = glm::rotate(mat, glm::radians(-90.0f), glm::vec3(1, 0, 0));        
        mat = glm::translate(mat, {0, diagonal/4 + diagonal/50, 0});
        arrowHeadModel->applyTransform(mat, 0, true);
//         arrowHeadModel->translate(0, 165, 0);
//         arrowHeadModel->rotate(glm::radians(-90.0), glm::vec3(1, 0, 0));
        
        mSlicePlaneScene = new Scene3D("SlicePlaneScene", {mCamera}, {arrowHeadModel, arrowTorsoModel, slicePlaneModel});

        auto slicePlaneMaterial = mManager->createMaterial("SlicePlane", "SlicePlane");
        auto slicePlanePass = mManager->createRenderPass("SlicePlanePass", mSlicePlaneScene, slicePlaneMaterial);
        slicePlanePass->setAutoClear(false);
        slicePlanePass->enableBlending(0);
        mSlicePlaneTarget = mManager->createRenderTarget("SlicePlaneTarget", mManager, viewport, {slicePlanePass}, true, mFBO->id());
        

        mDepthTech = mManager->createRenderTechnique("DepthPrePassTechnique", {mDepthTarget}, 1);
        mPaintTexTech = mManager->createRenderTechnique("PaintTexTechnique", {mDepthTarget, mPaintTexTarget, mDilationTarget});
        mPaintTexTech->removeTarget(mDepthTarget);
        mReadFBTech = mManager->createRenderTechnique("ReadFBTech", {mReadFBTarget});
        mEraseTexTech = mManager->createRenderTechnique("EraseTexTech", {mEraseTexTarget, mEraseDilationTarget});
        mProjTech = mManager->createRenderTechnique("ProjTechnique", {mProjTarget});
        mSeamMaskTech = mManager->createRenderTechnique("seamMaskTech", {mDepthTexTarget, mSeamMaskTarget});
                
        invalidateScenesFrom(0);        
        mRenderQueue.clear();
        mScenes.clear();
        
        insertTechnique(mSeamMaskTech, 1);
        //insertTechnique(mNeighborsTech, 1);
        //insertTechnique(mCornerCapTech, 1);
        //insertTechnique(mImmediateNeighborsTech, 1);
        insertTechnique(mDepthTech, 1);
        insertTechnique(mProjTech);
        insertTechnique(mAreaPerPixelTech, 1);
        //insertTechnique(mViewTexTech);
        insertTechnique(mUITech);
        
        initAppUniformData();
        initUniformData();

        mMainSceneIndex = mMainScene->index();
        resize(mWindowWidth, mWindowHeight);
    }
}

void GLRenderer::swapChiselscene(Scene3D* newScene)
{
    //swapScene(mMainScene, newScene);
    loadChiselScene(newScene);
}



void GLRenderer::setSceneDirty(int index)
{
    if(index > -1)
    {
        mDirtyScenes.insert(index);
        
        for(int i = index + 1; i < mScenes.size(); ++i)
        {            
            mDirtyScenes.insert(i);
            mScenes[i]->setFullUpdate();
        }
    }
}

void GLRenderer::invalidateVertexDataFrom(int index)
{
    if(index > -1 && index < mVertexCount)
        mVertexCount = index;
}

void GLRenderer::invalidateIndexDataFrom(int index)
{
    if(index > -1 && index < mIndexCount)
        mIndexCount = index;    
}

void GLRenderer::invalidateInstanceDataFrom(int index)
{
    if(index > -1 && index < mInstanceOffset)
        mInstanceOffset = index;        
}

void GLRenderer::invalidateDrawDataFrom(MeshOffsets offsets)
{
    mVertexCount = offsets.attrib;
    mIndexCount = offsets.index;
    mInstanceOffset = offsets.instance;
}

void GLRenderer::invalidateScenesFrom(unsigned int index)
{
    for(auto i = index; i < mScenes.size(); ++i)
    {
        if(mScenes[i] != nullptr)
        {
            mScenes[i]->setIndex(-1);
            mScenes[i]->setFullUpdate();
        }
    }    
}


// *** Private Methods *** //

void GLRenderer::update()
{
    // Update Camera
    
//     if(mUpdateCamera)
//     {
// 
//         mUniformBuffers[GLUniBuffer::Frame]->update(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mCamera->viewMatrix()));
//         
//         mUpdateCamera = false;        
//     }
    
    for(auto dirtyScene: mDirtyScenes)
        updateMultiDrawCommand(mScenes[dirtyScene]);

    mDirtyScenes.clear();
}

void GLRenderer::initGLBuffers()
{        
    if(mVAO == 0)
        glCreateVertexArrays(1, &mVAO);
    
    mBuffers.resize(GLBuffer::Count);

    mBuffers[GLBuffer::Index] = std::make_unique<ArrayBufferObject>(GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, 1, 0, mIndexBufferCache.size() * sizeof(GLuint), mIndexBufferCache.data());    
    glVertexArrayElementBuffer(mVAO, mBuffers[GLBuffer::Index]->id());


    mBuffers[GLBuffer::Vertex] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 3, 0, mVertexBufferCache.size(), mVertexBufferCache.data());    

    glEnableVertexArrayAttrib(mVAO, static_cast<GLuint>(GLBuffer::Vertex));
    glVertexArrayAttribFormat(mVAO, static_cast<GLuint>(GLBuffer::Vertex), mBuffers[GLBuffer::Vertex]->componentCount(), GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, static_cast<GLuint>(GLBuffer::Vertex), mBuffers[GLBuffer::Vertex]->id(), 0, 3 * mBuffers[GLBuffer::Vertex]->componentSize());    
    glVertexArrayAttribBinding(mVAO,static_cast<GLuint>(GLBuffer::Vertex), static_cast<GLuint>(GLBuffer::Vertex));
    

    mBuffers[GLBuffer::Normal] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 3, 0, mNormalBufferCache.size(), mNormalBufferCache.data());
    
    glEnableVertexArrayAttrib(mVAO, static_cast<GLuint>(GLBuffer::Normal));
    glVertexArrayAttribFormat(mVAO, static_cast<GLuint>(GLBuffer::Normal), mBuffers[GLBuffer::Normal]->componentCount(), GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, static_cast<GLuint>(GLBuffer::Normal), mBuffers[GLBuffer::Normal]->id(), 0, 3 * mBuffers[GLBuffer::Normal]->componentSize());    
    glVertexArrayAttribBinding(mVAO,static_cast<GLuint>(GLBuffer::Normal), static_cast<GLuint>(GLBuffer::Normal));
    

    mBuffers[GLBuffer::UV] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 2, 0, mUVBufferCache.size(), mUVBufferCache.data());
    
    glEnableVertexArrayAttrib(mVAO, static_cast<GLuint>(GLBuffer::UV));
    glVertexArrayAttribFormat(mVAO, static_cast<GLuint>(GLBuffer::UV), mBuffers[GLBuffer::UV]->componentCount(), GL_FLOAT, GL_FALSE, 0);    
    glVertexArrayVertexBuffer(mVAO, static_cast<GLuint>(GLBuffer::UV), mBuffers[GLBuffer::UV]->id(), 0, 2 * mBuffers[GLBuffer::UV]->componentSize());
    glVertexArrayAttribBinding(mVAO,static_cast<GLuint>(GLBuffer::UV), static_cast<GLuint>(GLBuffer::UV));
    
    
    mBuffers[GLBuffer::TAIndex] = std::make_unique<ArrayBufferObject>(GL_INT, 2, 1, mTexArrayIndexBufferCache.size() * 2 * sizeof(GLint), mTexArrayIndexBufferCache.data());
    
    glEnableVertexArrayAttrib(mVAO, static_cast<GLuint>(GLBuffer::TAIndex));
    glVertexArrayAttribIFormat(mVAO, static_cast<GLuint>(GLBuffer::TAIndex), mBuffers[GLBuffer::TAIndex]->componentCount(), GL_INT, 0);    
    glVertexArrayVertexBuffer(mVAO, static_cast<GLuint>(GLBuffer::TAIndex), mBuffers[GLBuffer::TAIndex]->id(), 0, 2 * mBuffers[GLBuffer::TAIndex]->componentSize());
    glVertexArrayAttribBinding(mVAO,static_cast<GLuint>(GLBuffer::TAIndex), static_cast<GLuint>(GLBuffer::TAIndex));
    glVertexArrayBindingDivisor(mVAO, static_cast<GLuint>(GLBuffer::TAIndex), mBuffers[GLBuffer::TAIndex]->divisor());
    
     
    mBuffers[GLBuffer::Color] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 4, 0, mColorBufferCache.size(), mColorBufferCache.data());
    
    glEnableVertexArrayAttrib(mVAO, static_cast<GLuint>(GLBuffer::Color));
    glVertexArrayAttribFormat(mVAO, static_cast<GLuint>(GLBuffer::Color), mBuffers[GLBuffer::Color]->componentCount(), GL_FLOAT, GL_FALSE, 0);    
    glVertexArrayVertexBuffer(mVAO, static_cast<GLuint>(GLBuffer::Color), mBuffers[GLBuffer::Color]->id(), 0, 4 * mBuffers[GLBuffer::Color]->componentSize());
    glVertexArrayAttribBinding(mVAO,static_cast<GLuint>(GLBuffer::Color), static_cast<GLuint>(GLBuffer::Color));
    

    
    mUniformBuffers[GLUniBuffer::Model] = std::make_unique<UniformBufferObject>(mUniformBufferSize * sizeof(glm::mat4), nullptr, sizeof(glm::mat4));
}

void GLRenderer::initGLBuffers(unsigned int attribCount, unsigned int indexCount, unsigned int uniformCount)
{
    mBuffers.resize(GLBuffer::Count);
    
    mBuffers[GLBuffer::Index] = std::make_unique<ArrayBufferObject>(GL_ELEMENT_ARRAY_BUFFER, GL_UNSIGNED_INT, 1, 0, indexCount * sizeof(GLuint));

    mBuffers[GLBuffer::Vertex] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 3, 0, 3 * attribCount * sizeof(GLfloat));    
    mBuffers[GLBuffer::Normal] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 3, 0, 3 * attribCount * sizeof(GLfloat));
    mBuffers[GLBuffer::UV] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 2, 0, 2 * attribCount * sizeof(GLfloat));    
    mBuffers[GLBuffer::TAIndex] = std::make_unique<ArrayBufferObject>(GL_INT, 2, 1, 2 * attribCount / 3 * sizeof(GLint));    
    mBuffers[GLBuffer::Color] = std::make_unique<ArrayBufferObject>(GL_FLOAT, 4, 0, 4 * attribCount * sizeof(GLfloat));

    mUniformBuffers[GLUniBuffer::Model] = std::make_unique<UniformBufferObject>(mUniformBufferSize * sizeof(glm::mat4), nullptr, sizeof(glm::mat4));        
}

void GLRenderer::resizeGLBuffers(unsigned int attribCount, unsigned int indexCount)
{
//     auto attribCapacity = mBuffers[GLBuffer::Vertex]->capacity() / (mBuffers[GLBuffer::Vertex]->componentCount() * mBuffers[GLBuffer::Vertex]->componentSize());
//     auto indexCapacity = mBuffers[GLBuffer::Index]->capacity() / mBuffers[GLBuffer::Index]->componentSize();
    
    if(mBuffers[GLBuffer::Vertex]->capacity() < attribCount * mBuffers[GLBuffer::Vertex]->componentCount() * mBuffers[GLBuffer::Vertex]->componentSize())
    {
        for(int i = 0; i < GLBuffer::Index; ++i)
        {
            if (mBuffers[i]->divisor() == 0)
            {
                mBuffers[i]->resize(attribCount * mBuffers[i]->componentCount() * mBuffers[i]->componentSize());
                mBuffers[i]->setFullCacheUpdate();
            }
        }
    }
    
    if(mBuffers[GLBuffer::Index]->capacity() < indexCount * mBuffers[GLBuffer::Index]->componentSize())
    {
        mBuffers[GLBuffer::Index]->resize(indexCount * mBuffers[GLBuffer::Index]->componentSize());
        mBuffers[GLBuffer::Index]->setFullCacheUpdate();
    }
    
//     attribCapacity = mBuffers[GLBuffer::Vertex]->capacity() / (mBuffers[GLBuffer::Vertex]->componentCount() * mBuffers[GLBuffer::Vertex]->componentSize());
//     indexCapacity = mBuffers[GLBuffer::Index]->capacity() / mBuffers[GLBuffer::Index]->componentSize();
//     auto a = 0;
}

void GLRenderer::resizeInstanceBuffers(unsigned int instanceCount)
{
    if(mBuffers[GLBuffer::TAIndex]->capacity() < instanceCount * mBuffers[GLBuffer::TAIndex]->componentCount() * mBuffers[GLBuffer::TAIndex]->componentSize() / mBuffers[GLBuffer::TAIndex]->divisor())
    {
        mBuffers[GLBuffer::TAIndex]->resize(instanceCount * mBuffers[GLBuffer::TAIndex]->componentCount() * mBuffers[GLBuffer::TAIndex]->componentSize() / mBuffers[GLBuffer::TAIndex]->divisor());
        mBuffers[GLBuffer::TAIndex]->setFullCacheUpdate();
    }    
}



void GLRenderer::updateGLBuffers()
{    
/*    mStaticVertices->bind();
    std::copy(begin(mVertexBufferCache), end(mVertexBufferCache), reinterpret_cast<GLfloat *>(mStaticVertices->dataPointer()));
    
    mStaticNormals->bind();
    std::copy(begin(mNormalBufferCache), end(mNormalBufferCache), reinterpret_cast<GLfloat *>(mStaticNormals->dataPointer()));
    
    mStaticUVs->bind();
    std::copy(begin(mUVBufferCache), end(mUVBufferCache), reinterpret_cast<GLfloat *>(mStaticUVs->dataPointer()));

    mStaticTexArrayIndices->bind();
    std::copy(begin(mTexArrayIndexBufferCache), end(mTexArrayIndexBufferCache), reinterpret_cast<GLint *>(mStaticTexArrayIndices->dataPointer()));

    mStaticIndices->bind();
    std::copy(begin(mIndexBufferCache), end(mIndexBufferCache), reinterpret_cast<GLuint *>(mStaticIndices->dataPointer()));*/
    
/*    if(mVBO[0] == 0)
    {
        glGenBuffers(1, &mVBO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
        glBufferData(GL_ARRAY_BUFFER,mVertexBuffer.size() *sizeof(float), &mVertexBuffer.front(), GL_STATIC_DRAW);
    }
    
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(0);

    if(mVBO[1] == 0)
        glGenBuffers(1, &mVBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, mNormalBuffer.size() *sizeof(float), &mNormalBuffer.front(), GL_STATIC_DRAW);
    
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(1);
    
    if(mVBO[2] == 0)    
        glGenBuffers(1, &mVBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[2]);
    glBufferData(GL_ARRAY_BUFFER, mUVBuffer.size() *sizeof(float), &mUVBuffer.front(), GL_STATIC_DRAW);
    
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(2);
    
    if(mVBO[3] == 0)    
        glGenBuffers(1, &mVBO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[3]);            
    glBufferData(GL_ARRAY_BUFFER, mTexArrayIndexBuffer.size()*sizeof(GLint), &mTexArrayIndexBuffer.front(), GL_STATIC_DRAW);
    
    glVertexAttribIPointer((GLuint)3, 2, GL_INT, 0, 0);
    
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    if(mVBO[4] == 0)
        glGenBuffers(1, &mVBO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[4]);            
    glBufferData(GL_ARRAY_BUFFER, mColorBuffer.size()*sizeof(GLfloat), &mColorBuffer.front(), GL_STATIC_DRAW);
    
    glVertexAttribPointer((GLuint)4, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    
    if(mVBO[5] == 0)    
        glGenBuffers(1, &mVBO[5]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer.size() *sizeof(GLuint), &mIndexBuffer.front(), GL_STATIC_DRAW);*/
    
    LOG("Mat buffer size: ", mMatBufferCache.size());
    LOG("Mat buffer capacity ", mMatBufferCache.size() * sizeof(glm::mat4));
    
    for(unsigned int index = 0; index < mMatBufferCache.size(); ++index)
        mUniformBuffers[GLUniBuffer::Model]->update(index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(mMatBufferCache[index]));
 
    unsigned int slices = static_cast<unsigned int>(floor(mMatBufferCache.size() / 1000));
    auto lastSliceSize = (mMatBufferCache.size() % 1000) * sizeof(glm::mat4);
        
    for(unsigned int slice = 0; slice < slices; ++slice)
        mUniformBuffers[GLUniBuffer::Model]->bindToIndexedBufferTarget(1 + slice, slice * 1000 * sizeof(glm::mat4), 1000 * sizeof(glm::mat4));

    //TODO: Resolver el anclaje de los buffers de uniformers
    mUniformBuffers[GLUniBuffer::Model]->bindToIndexedBufferTarget(1 , 0, 1000 * sizeof(glm::mat4));
    
    if (lastSliceSize) 
        mUniformBuffers[GLUniBuffer::Model]->bindToIndexedBufferTarget(1 + slices , slices * 1000 * sizeof(glm::mat4), lastSliceSize); 
}

void GLRenderer::initUniformData()
{      
    auto camera = mScene->camera();

    mUniformBuffers[GLUniBuffer::Frame]->updateCache(0, 64, glm::value_ptr(camera->projectionMatrix()));            
    mUniformBuffers[GLUniBuffer::Frame]->updateCache(64, 64, glm::value_ptr(camera->viewMatrix()));
    mScene->setProjCameraNeedUpdate(false);
    mScene->setViewCameraNeedUpdate(false);

    
    auto lights = mScene->lights();
    auto dirtyIndexes = mScene->dirtyLightsIndexes();
        
    for (auto index: dirtyIndexes)
    {
        glm::vec4 position = glm::vec4(lights[index]->position(), 1.0);
        glm::vec4 direction = glm::normalize(glm::vec4(-1.0, -1.0, -1.0, 0.0) - position);
        glm::vec4 color(1.0, 0.0, 0.0, 1.0);

        mUniformBuffers[GLUniBuffer::Frame]->updateCache(128 + index * 48, 16, glm::value_ptr(position));
        mUniformBuffers[GLUniBuffer::Frame]->updateCache(144 + index * 48, 16, glm::value_ptr(direction));
        mUniformBuffers[GLUniBuffer::Frame]->updateCache(160 + index * 48, 16, glm::value_ptr(color));
    }
        
    mScene->cleanDirtyLights();
    mScene->setLightsNeedUpdate(false);

    mUniformBuffers[GLUniBuffer::Frame]->updateGPU();
    
    mPaintingDataNeedUpdate = true;
    updateGenericUniformData(); //TODO: Eliminate. Not longer necessary
}

void GLRenderer::initAppUniformData()
{
    mUniformBuffers[GLUniBuffer::App] = std::make_unique<UniformBufferObject>(12 * sizeof(glm::uvec2), nullptr, sizeof(glm::uvec2));
    mUniformBuffers[GLUniBuffer::App]->updateCache(0, sizeof(glm::uvec2), glm::value_ptr(mDepthTarget->depthTexOutput()->textureArrayIndices()));
/*    mUniformBuffers[GLUniBuffer::App]->updateCache(0, sizeof(glm::uvec2), glm::value_ptr(mDepthTarget->colorTexOutputs()[0]->textureArrayIndices())); */   
    mUniformBuffers[GLUniBuffer::App]->updateCache(sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mSeamMaskTarget->colorTexOutputs()[0]->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateCache(2 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mBrushShapeTexture->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateCache(3 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mDepthTexTarget->depthTexOutput()->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateCache(4 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mBrushMaskTexture->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateCache(5 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mAreaPerPixelTexture->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateCache(6 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mLockTexture->textureArrayIndices()));
    mUniformBuffers[GLUniBuffer::App]->updateCache(7 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mNeighborhoodTexture->textureArrayIndices()));

    //mUniformBuffers[GLUniBuffer::App]->updateCache(2 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mPaintTexTarget->colorTexOutputs()[2]->textureArrayIndices()));    
//     mUniformBuffers[GLUniBuffer::App]->updateCache(4 * sizeof(glm::uvec2), sizeof(glm::uvec2), glm::value_ptr(mDilatedLayerTexture->textureArrayIndices()));    

    
    mUniformBuffers[GLUniBuffer::App]->updateGPU();    
    mUniformBuffers[GLUniBuffer::App]->bindToIndexedBufferTarget(10, 0, 12 * sizeof(glm::uvec2));
}


void GLRenderer::updateGenericUniformData()
{
    if(mManager->dirtyImageArrays().size())
    {
        for(const auto dirtyImage: mManager->dirtyImageArrays())
        {
            LOG("Render - Dirty image: ", dirtyImage->imageUnit(), ", id: ", dirtyImage->id());
            glBindImageTexture(dirtyImage->imageUnit(), dirtyImage->id(), 0, GL_TRUE, 0, GL_READ_WRITE, dirtyImage->internalFormat());            
        }        
        mManager->clearDirtyImageArrays();
    }    
    
    if(mManager->dirtyTextureArrays().size())
    {        
        for(const auto dirtyArray: mManager->dirtyTextureArrays())
        {
            LOG("1A Dirty Tex Array id: ", dirtyArray->id(), ", index: ", dirtyArray->index(), ", sampler: ", static_cast<int>(dirtyArray->samplerType()), ", textureUnit: ", dirtyArray->textureUnit());
            glBindTextureUnit(dirtyArray->textureUnit(), dirtyArray->id());
        }
        
        mManager->clearDirtyTextureArrays();
    }
        
    if(mManager->dirtySampler().size())
    {
        const auto& bindedTexUnits = mManager->bindedTexUnits();
                
        for(const auto& program: mManager->programs())
        {
            const auto& programShader = program.second;
            programShader->use();

            for(auto targetSampler: mManager->dirtySampler())
            {
                auto target = targetSampler.first;
                auto sampler = targetSampler.second;
                const auto& texUnits = bindedTexUnits.at(targetSampler);
                
                std::string unitstring;
                for(auto texUnit : texUnits)
                    unitstring.append(std::to_string(texUnit) + " ");
                
                LOG("1A Target: ", target, ", Sampler: ", static_cast<int>(targetSampler.second), ", TexUnits : ", unitstring);
                
                if(target == GL_TEXTURE_2D_ARRAY)
                {
                    if(sampler == SamplerType::Float && programShader->uniform("ArrayTexture[0]") >= 0)
                        glUniform1iv(programShader->uniform("ArrayTexture[0]"), texUnits.size(), texUnits.data());
                    else if(sampler == SamplerType::Signed && programShader->uniform("iArrayTexture[0]") >= 0)
                        glUniform1iv(programShader->uniform("iArrayTexture[0]"), texUnits.size(), texUnits.data());
                    else if(sampler == SamplerType::Unsigned && programShader->uniform("uArrayTexture[0]") >= 0)
                        glUniform1iv(programShader->uniform("uArrayTexture[0]"), texUnits.size(), texUnits.data());
                }
                else if(target == GL_TEXTURE_1D_ARRAY)
                {
                    if(sampler == SamplerType::Float && programShader->uniform("ArrayTexture1D[0]") >= 0)
                        glUniform1iv(programShader->uniform("ArrayTexture1D[0]"), texUnits.size(), texUnits.data());
                    else if(sampler == SamplerType::Signed && programShader->uniform("iArrayTexture1D[0]") >= 0)
                        glUniform1iv(programShader->uniform("iArrayTexture1D[0]"), texUnits.size(), texUnits.data());
                    else if(sampler == SamplerType::Unsigned && programShader->uniform("uArrayTexture1D[0]") >= 0)
                        glUniform1iv(programShader->uniform("uArrayTexture1D[0]"), texUnits.size(), texUnits.data());                    
                }
            }           
        }
        
        mManager->clearDirtySampler();
    }
    
    if(mPaintingDataNeedUpdate)
    {
        mUniformBuffers[GLUniBuffer::Material]->updateCache(sizeof(float) + sizeof(unsigned int), sizeof(unsigned int), &mLayerCount);
        mUniformBuffers[GLUniBuffer::Material]->updateCache(sizeof(float) + 2 * sizeof(unsigned int), sizeof(bool), &mVertexColorVisibility);
        mUniformBuffers[GLUniBuffer::Material]->updateCache(sizeof(glm::vec4), sizeof(glm::vec4), &mDefaultModelColor);
        mUniformBuffers[GLUniBuffer::Material]->updateCache(2 * sizeof(glm::vec4), sizeof(glm::vec3), glm::value_ptr(mSpecularColor));
        mUniformBuffers[GLUniBuffer::Material]->updateCache(2 * sizeof(glm::vec4) + sizeof(glm::vec3), sizeof(float), &mSpecularPower);

        if (mLayers.size() > mCurrentLayerIndex)
        {
            auto currentLayer = mLayers[mCurrentLayerIndex];
            
            if (currentLayer.mSamplerType == 0)
            {
                int value = static_cast<int>(mCurrentPaintingValue);
                mUniformBuffers[GLUniBuffer::Material]->updateCache(0, sizeof(int), &value);
            }
            else if (currentLayer.mSamplerType == 1)
            {
                unsigned int value = static_cast<unsigned int>(mCurrentPaintingValue);
                mUniformBuffers[GLUniBuffer::Material]->updateCache(0, sizeof(unsigned int), &value);
            }
            else
                mUniformBuffers[GLUniBuffer::Material]->updateCache(0, sizeof(float), &mCurrentPaintingValue);
            
            auto index = mLayers.size() - 1 - mCurrentLayerIndex;
            mUniformBuffers[GLUniBuffer::Material]->updateCache(sizeof(float), sizeof(unsigned int), &index);
            
            
            size_t offset = 3 * sizeof(glm::vec4);// + sizeof(float)  + 2 * sizeof(unsigned int);
            
            for (int i = mLayers.size() - 1; i > -1; --i)
            {
                auto value = mLayers[i].mValue;
                auto mask = mLayers[i].mMask;
                auto palette = mLayers[i].mPalette;                
                auto paletteRange = mLayers[i].mPaletteRange;
                auto opacity = mLayers[i].mOpacity;
                auto sType = mLayers[i].mSamplerType;
                
                glm::uvec4 layerIndices = glm::uvec4(value->textureArrayIndices(), mask->textureArrayIndices());
                glm::uvec2 paletteIndices = palette->textureArrayIndices();//  mCurrentPaletteTexture->textureArrayIndices();
                //glm::vec2 paletteRange(mCurrentPaletteRange.first, mCurrentPaletteRange.second);
                
                
                mUniformBuffers[GLUniBuffer::Material]->updateCache(offset, sizeof(glm::uvec4), glm::value_ptr(layerIndices));
                mUniformBuffers[GLUniBuffer::Material]->updateCache(offset + sizeof(glm::uvec4), sizeof(glm::uvec2), glm::value_ptr(paletteIndices));
                mUniformBuffers[GLUniBuffer::Material]->updateCache(offset + sizeof(glm::uvec4) + sizeof(glm::uvec2), sizeof(glm::vec2), glm::value_ptr(paletteRange));
                mUniformBuffers[GLUniBuffer::Material]->updateCache(offset + sizeof(glm::uvec4) + sizeof(glm::uvec2) + sizeof(glm::vec2), sizeof(float), &opacity);
                mUniformBuffers[GLUniBuffer::Material]->updateCache(offset + sizeof(glm::uvec4) + sizeof(glm::uvec2) + sizeof(glm::vec2) + sizeof(float), sizeof(unsigned int), &sType);
                offset += 3 * sizeof(glm::uvec4); //+ sizeof(glm::uvec2) + sizeof(glm::vec2) + sizeof(float) + sizeof(unsigned int);
            }
            
            //         glm::uvec4 layerIndices = glm::uvec4(mDilatedLayerValueTexture->textureArrayIndices(), mCurrentLayerMaskTexture->textureArrayIndices());
            //         glm::uvec2 paletteIndices = mCurrentPaletteTexture->textureArrayIndices();
            //         glm::vec2 paletteRange(mCurrentPaletteRange.first, mCurrentPaletteRange.second);
            // 
            //         mUniformBuffers[GLUniBuffer::Material]->updateCache(offset, sizeof(glm::uvec4), glm::value_ptr(layerIndices));
            //         mUniformBuffers[GLUniBuffer::Material]->updateCache(offset + sizeof(glm::uvec4), sizeof(glm::uvec2), glm::value_ptr(paletteIndices));
            //         mUniformBuffers[GLUniBuffer::Material]->updateCache(offset + sizeof(glm::uvec4) + sizeof(glm::uvec2), sizeof(glm::vec2), glm::value_ptr(paletteRange));
        }
        
        mPaintingDataNeedUpdate = false;
    }
    
    mUniformBuffers[GLUniBuffer::Material]->updateGPU();
}

void GLRenderer::updateSceneUniformData(Scene3D *scene, bool forceUpdate)
{    
    auto camera = scene->camera();
    LOG("Scene update: ", scene->name());
    if(scene->projCameraNeedUpdate() || forceUpdate)
    {
        mUniformBuffers[GLUniBuffer::Frame]->updateCache(0, sizeof(glm::mat4), glm::value_ptr(camera->projectionMatrix()));        
        scene->setProjCameraNeedUpdate(false);
        //LOG("UpdateSceneUniform::Scene: ", scene->name(), ", Projection Camera: ", glm::to_string(camera->projectionMatrix()));
    }
    
    if(scene->viewCameraNeedUpdate() || forceUpdate)
    {
        mUniformBuffers[GLUniBuffer::Frame]->updateCache(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->viewMatrix()));        
        scene->setViewCameraNeedUpdate(false);
        //LOG("UpdateSceneUniform::View Camera: ", glm::to_string(camera->viewMatrix()));
    }

    if (mMode == Mode::Mark || mMode == Mode::Erase)
    {
        auto markToolModel = mPPScene->findModel3D("BrushModel");        
        glm::vec4 brushTrans = markToolModel->modelMatrix()[3];
//         LOG("Mark Tool position: ", glm::to_string(brushTrans));
//         glm::vec3 temp = glm::unProject(glm::vec3(brushTrans.x, brushTrans.y, 0.0), mMainScene->camera()->viewMatrix(), mMainScene->camera()->projectionMatrix(), glm::vec4(0, 0, 1280.0, 720.0));
//         LOG("Mark Tool unproject: ", glm::to_string(mMainScene->camera()->viewMatrix() * glm::vec4(temp,1.0)));
        glm::vec2 MarkToolSize = glm::vec2(markToolModel->mesh()->dimensions());//markToolModel->mesh()->width(), markToolModel->mesh()->height());
        glm::vec3 scaleFactor = glm::vec3(mWindowWidth * 0.5f / MarkToolSize.x, mWindowHeight * 0.5f / MarkToolSize.y, 0.0f);
        glm::vec3 translationFactor = -glm::vec3((brushTrans.x - mWindowWidth * 0.5f) / MarkToolSize.x, (brushTrans.y - mWindowHeight * 0.5f) / MarkToolSize.y, 0.0f);
        
        glm::mat4 projScaleTrans = glm::translate(translationFactor) * glm::scale(scaleFactor);
        glm::mat4 proyectorPVCamera =  projScaleTrans * mCamera->projectionMatrix();

        mUniformBuffers[GLUniBuffer::Frame]->updateCache(2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proyectorPVCamera));
    }
        
    if(scene->lightsNeedUpdate() || forceUpdate)
    {
        auto lights = scene->lights();
        auto dirtyIndexes = scene->dirtyLightsIndexes();
        
        for (auto index: dirtyIndexes)
        {
            glm::vec4 position = glm::vec4(lights[index]->position(), 1.0);
            glm::vec4 direction = glm::normalize(glm::vec4(-1.0, -1.0, -1.0, 0.0) - position);
            glm::vec4 color(1.0, 0.0, 0.0, 1.0);

            mUniformBuffers[GLUniBuffer::Frame]->updateCache(32 * 4 + index * 12, 4 * 4, glm::value_ptr(position));
            mUniformBuffers[GLUniBuffer::Frame]->updateCache(36 * 4 + index * 12, 4 * 4, glm::value_ptr(direction));
            mUniformBuffers[GLUniBuffer::Frame]->updateCache(40 * 4 + index * 12, 4 * 4, glm::value_ptr(color));
        }
        
        scene->cleanDirtyLights();
        scene->setLightsNeedUpdate(false);
    }
    
    mUniformBuffers[GLUniBuffer::Frame]->updateGPU();
        
    if(scene->texturesNeedUpdate() || forceUpdate)
    {
        
    }
    
//     if(scene->modelsNeedUpdate() || forceUpdate)
//     {
//         auto models = scene->dirtyModels();
//         
//         for(auto model: models)
//         {
//             auto instanceIndex = scene->drawModelOffset(model->sceneIndex(scene));//scene->drawInstanceIndex(model->name());
//             
//             mUniformBuffers[GLUniBuffer::Model]->updateCache(sizeof(glm::mat4) * instanceIndex, sizeof(glm::mat4), glm::value_ptr(model->modelMatrix()));            
//         }
//         
//         scene->clearDirtyModels();
//         
//         mUniformBuffers[GLUniBuffer::Model]->updateGPU();
//     }
            
//     auto viewMatrixData = glm::value_ptr(camera->viewMatrix());
//     auto projMatrixData = glm::value_ptr(camera->projectionMatrix());
//     
//     LOG("Projection: ", glm::to_string(camera->projectionMatrix()));
//     LOG("View: ", glm::to_string(camera->viewMatrix()));
//     
//     for(int i = 0; i < 16; i+=4)
//         LOG("Detail: ", projMatrixData[i], " ", projMatrixData[i+1], " ", projMatrixData[i+2], " ", projMatrixData[i+3]);
//     
//     perFrameData.insert(end(perFrameData), projMatrixData, projMatrixData + 16);
//     perFrameData.insert(end(perFrameData), viewMatrixData, viewMatrixData + 16);
//   
//     
//     mPerFrameUFO->update(0, 2 * sizeof(glm::mat4), perFrameData.data());
}

void GLRenderer::updateMaterialUniformData()
{

}


void GLRenderer::updateTransientUniformData(RenderTarget* target)
{
    const auto& colorInputs = target->colorInputIndices();
    const auto& depthInputs = target->depthInputIndices();
    
    if(colorInputs.size() > 0)
        mUniformBuffers[GLUniBuffer::Transient]->updateCache(0, colorInputs.size() * sizeof(GLuint) * 2, colorInputs.data());
    
    if(depthInputs.size() > 0)
        mUniformBuffers[GLUniBuffer::Transient]->updateCache(colorInputs.size() * sizeof(GLuint) * 2, depthInputs.size() * sizeof(GLuint), depthInputs.data());
    
    mUniformBuffers[GLUniBuffer::Transient]->updateGPU();
}

void GLRenderer::updateSlicePlaneUniformData()
{
    auto planeMatrix = mSlicePlaneScene->models()[2]->modelMatrix();
    mPlanePoint = planeMatrix * glm::vec4(glm::vec3(0, 0, 0), 1.0);
    mPlaneNormal = glm::vec4(glm::mat3(glm::inverse(glm::transpose(mCamera->viewMatrix() * planeMatrix))) * glm::vec3(0, 0, -1), 1.0);
    
//     LOG("updateSlicePlaneUniformData Slice Matrix: ", glm::to_string(planeMatrix));
//     LOG("updateSlicePlaneUniformData Slice Normal: ", glm::to_string(mPlaneNormal));
//     LOG("updateSlicePlaneUniformData Slice Point: ", glm::to_string(mPlanePoint));
    
    mUniformBuffers[GLUniBuffer::App]->updateCache(8 * sizeof(glm::uvec2), sizeof(glm::vec4), glm::value_ptr(mPlaneNormal));    
    mUniformBuffers[GLUniBuffer::App]->updateCache(10 * sizeof(glm::uvec2), sizeof(glm::vec4), glm::value_ptr(mPlanePoint));
    
    mUniformBuffers[GLUniBuffer::App]->updateGPU();
}

void GLRenderer::updateMeshData(Scene3D* scene)
{
    if(scene->meshesNeedUpdate())
    {
        std::unordered_set <unsigned int> updatedBuffers;
        for (const auto& mesh : scene->dirtyMeshes())
        {
            //TODO: RGBA por defecto en renderTarget, glclearColor del inicio por defecto, indexBufferCache
            auto meshOffset = scene->drawMeshOffset(mesh->name());
            std::vector<GLuint> indexBufferCache;
            for(const auto& buffer: mesh->updatedBuffers())
            {
                auto updateSize = mesh->bufferUpdateSize(buffer);
                auto updateOffset = mesh->bufferUpdateOffset(buffer);
                auto dataPointer = mesh->buffer(buffer).data() + updateOffset;
                
                int offset = meshOffset.first * mBuffers[buffer]->componentCount() * mBuffers[buffer]->componentSize();
                
                if (buffer == GLBuffer::Index)
                {
                    indexBufferCache.clear();

                    auto cibSize = mesh->buffer(GLBuffer::Index).size()/sizeof(GLuint);
                    auto constcib = reinterpret_cast<const GLuint *>(mesh->buffer(GLBuffer::Index).data());
                    for(int i = 0; i < cibSize; ++i)
                        indexBufferCache.push_back(constcib[i] + meshOffset.first);
                    dataPointer = reinterpret_cast<glm::byte *>(indexBufferCache.data()) + updateOffset;
                    offset = meshOffset.second * mBuffers[buffer]->componentCount() * mBuffers[buffer]->componentSize();
                }
                
                mBuffers[buffer]->updateCache(offset, updateSize, dataPointer);
                updatedBuffers.insert(buffer);
            }
            
            mesh->clearUpdatedBuffers();
        }
        
        for(auto buffer: updatedBuffers)
            mBuffers[buffer]->updateGPU();
            
        scene->clearDirtyMeshes();
    }
}

void GLRenderer::updateMultiDrawCommand(Scene3D* scene)
{
    LOG("Renderer: Updating MultiDrawCommmand for Scene ", scene->name() );
  
    auto meshInstances = scene->modelsByGeometry(); 
    auto modelsPerMaterialPerMesh = scene->modelsPerMaterialPerMesh();
    auto modelsPerMeshPerMaterial = scene->modelsPerMeshPerMaterial();
    auto materialsAffectedByDirtyModels = scene->materialsAffectedByDirtyModels();
    auto meshes = scene->meshes();
      
    auto drawIndirectCommands = scene->drawCommands();
    auto drawIndirectCommandsPerMaterial = scene->materialDrawCommands2();
    auto sceneCommandCountPerMaterial = scene->drawCommandsPerMaterialCount();
    
    std::map<int, std::set<int>> dirtyMaterialDataPerMesh;
    
    std::string meshNames;
    for(auto mesh: meshes)
        meshNames += mesh->name() + " ";
//     LOG("Meshes: ", meshNames);
    
    std::string modelNames;
    for(auto model: scene->models())
        modelNames += model->name() + " ";
//     LOG("Models: ", modelNames);
    
    meshNames.clear();
    for(auto mesh: scene->dirtyMeshes2())
        meshNames += mesh->name() + " ";
//     LOG("Dirty Meshes: ", meshNames);
    
    modelNames.clear();
    for(auto model: scene->dirtyModels2())
    {
        auto idx = model->sceneIndex(scene);
        modelNames += model->name() + "-<" + std::to_string(idx[0]) + ", " + std::to_string(idx[1]) + ", " + std::to_string(idx[2]) + "> ";
    }
/*    LOG("Models: ", modelNames);*/        

    
    if(scene->meshCountChanged())
    {
        std::size_t drawIndirectCommandCount = 0;
        
        for(auto mesh: meshes)
            drawIndirectCommandCount += mesh->subMeshes().size();
     
        drawIndirectCommands.resize(drawIndirectCommandCount);
        scene->setMeshCountChanged(false);
    }
    
    if(scene->modelCountChanged())
    {
        if(scene->materials().size() > drawIndirectCommandsPerMaterial.size())
            drawIndirectCommandsPerMaterial.resize(scene->materials().size());
        
        for(auto materialIndex : materialsAffectedByDirtyModels)
        {
            std::size_t drawIndirectCommandCountPerMaterial = 0;
            
            for(auto modelsPerCurrentMatAndMesh: modelsPerMaterialPerMesh[materialIndex])
            {
                dirtyMaterialDataPerMesh[modelsPerCurrentMatAndMesh.first].insert(materialIndex);
                drawIndirectCommandCountPerMaterial += meshes[modelsPerCurrentMatAndMesh.first]->subMeshes().size();
            }
                    
            drawIndirectCommandsPerMaterial[materialIndex].resize(drawIndirectCommandCountPerMaterial);
        }
        
//         for(auto modelsPerCurrentMat : modelsPerMaterialPerMesh)
//         {        
//             std::size_t drawIndirectCommandCountPerMaterial = 0;
//             
//             for(auto modelsPerCurrentMatAndMesh: modelsPerCurrentMat.second)
//                 drawIndirectCommandCountPerMaterial += meshes[modelsPerCurrentMatAndMesh.first]->subMeshes().size();
//                     
//             drawIndirectCommandsPerMaterial[modelsPerCurrentMat.first].resize(drawIndirectCommandCountPerMaterial);
//         }
        
        scene->setModelCountChanged(false);
    }
    
    bool updateCommandPerMaterial = false;    
    
    if(scene->meshesNeedUpdate())
    {
        std::unordered_set <unsigned int> updatedBuffers;
        for (const auto& mesh : scene->dirtyMeshes2())
        {   
            auto csm = mesh->subMeshes();
            auto meshIndex = mesh->sceneIndex(scene);
            auto offsets = scene->drawMeshOffset(meshIndex);
            bool updateCommand = false;
            
            if(offsets.attrib == std::numeric_limits<unsigned int>::max())
            {
                offsets.attrib = mVertexCount;
                mVertexCount += mesh->vertexCount();
                updateCommand = true;
            }
            
            if(offsets.index == std::numeric_limits<unsigned int>::max())
            {
                offsets.index = mIndexCount;
                mIndexCount += mesh->elementCount();
                updateCommand = true;
            }
            
            if(offsets.command == std::numeric_limits<unsigned int>::max())                        
            {
                if(meshIndex > 0)
                    offsets.command = scene->drawMeshOffset(meshIndex - 1).command + static_cast<int>(csm.size());
                else
                    offsets.command = 0;
            }
            
            if(offsets.instance == std::numeric_limits<unsigned int>::max())
            {
                offsets.instance = mInstanceOffset;
                updateCommand = true;
                updateCommandPerMaterial = true;
            }
            
            for(auto materialIndex: dirtyMaterialDataPerMesh[meshIndex])
            {
                if(offsets.commandPerMaterial[materialIndex] == std::numeric_limits<unsigned int>::max())
                {
                    offsets.commandPerMaterial[materialIndex] = sceneCommandCountPerMaterial[materialIndex];
                    sceneCommandCountPerMaterial[materialIndex] += csm.size();                        
                }
            }
            
            if(updateCommand)
            {
                resizeGLBuffers(mVertexCount, mIndexCount);
//                 LOG("Vertex: ", mVertexCount, ", Index: ", mIndexCount);
            }
            
            scene->setDrawMeshOffset(meshIndex, offsets);
            scene->setDrawCommandCountPerMaterial(sceneCommandCountPerMaterial);
            
            std::vector<GLuint> indexBufferCache;
            
            for(const auto& buffer: mesh->updatedBuffers())
            {                    
                auto updateSize = mesh->bufferUpdateSize(buffer);
                auto updateOffset = mesh->bufferUpdateOffset(buffer);
                auto dataPointer = mesh->buffer(buffer).data() + updateOffset;
                
                GLintptr dataOffset = offsets.attrib * mBuffers[buffer]->componentCount() * mBuffers[buffer]->componentSize();
                
                if (buffer == GLBuffer::Index)
                {
                    auto indexBuffer = (!mesh->indexBuffer2Used()) ? GLBuffer::Index : GLBuffer::Index2;
                    indexBufferCache.clear();
                    auto cibSize = mesh->buffer(indexBuffer).size()/sizeof(GLuint);
                    auto constcib = reinterpret_cast<const GLuint *>(mesh->buffer(indexBuffer).data());
                    
                    for(int i = 0; i < cibSize; ++i)
                        indexBufferCache.push_back(constcib[i] + offsets.attrib);
                    
                    updateSize = mesh->bufferUpdateSize(indexBuffer);
                    updateOffset = mesh->bufferUpdateOffset(indexBuffer);
                    dataPointer = reinterpret_cast<glm::byte *>(indexBufferCache.data()) + updateOffset;
                    dataOffset = offsets.index * mBuffers[buffer]->componentCount() * mBuffers[buffer]->componentSize();
                }
                
//                 LOG("Mesh ", mesh->name(), ", Update buffer ", buffer, ", dataOffset: ", dataOffset, ", updateSize: ", updateSize);
                
                mBuffers[buffer]->updateCache(dataOffset, updateSize, dataPointer);
                updatedBuffers.insert(buffer);
            }
            
            for(unsigned int i = 0; i < csm.size(); i++)
            {
                drawIndirectCommands[offsets.command].mCount = csm[i];
                drawIndirectCommands[offsets.command].mInstanceCount = meshInstances[mesh->name()].size();
                drawIndirectCommands[offsets.command].mFirstIndex = offsets.index;
                drawIndirectCommands[offsets.command].mBaseVertex = 0;
                drawIndirectCommands[offsets.command].mBaseInstance = offsets.instance;
                
//                 LOG("Mesh: ", mesh->name(), " subMesh: ", i);
//                 LOG("- Count: ", drawIndirectCommands[offsets.command].mCount);
//                 LOG("- InstanceCount: ", drawIndirectCommands[offsets.command].mInstanceCount);
//                 LOG("- FirstIndex: ", drawIndirectCommands[offsets.command].mFirstIndex);
//                 LOG("- BaseVertex: ", drawIndirectCommands[offsets.command].mBaseVertex);
//                 LOG("- BaseInstance: ", drawIndirectCommands[offsets.command].mBaseInstance);
                
                for(auto material: modelsPerMeshPerMaterial[meshIndex])
                {
                    auto materialIndex = material.first;                        
                    auto commandIndex = offsets.commandPerMaterial[materialIndex];
                    
                    drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mCount = csm[i];
                    drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mInstanceCount = material.second.size();
                    drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mFirstIndex = offsets.index;
                    drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mBaseVertex = 0;
                    drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mBaseInstance = offsets.instance;
                    
/*                    LOG("Mesh: ", mesh->name(), " subMesh: ", i, " material: ", materialIndex);
                    LOG("- Count: ", drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mCount);
                    LOG("- InstanceCount: ", drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mInstanceCount);
                    LOG("- FirstIndex: ", drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mFirstIndex);
                    LOG("- BaseVertex: ", drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mBaseVertex);
                    LOG("- BaseInstance: ", drawIndirectCommandsPerMaterial[materialIndex][commandIndex].mBaseInstance);   */                     
                    
                    offsets.instance += material.second.size();
                    offsets.commandPerMaterial[materialIndex]++;
                }
                
                offsets.index += csm[i];
                offsets.command++;                    
            }
            
            mInstanceOffset = offsets.instance;
            
            mesh->clearUpdatedBuffers();
        }
        
        for(auto buffer: updatedBuffers)
            mBuffers[buffer]->updateGPU();
        
        scene->clearDirtyMeshes();            
    }
    
    if(updateCommandPerMaterial)
    {
        resizeInstanceBuffers(mInstanceOffset);
//         LOG("Instance: ", mInstanceOffset);
    }
    
    for(auto model : scene->dirtyModels2())                
    {
        auto index = model->sceneIndex(scene);
        auto instanceOffset = scene->drawModelOffset(index);
        
//         LOG("Dirty model: ", model->name(), ", index[ ", index[0], ", ", index[1], ", ", index[2], "], instance offset: ", instanceOffset);
        
        if(instanceOffset == std::numeric_limits<unsigned int>::max())
        {
            if(index[2] > 0)
            {                
                auto previous = modelsPerMeshPerMaterial[index[0]][index[1]][index[2] - 1];
                instanceOffset = scene->drawModelOffset(previous->sceneIndex(scene)) + previous->mesh()->subMeshes().size();
            }
            else if(index[1] > 0)
            {
                auto prevIter = std::prev(modelsPerMeshPerMaterial[index[0]].find(index[1]), 1);
                auto previous = (*prevIter).second[(*prevIter).second.size() - 1];
                instanceOffset = scene->drawModelOffset(previous->sceneIndex(scene)) + previous->mesh()->subMeshes().size();
            }
            else
                instanceOffset = scene->drawMeshOffset(model->mesh()->sceneIndex(scene)).instance;                    

            scene->setDrawModelOffset(index, instanceOffset);              
        }

        auto csm = model->mesh()->subMeshes();
        
        for(unsigned int i = 0; i < csm.size(); i++)                    
        {            
            GLsizeiptr updateSize = mUniformBuffers[GLUniBuffer::Model]->componentSize();
            GLintptr updateOffset = instanceOffset * updateSize + (i * updateSize);            
            mUniformBuffers[GLUniBuffer::Model]->updateCache(updateOffset, updateSize, glm::value_ptr(model->modelMatrix()));
//             LOG("UniBuff Model updateOffset:  ", updateOffset, ", updateSize: ", updateSize);
//             LOG("Model matrix: ", glm::to_string(model->modelMatrix()));
                        
            auto diffuseTexture = model->material(i)->diffuseTexture();
            auto texArrayIndices = (diffuseTexture != nullptr) ? diffuseTexture->textureArrayIndices() : glm::uvec2{0, 0};
            updateSize = mBuffers[GLBuffer::TAIndex]->componentCount() * mBuffers[GLBuffer::TAIndex]->componentSize();
            updateOffset = instanceOffset * updateSize + (i * updateSize);            
            mBuffers[GLBuffer::TAIndex]->updateCache(updateOffset, updateSize, glm::value_ptr(texArrayIndices));                        
        }
    }
    
    mUniformBuffers[GLUniBuffer::Model]->updateGPU();
    mBuffers[GLBuffer::TAIndex]->updateGPU();
    
    scene->clearDirtyModels();
    scene->clearMaterialsAffectedByDirtyModels();
    
    scene->setDrawCommands(drawIndirectCommands);
    scene->setDrawCommandsPerMaterial(drawIndirectCommandsPerMaterial);     
}

void GLRenderer::processComputeJobs()
{
    if(mManager->dirtyImageArrays().size())
    {
        for(const auto dirtyImage: mManager->dirtyImageArrays())
        {
            LOG("Process Compute Jobs - Dirty image: ", dirtyImage->imageUnit(), ", id: ", dirtyImage->id());
            glBindImageTexture(dirtyImage->imageUnit(), dirtyImage->id(), 0, GL_TRUE, 0, GL_READ_WRITE, dirtyImage->internalFormat());            
        }        
        mManager->clearDirtyImageArrays();
    }
    
    for(const auto& job: mComputeQueue)
    {
        job->shader()->use();
        
        const auto& groupCount = job->groupCount();
        const auto& groupSize = job->groupSize();
        const auto& data = job->data();
        
        //mUniformBuffers[GLUniBuffer::Compute]->updateCache(0, indices.size() * sizeof(GLint), indices.data());
        if(data.size())
        {
            mUniformBuffers[GLUniBuffer::Compute]->updateCache(0, data.size(), data.data());
            mUniformBuffers[GLUniBuffer::Compute]->updateGPU();
        }                
        
        glDispatchComputeGroupSizeARB(groupCount[0], groupCount[1], groupCount[2], 
                                      groupSize[0], groupSize[1], groupSize[2]);
        
        glMemoryBarrier(job->memoryBarriers());
        
        glGenerateTextureMipmap(mLayers[0].mValue->textureArrayId());
        glGenerateTextureMipmap(mLayers[0].mMask->textureArrayId());        
        
        mManager->deleteShader(job->shader()->name());
    }
    
    if(mComputeQueue.size() > 0)
        mComputeQueue.clear();
    
    mManager->freeAllImageUnits();
    
    mChisel->unloadTempLayers();
}


void GLRenderer::generateMultiDrawCommand(Scene3D* scene)
{
    LOG("Renderer Generating MultiDrawCommmand 3 for Scene ", scene->name() );
  
    auto instancesByMesh = scene->modelsByGeometry(); 
    auto instancesMeshMaterial = scene->modelsByGeometry2();
    auto instancesMaterialMesh = scene->modelsByMaterial2();    
    auto meshes = scene->meshes();
    

    size_t drawIndirectCommandSize = 0; 
    
    for(auto mesh: meshes)
        for(auto submesh: mesh->subMeshes())
            drawIndirectCommandSize++;
    
    std::vector<DrawElementsIndirectCommand> drawIndirectCommands(drawIndirectCommandSize);
    std::map<std::string, std::vector<DrawElementsIndirectCommand>> drawIndirectCommands2;
        
    for(auto materialInstance : instancesMaterialMesh)
    {        
        drawIndirectCommandSize = 0; 
    
        for(auto meshes: materialInstance.second)
        {
            auto mesh = scene->findMesh(meshes.first);
            
            for(auto submesh: mesh->subMeshes())
                drawIndirectCommandSize++;
        }
        
        drawIndirectCommands2[materialInstance.first].resize(drawIndirectCommandSize);
    }

    /*vertexBufferSize = mVertexBufferCache.size();
    normalBufferSize = mNormalBufferCache.size();
    uvBufferSize = mUVBufferCache.size();
    indexBufferSize = mIndexBufferCache.size();
    matBufferSize = mMatBufferCache.size();
    texArrayIndexBufferSize = mTexArrayIndexBufferCache.size();*/
    
    unsigned int drawIndCommandIndex = 0;
    std::map<std::string, unsigned int> drawIndCommandIndex2;
           
    for(auto index = 0; index < meshes.size(); ++index)
    {   
        Mesh* mesh = meshes[index];

        auto csm = mesh->subMeshes();        
        auto cvb = mesh->buffer(GLBuffer::Vertex);
        auto ccb = mesh->buffer(GLBuffer::Color);
        auto cnb = mesh->buffer(GLBuffer::Normal);
        auto cuvb = mesh->buffer(GLBuffer::UV);        
        auto cibSize = mesh->buffer(GLBuffer::Index).size()/sizeof(GLuint);
        auto constcib = reinterpret_cast<const GLuint *>(mesh->buffer(GLBuffer::Index).data());
        std::map<int, unsigned int> drawCommandIndPerMaterial;
        unsigned int tempInstanceOffset = mInstanceOffset;
        unsigned int tempVertexCount = mVertexCount;
        unsigned int tempIndexCount = mIndexCount;
        unsigned int tempDrawIndCommand = drawIndCommandIndex;
                
        scene->setDrawMeshOffset(mesh->name(), std::pair<int, int>(mVertexCount, mIndexCount));
        //scene->setDrawMeshOffset(index, {mVertexCount, mIndexCount, mInstanceOffset, drawIndCommandIndex});
        //std::for_each(begin(cib), end(cib), [&](unsigned int &idx){ idx += mIndicesAdded; });
              
        mVertexBufferCache.insert(mVertexBufferCache.end(), cvb.begin(), cvb.end());
        mColorBufferCache.insert(mColorBufferCache.end(), ccb.begin(), ccb.end());
        mNormalBufferCache.insert(mNormalBufferCache.end(), cnb.begin(), cnb.end());
        
        if(cuvb.size())
            mUVBufferCache.insert(mUVBufferCache.end(), cuvb.begin(), cuvb.end());
        else
            mUVBufferCache.insert(mUVBufferCache.end(), 2 * cibSize, 0);
        
        for(int i = 0; i < cibSize; ++i)
            mIndexBufferCache.push_back(constcib[i] + mVertexCount);

        //mIndexBufferCache.insert(mIndexBufferCache.end(), cib.begin(), cib.end());
 
        
        for(unsigned int i = 0; i < csm.size(); i++ )
        {
            drawIndirectCommands[drawIndCommandIndex].mCount = csm[i];
            drawIndirectCommands[drawIndCommandIndex].mInstanceCount = instancesByMesh[mesh->name()].size();
            drawIndirectCommands[drawIndCommandIndex].mFirstIndex = mIndexCount;
            drawIndirectCommands[drawIndCommandIndex].mBaseVertex = 0;
            drawIndirectCommands[drawIndCommandIndex].mBaseInstance = mInstanceOffset;

            LOG("Mesh: ", mesh->name(), " subMesh: ", i);
            LOG("- Count: ", drawIndirectCommands[drawIndCommandIndex].mCount);
            LOG("- InstanceCount: ", drawIndirectCommands[drawIndCommandIndex].mInstanceCount);
            LOG("- FirstIndex: ", drawIndirectCommands[drawIndCommandIndex].mFirstIndex);
            LOG("- BaseVertex: ", drawIndirectCommands[drawIndCommandIndex].mBaseVertex);
            LOG("- BaseInstance: ", drawIndirectCommands[drawIndCommandIndex].mBaseInstance);
            
            for(auto material: instancesMeshMaterial[mesh->name()])
            {
                std::string matName = material.first;
                unsigned int  commandIndex = drawIndCommandIndex2[matName];
                
                drawCommandIndPerMaterial[scene->findMaterial(matName)->sceneIndex(scene)] = commandIndex;
                
                drawIndirectCommands2[matName][commandIndex].mCount = csm[i];
                drawIndirectCommands2[matName][commandIndex].mInstanceCount = material.second.size();
                drawIndirectCommands2[matName][commandIndex].mFirstIndex = mIndexCount;
                drawIndirectCommands2[matName][commandIndex].mBaseVertex = 0;
                drawIndirectCommands2[matName][commandIndex].mBaseInstance = mInstanceOffset;
        
                LOG("Mesh: ", mesh->name(), " subMesh: ", i, " material: ", matName);
                LOG("- Count: ", drawIndirectCommands2[matName][commandIndex].mCount);
                LOG("- InstanceCount: ", drawIndirectCommands2[matName][commandIndex].mInstanceCount);
                LOG("- FirstIndex: ", drawIndirectCommands2[matName][commandIndex].mFirstIndex);
                LOG("- BaseVertex: ", drawIndirectCommands2[matName][commandIndex].mBaseVertex);
                LOG("- BaseInstance: ", drawIndirectCommands2[matName][commandIndex].mBaseInstance);
                
                //mInstanceOffset += material.second.size(); 
                drawIndCommandIndex2[matName]++;
                
                for(auto instance: material.second)
                {
                    scene->setDrawInstanceIndex(instance->name(), mInstanceOffset);
                    //scene->setDrawModelOffset(instance->sceneIndex(scene), mInstanceOffset);
                                        
                    mMatBufferCache.push_back(instance->modelMatrix());
                                    
                    if(instance->material(i)->diffuseTexture() != nullptr)
                    {
                        unsigned int texArrayIndex = instance->material(i)->diffuseTexture()->textureArrayIndex();
                        unsigned int texArrayLayerIndex = instance->material(i)->diffuseTexture()->textureArrayLayerIndex();

                        LOG("- Texture: Array ", texArrayIndex, ", Layer ", texArrayLayerIndex);

                        mTexArrayIndexBufferCache.push_back(texArrayIndex);
                        mTexArrayIndexBufferCache.push_back(texArrayLayerIndex);
                    }
                    else
                    {
                        mTexArrayIndexBufferCache.push_back(0);
                        mTexArrayIndexBufferCache.push_back(0);                
                    }
//                     glm::vec4 color = instance->material(i)->diffuseChannel().mColor;
//                     mColorBufferCache.push_back(color.r);
//                     mColorBufferCache.push_back(color.g);
//                     mColorBufferCache.push_back(color.b);
//                     mColorBufferCache.push_back(color.a);
                    
                    mInstanceOffset++;
                }
                
            }
            
            drawIndCommandIndex++;                                   
            mIndexCount += csm[i];
            mVertexCount += cvb.size()/(3 * sizeof(GLfloat));                        
        }
        
        scene->setDrawMeshOffset(index, {tempVertexCount, tempIndexCount, tempInstanceOffset, tempDrawIndCommand, drawCommandIndPerMaterial});
    }
    
    scene->setDrawCommands(drawIndirectCommands);
    scene->setDrawCommandsPerMaterial(drawIndirectCommands2);    
}
