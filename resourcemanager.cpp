#include "resourcemanager.h"
#include "chisel.h"
#include "renderer.h"
#include "scene3d.h"
#include "renderpass.h"
#include "rendertarget.h"
#include "rendertechnique.h"
#include "GLSLFileLoader.h"
#include "fbxloader.h"
#include "fbxexporter.h"
#include "plyloader.h"
#include "objloader.h"
#include "registerlayer.h"
#include "sqlitedatabasemanager.h"
#include "databasetable.h"
#include "glutils.h"
#include "cerealglm.h"
#include "logger.hpp"
#include "disklayermodel.h"

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/memory.hpp>

#include <QImage>
#include <QDir>
#include <QCoreApplication>
#include <QGLWidget>

#include <regex>
#include <iostream>
#include <array>


ResourceManager::ResourceManager(Chisel* chisel, Renderer* renderer)
:
    mChisel(chisel),
    mRenderer(renderer),
    mAppPath(QCoreApplication::applicationDirPath().toStdString() + "/")
{
    if(mRenderer != nullptr)
        mRenderer->setResourceManager(this);

    // Temp
    
    mSQLiteDatabaseManager = std::make_unique<SQLiteDatabaseManager>("model.db");
    
    if(!directoryExists("palettes/"))
        createDirectory("palettes/");
    
    if(!isDirectoryEmpty("palettes/"))
        loadPalettes("palettes/");
    else
    {
        createPalette("Paleta", Palette::Type::Discrete, std::map<double, glm::vec4>({{0.0, glm::vec4(1.0, 0.0, 0.0, 1.0)},{10.0, glm::vec4(1.0, 0.0, 1.0, 1.0)}}));
        createPalette("Otra", Palette::Type::Discrete, std::map<double, glm::vec4>({{0.0, glm::vec4(1.0, 0.647, 0.0, 1.0)},{10.0, glm::vec4(0.0, 0.0, 1.0, 1.0)}}));
        createPalette("Nueva", Palette::Type::Discrete, std::map<double, glm::vec4>({{0.0, glm::vec4(0.0, 1.0, 0.5, 1.0)},{100.0, glm::vec4(0.5, 0.0, 1.0, 1.0)}}));
//         mPalettes.push_back(std::make_unique<Palette>(0, "Paleta", "", Palette::Type::Discrete, std::map<double, glm::vec4>({{0.0, glm::vec4(1.0, 0.0, 0.0, 1.0)},{10.0, glm::vec4(1.0, 0.0, 1.0, 1.0)}})));
//         mPalettes.push_back(std::make_unique<Palette>(1, "Otra", "", Palette::Type::Discrete, std::map<double, glm::vec4>({{0.0, glm::vec4(1.0, 0.647, 0.0, 1.0)},{10.0, glm::vec4(0.0, 0.0, 1.0, 1.0)}})));
//         mPalettes.push_back(std::make_unique<Palette>(2, "Nueva", "", Palette::Type::Discrete, std::map<double, glm::vec4>({{0.0, glm::vec4(0.0, 1.0, 0.5, 1.0)},{100.0, glm::vec4(0.5, 0.0, 1.0, 1.0)}})));
        
        for(auto i = 0; i < mPalettes.size(); ++i)
            savePalette(i, "", "palettes/");
    }
    
    mLocalPaletteIndex = static_cast<unsigned int>(mPalettes.size());
}

ResourceManager::~ResourceManager()
{

}

// *** Public Methods *** //

Texture * ResourceManager::texture(std::string texture) const
{
    auto found = mTextures.find(texture);
    
    return (found != end(mTextures)) ? found->second.get() : nullptr;
}

Program* ResourceManager::shaderProgram(std::string program) const
{
    if(mPrograms.find(program) != mPrograms.end())
        return mPrograms.at(program).get();
    else
        return nullptr;
}

Material* ResourceManager::material(std::string material) const
{
    if(mMaterials.find(material) != mMaterials.end())
        return mMaterials.at(material).get();
    else
        return nullptr;
}

Mesh* ResourceManager::mesh(std::string mesh) const
{
    if(mMeshes.find(mesh) != mMeshes.end())
        return mMeshes.at(mesh).get();
    else
        return nullptr;
}

Scene3D * ResourceManager::scene(std::string scene) const
{
    auto found = mScenes.find(scene);

    return (found != end(mScenes)) ? found->second.get() : nullptr;
}

Camera* ResourceManager::camera(std::string camera) const
{
    auto found = mCameras.find(camera);
    
    return (found != end(mCameras)) ? found->second.get() : nullptr;
}

bool ResourceManager::isValidName(const std::string candidate) const
{
    std::string forbiddenNames = "CON|PRN|AUX|NUL|COM|COM1|COM2|COM3|COM4|COM5|COM6|COM7|COM8|COM9|LPT|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9";
    std::regex validPattern("^(?!(" + forbiddenNames + ")$)[a-zA-Z][a-zA-Z0-9_]*$", std::regex::ECMAScript);
    
    return std::regex_match(candidate, validPattern);    
}

std::string ResourceManager::createValidName(const std::string candidate, std::vector<std::string> existingNames) const
{
    std::string validName = isValidName(candidate) ? candidate : "tempName";    
        
    bool valid = false;
    std::string baseValidName = validName;

    for(unsigned int i = 1; i < std::numeric_limits<unsigned int>::max() && !valid; ++i)
    {           
        auto found = std::find_if(std::begin(existingNames), std::end(existingNames), [&](const std::string& filename) { return (filename == validName) ? true : false; });
        
        if(found == end(existingNames))
            valid = true;
        else
            validName = baseValidName + "_" + std::to_string(i);
    }
    
    return validName;    
}

std::string ResourceManager::createValidLayerName(const std::string name) const
{
    return createValidName(name, existingLayerNames());
}

std::string ResourceManager::createValidPaletteName(const std::string name) const
{
    return createValidName(name, existingPaletteNames());
}

std::vector<std::string> ResourceManager::existingLayerNames() const
{
    auto diskLayers = mChisel->diskLayerModel()->filenames();
    std::vector<std::string> activeLayers;
    std::vector<std::string> existingLayers;
    
    for(const auto& layer: mLayers)
        activeLayers.push_back(layer->name());

    std::sort(begin(diskLayers), end(diskLayers));
    std::sort(begin(activeLayers), end(activeLayers));
    
    std::set_union(begin(activeLayers), end(activeLayers), begin(diskLayers), end(diskLayers), std::inserter(existingLayers, begin(existingLayers)));
    
    return existingLayers;
}

std::vector<std::string> ResourceManager::existingPaletteNames() const
{
    std::vector<std::string> paletteNames;
    
    for(const auto& palette: mPalettes)
        paletteNames.push_back(palette->name());

    return paletteNames;
}

bool ResourceManager::layerExists(const std::string layerName) const
{
    auto layerNames = existingLayerNames();
    
    auto found = std::find_if(std::begin(layerNames), std::end(layerNames), [&](const std::string& currentLayerName) { return (currentLayerName == layerName) ? true : false; });
    
    return (found != end(layerNames)) ? true : false;
}

bool ResourceManager::paletteExists(const std::string paletteName) const
{
    auto paletteNames = existingPaletteNames();
    
    auto found = std::find_if(std::begin(paletteNames), std::end(paletteNames), [&](const std::string& currentPaletteName) { return (currentPaletteName == paletteName) ? true : false; });
    
    return (found != end(paletteNames)) ? true : false;
}

Layer* ResourceManager::layer(std::string layer) const
{
    auto search = std::find_if(begin(mLayers), end(mLayers), [&](const std::unique_ptr<Layer>& currentLayer){ return (layer == currentLayer->name()) ? true : false;});
    
    return (search != end(mLayers)) ? search->get() : nullptr;
}

std::vector<Layer*> ResourceManager::layers() const
{
    std::vector<Layer*> layers;
    
    for(const auto& layer : mLayers)
        layers.push_back(layer.get());
    
    return layers;
}

Palette* ResourceManager::palette(std::string palette) const
{
    auto search = std::find_if(begin(mPalettes), end(mPalettes), [&](const std::unique_ptr<Palette>& currentPalette){ return (palette == currentPalette->name()) ? true : false;});
    
    return (search != end(mPalettes)) ? search->get() : nullptr;
}

Palette * ResourceManager::layerPalette(std::string palette) const
{
    auto search = std::find_if(begin(mLayerPalettes), end(mLayerPalettes), [&](const std::unique_ptr<Palette>& currentPalette){ return (palette == currentPalette->name()) ? true : false;});
    
    return (search != end(mLayerPalettes)) ? search->get() : nullptr;    
}


Palette* ResourceManager::palette(unsigned int index) const
{
    return (index < mPalettes.size()) ? mPalettes[index].get() : nullptr;
}

std::vector<Palette *> ResourceManager::palettes() const
{
    std::vector<Palette*> palettes;
    
    for(const auto& palette: mPalettes)
        palettes.push_back(palette.get());
    
    return palettes;
}

std::vector<std::string> ResourceManager::paletteNames() const
{
    std::vector<std::string> names;
    
    for(const auto& palette: mPalettes)
        names.push_back(palette->name());
    
    return names;
}

bool ResourceManager::paletteFileExits(std::string palette) const
{
    if(fileExists(mCHISelPath + mStdPaths[PALETTE], palette + mFileExtensions[PALETTE]) || fileExists("./" + mStdPaths[PALETTE], palette + mFileExtensions[PALETTE]))
        return true;
    else
        return false;
}


bool ResourceManager::containMesh(std::string mesh) const
{
    return mMeshes.find(mesh) != mMeshes.end();
}

bool ResourceManager::containModel(std::string model) const
{
    return mModels.find(model) != end(mModels);
}

bool ResourceManager::containMaterial(std::string material) const
{
    return mMaterials.find(material) != mMaterials.end();
}

bool ResourceManager::containCamera(std::string camera) const
{
    return mCameras.find(camera) != end(mCameras);
}

bool ResourceManager::containScene(std::string scene) const
{
    return mScenes.find(scene) != end(mScenes);
}

bool ResourceManager::containRenderPass(std::string renderPass) const
{
    return mRenderPasses.find(renderPass) != end(mRenderPasses);
}

bool ResourceManager::containRenderTarget(std::string renderTarget) const
{
    return mRenderTargets.find(renderTarget) != end(mRenderTargets);
}

bool ResourceManager::containRenderTechnique(std::string renderTechnique) const
{
    return mRenderTechniques.find(renderTechnique) != end(mRenderTechniques);
}

bool ResourceManager::containTexture(std::string texture) const
{
    return mTextures.find(texture) != mTextures.end();
}

Mesh* ResourceManager::createMesh(std::string meshName)
{
    if(!containMesh(meshName))
    {
        mMeshes[meshName] = std::make_unique<Mesh>(meshName);
        return mMeshes[meshName].get();
    }
    else
        return nullptr;
}

Quad * ResourceManager::createQuad(std::string name, glm::vec2 orig, glm::vec2 dimensions, glm::vec4 color)
{
    if(!containMesh(name))
    {
        mMeshes[name] = std::make_unique<Quad>(name, orig, dimensions, color);
        return static_cast<Quad*>(mMeshes[name].get());
    }
    else
        return nullptr;    
}

Disk* ResourceManager::createDisk(std::string name, float innerRadius, float outerRadius, int slices)
{
    if(!containMesh(name))
    {
        mMeshes[name] = std::make_unique<Disk>(name, innerRadius, outerRadius, slices);
        return static_cast<Disk*>(mMeshes[name].get());
    }
    else
        return nullptr;    
}

Circle* ResourceManager::createCircle(std::string name, glm::vec3 center, float radius, int slices)
{
    if(!containMesh(name))
    {
        mMeshes[name] = std::make_unique<Circle>(name, center, radius, slices);
        return static_cast<Circle*>(mMeshes[name].get());
    }
    else
        return nullptr;        
}

Cylinder * ResourceManager::createCylinder(std::string name, float bottomRadius, float topRadius, float height, uint32_t radialSlices, uint32_t heightSteps, bool isOpen, glm::vec4 color)
{
    if(!containMesh(name))
    {
        mMeshes[name] = std::make_unique<Cylinder>(name, bottomRadius, topRadius, height, radialSlices, heightSteps, isOpen, color);
        return static_cast<Cylinder*>(mMeshes[name].get());
    }
    else
        return nullptr;      
}


Model3D * ResourceManager::createModel(std::string modelName)
{
    if(!containModel(modelName))
    {
        mModels[modelName] = std::make_unique<Model3D>(modelName);
        return mModels[modelName].get();
    }
    else
        return nullptr;    
}

Model3D* ResourceManager::createModel(std::string modelName, Mesh * mesh, std::vector<Material*> materials)
{
    auto newModel = createModel(modelName);

    if (newModel)
    {
        newModel->setMesh(mesh);
        newModel->setMaterials(materials);

        return newModel;
    }
    else
        return nullptr;
}

Material* ResourceManager::createMaterial(std::string name, std::string shaderName)
{
    if(!containMaterial(name))
    {
        mMaterials[name] = std::make_unique<Material>(name);
        
        auto mat = mMaterials[name].get();
        
        if(shaderName.length() > 0)
        {
            auto shader = shaderProgram(shaderName);
            
            if(shader != nullptr)
                mat->setShader(shader);
        }
        
        return mat;
    }
    else
        return nullptr;    
}

Camera* ResourceManager::createCamera(std::string name)
{
    if(!containCamera(name))
    {
        mCameras[name] = std::make_unique<Camera>(name);
        return mCameras[name].get();
    }
    else
        return nullptr;        
}

Camera* ResourceManager::createCamera(std::string name, glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    if(!containCamera(name))
    {
        mCameras[name] = std::make_unique<Camera>(name, projectionMatrix, viewMatrix);
        return mCameras[name].get();
    }
    else
        return nullptr;    
}

Camera* ResourceManager::createCamera(std::string name, Model3D* model)
{
    auto camera = createCamera(name);
    
    if(camera != nullptr)
    {
        camera->setAspectWidth(1280);
        camera->setAspectHeight(720);
        camera->setAspectRatio(static_cast<double>(1280)/static_cast<double>(720));
        camera->setFieldOfViewX(49.134);
        camera->setFieldOfViewY(28.841);
        camera->setFieldOfView(49.134);
        camera->setFocalLength(35);

        mRenderer->alignCameraToModel(camera, model);       
    }
    
    return camera;
}

Scene3D* ResourceManager::createScene(std::string name, std::vector<Camera *> cameras, std::vector<Model3D *> models)
{
    if(!containScene(name))
    {
        mScenes[name] = std::make_unique<Scene3D>(name, cameras, models);
        return mScenes[name].get();
    }
    else
        return nullptr;
}

Mesh* ResourceManager::copyMesh(const Mesh& sourceMesh, std::string meshName)
{
    if(!containMesh(meshName))
    {    
        mMeshes[meshName] = std::make_unique<Mesh>(sourceMesh);
        auto copyMesh = mMeshes[meshName].get();
        copyMesh->setName(meshName);
        return copyMesh;
    }
    else
        return nullptr;
}


RenderPass * ResourceManager::createRenderPass(std::string name, Scene3D* scene, Material* sceneMaterial, std::vector<BlendingState> blendingStates, bool depthTest, GLenum depthFunc)
{
    if(!containRenderPass(name))
    {
        mRenderPasses[name] = std::make_unique<RenderPass>(name, scene, sceneMaterial, blendingStates, depthTest, depthFunc);
        return mRenderPasses[name].get();
    }
    else
        return nullptr;   
}

RenderTarget * ResourceManager::createRenderTarget(std::string name, ResourceManager* manager, std::array<GLint, int(4)> viewport, const std::vector<RenderPass *>& passes, bool defaultTarget, GLuint defaultTargetID)
{
    if(!containRenderTarget(name))
    {
        mRenderTargets[name] = std::make_unique<RenderTarget>(name, manager, viewport, passes, defaultTarget, defaultTargetID);
        return mRenderTargets[name].get();
    }
    else
        return nullptr;          
}

RenderTarget * ResourceManager::createRenderTarget(std::string name, ResourceManager* manager, std::array<GLint, int(4)> viewport, const std::vector<RenderPass *>& passes, const std::vector<Texture *>& colorTargets, Texture* depthTarget, bool defaultTarget, GLuint defaultTargetID)
{
    if(!containRenderTarget(name))
    {
        mRenderTargets[name] = std::make_unique<RenderTarget>(name, manager, viewport, passes, colorTargets, depthTarget, defaultTarget, defaultTargetID);
        return mRenderTargets[name].get();
    }
    else
        return nullptr;    
}

RenderTechnique * ResourceManager::createRenderTechnique(std::string name, std::vector<RenderTarget *> targets, int life)
{
    if(!containRenderTechnique(name))
    {
        mRenderTechniques[name] = std::make_unique<RenderTechnique>(name, targets, false, life);
        return mRenderTechniques[name].get();
    }
    else
        return nullptr;        
}


// void ResourceManager::insertMesh(std::string meshName, Mesh* mesh)
// {
// //     if(mMeshes.find(meshName) == mMeshes.end())
// //         mMeshes[meshName] = mesh;
// }
// 
// void ResourceManager::insertModel(std::string modelName, Model3D* model)
// {
// //     if(mModels.find(modelName) == end(mModels))
// //         mModels[modelName] = model;
// }
// 
// 
// void ResourceManager::insertMaterial(std::string matName, Material* mat)
// {
// //     if(mMaterials.find(matName) == mMaterials.end())
// //         mMaterials[matName] = mat;
// }
// 
// void ResourceManager::insertTexture(Texture* texture)
// {
//     //std::pair<unsigned int, unsigned int> dimensions(texture->width(), texture->height());
//     int_fast64_t textureArrayIndex = (static_cast<int_fast64_t>(texture->internalFormat()) << 32) | (static_cast<int_fast64_t>(texture->width()) << 16) | (static_cast<int_fast64_t>(texture->height()));
//     LOG("Texture Array Index: ", textureArrayIndex, ", iformat: ", texture->internalFormat(), ", w: ", texture->width(), ", h: ", texture->height());
//     mTextureArrays[textureArrayIndex].push_back(texture);
// }

void ResourceManager::setRenderer(Renderer* renderer)
{
    mRenderer = renderer;
    mRenderer->setResourceManager(this);
}


void ResourceManager::initGLResourceData()
{
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &mGLConsts.maxUniformBlockSize );
    LOG("GL_MAX_UNIFORM_BLOCK_SIZE: ", mGLConsts.maxUniformBlockSize);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &mGLConsts.maxUniformBufferBindings );
    LOG("GL_MAX_UNIFORM_BUFFER_BINDINGS: ", mGLConsts.maxUniformBufferBindings);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &mGLConsts.maxUniformBufferAlignment );
    LOG("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: ", mGLConsts.maxUniformBufferAlignment);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mGLConsts.maxTextureUnits);
    LOG("GL_MAX_TEXTURE_IMAGE_UNITS: ", mGLConsts.maxTextureUnits);
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &mGLConsts.maxArrayTextureLayers);
    LOG("GL_MAX_ARRAY_TEXTURE_LAYERS: ", mGLConsts.maxArrayTextureLayers);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mGLConsts.maxTextureSize);    
    LOG("GL_MAX_TEXTURE_SIZE: ", mGLConsts.maxTextureSize);
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &mGLConsts.max3DTextureSize);
    LOG("GL_MAX_3D_TEXTURE_SIZE: ", mGLConsts.max3DTextureSize);    
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mGLConsts.maxColorAttachments);
    LOG("GL_MAX_COLOR_ATTACHMENTS: ", mGLConsts.maxColorAttachments);
    
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &mGLConsts.maxGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &mGLConsts.maxGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &mGLConsts.maxGroupCount[2]);
    LOG("GL_MAX_COMPUTE_WORK_GROUP_COUNT: ", mGLConsts.maxGroupCount[0], ", ", mGLConsts.maxGroupCount[1], ", ", mGLConsts.maxGroupCount[2]);
        
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &mGLConsts.maxGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &mGLConsts.maxGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &mGLConsts.maxGroupSize[2]);
    LOG("GL_MAX_COMPUTE_WORK_GROUP_SIZE: ", mGLConsts.maxGroupSize[0], ", ", mGLConsts.maxGroupSize[1], ", ", mGLConsts.maxGroupSize[2]);
    
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &mGLConsts.maxGroupInvocations);
    LOG("GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: ", mGLConsts.maxGroupInvocations);
    
    glGetIntegerv(GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB, &mGLConsts.maxVariableGroupInvocations);
    LOG("GL_MAX_COMPUTE_VARIABLE_GROUP_INVOCATIONS_ARB: ", &mGLConsts.maxVariableGroupInvocations);    
            
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &mGLConsts.maxImageUnits);
    LOG("GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS: ", mGLConsts.maxImageUnits);
    
    mFreeTexUnitList.resize(mGLConsts.maxTextureUnits);
    std::iota(rbegin(mFreeTexUnitList), rend(mFreeTexUnitList), 0);
        
    for(auto target : {GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY})
        for(auto sampler : {SamplerType::Float, SamplerType::Signed, SamplerType::Unsigned})
        {
            mFreeTexArrayIndices[{target, sampler}].resize(100);
            std::iota(rbegin(mFreeTexArrayIndices[{target, sampler}]), rend(mFreeTexArrayIndices[{target, sampler}]), 0);
        }
    
    mBindedImageArrays.resize(mGLConsts.maxImageUnits, nullptr);
    freeAllImageUnits();
}

Program* ResourceManager::createComputeShader(const std::string& name, const std::string& source)
{    
    auto prog = std::make_unique<Program>(name);
    Shader shader(name + ".comp", GL_COMPUTE_SHADER);

    shader.setSourceCode(source);
    shader.compileSourceCode();
    
    if (!shader.isCompiled())
    {
        LOG_ERR(shader.name(), ": ", shader.sourceCode());
        LOG_ERR(shader.name(), ":\n", shader.log());
    }

    prog->insertShader(shader);
    prog->linkShaders();

    if (!prog->isLinked())
        LOG_ERR(prog->log());
    else
    {
        prog->detachShaders();
        prog->deleteShaders();
    }
    
    //mShaders[name] = shader;
    auto compProgPtr = prog.get();
    mPrograms[name] = std::move(prog);
    
    return compProgPtr;
}


void ResourceManager::loadEffects(const std::string& path, const std::vector<std::string>& filters)
{
    QString qPath(path.c_str());
    QStringList qFilters;    
    for(const auto& filter: filters) qFilters.append(QString(filter.c_str()));
    
    GLSLFileLoader effectLoader(qPath, qFilters, mShaders, mPrograms);

    effectLoader.parseGLSLEffectFiles();
}

Texture* ResourceManager::loadTexture(std::string path, bool create)
{
    QImage image, imageGL;
        
    if(image.load(path.data()))
    {
        imageGL = QGLWidget::convertToGLFormat(image);
        
        GLsizei width = imageGL.width();
        GLsizei height = imageGL.height();
        GLint iformat;
        GLint format;

        if(imageGL.hasAlphaChannel())
        {
            iformat = GL_RGBA8;
            format = GL_RGBA;
        }
        else
        {
            iformat = GL_RGB8;
            format = GL_RGB;
        }

        std::vector<glm::byte> pixelData(imageGL.byteCount());        
        pixelData.assign(imageGL.bits(), imageGL.bits() + imageGL.byteCount());

        auto tex = createTexture(path, GL_TEXTURE_2D, iformat, width, height, format, GL_UNSIGNED_BYTE, pixelData, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, true);
                        
        return tex;
    }
    else
    {
        LOG_ERR("RManager couldn't load the file ", path);
        return nullptr;        
    }
}

void ResourceManager::generateTextureArrays()
{
    LOG("RManager generating texture Array: ");
    std::array<unsigned int, 3> index2D {0, 0, 0};
    std::array<unsigned int, 3> index1D {0, 0, 0};
    for(auto textures: mTextureArrays)
    {   
        Texture *first = textures.second.front();      
        GLint levels = static_cast<GLint>(log2(std::max(first->width(), first->height())) + 1);
        SamplerType sType = getSamplerType(first->internalFormat());
        
        if(first->target() == GL_TEXTURE_2D)
        {
            createTextureArray(15, textures.second, GL_TEXTURE_2D_ARRAY, first->internalFormat(), first->width(), first->height(), first->format(), first->type(), first->magFilter(), first->minFilter(), first->anisoFiltLevel(), GL_TRUE);
//             auto texArray = new TextureArray(index2D[static_cast<unsigned int>(sType)], 5, textures.second, GL_TEXTURE_2D_ARRAY, levels, first->internalFormat(), first->width(), first->height(), first->format(), first->type(), first->magFilter(), first->minFilter(), first->anisoFiltLevel());
//             mOGLTexArray.push_back(texArray);
//             mGLTex2DArrays[{sType, textures.first}].push_back(texArray);
            //index2D[static_cast<unsigned int>(sType)]++;
            //LOG("-- Index 2D: ", index2D[static_cast<unsigned int>(sType)] - 1, ", id: ", mGLTex2DArrays[{sType, textures.first}][mGLTex2DArrays[{sType, textures.first}].size() - 1]->id(), ", [", mGLTex2DArrays[{sType, textures.first}][mGLTex2DArrays[{sType, textures.first}].size() - 1]->internalFormat(),"], Dim: [", first->width(), ", ", first->height(), "] ");
        }
        else if(first->target() == GL_TEXTURE_1D)
        {
            auto texArray = new TextureArray(index1D[static_cast<unsigned int>(sType)], 5, textures.second, GL_TEXTURE_1D_ARRAY, levels, first->internalFormat(), first->width(), first->height(), first->format(), first->type(), first->magFilter(), first->minFilter(), first->anisoFiltLevel());
//             mOGLTexArray.push_back(texArray);
//             mGLTex1DArrays[{sType, textures.first}].push_back(texArray);
//             index1D[static_cast<unsigned int>(sType)]++;
//             LOG("-- Index 1D: ", index1D[static_cast<unsigned int>(sType)] - 1, ", id: ", mGLTex1DArrays[{sType, textures.first}][mGLTex1DArrays[{sType, textures.first}].size() - 1]->id(), ", [", mGLTex1DArrays[{sType, textures.first}][mGLTex1DArrays[{sType, textures.first}].size() - 1]->internalFormat(),"], Dim: [", first->width(), ", ", first->height(), "] ");
        }
       
        
        for(auto texture: textures.second)
            LOG("  - Name: ", texture->name());

        
    }        
}

int ResourceManager::commitFreeImageUnit(TextureArray* imageArray)
{
    if(mFreeImageUnitList.size() && imageArray->imageUnit() == -1)
    {
        auto unit = mFreeImageUnitList.back();
        mFreeImageUnitList.pop_back();
        
        imageArray->setImageUnit(unit);
        mBindedImageArrays[unit] = imageArray;
        mDirtyImageArrays.push_back(imageArray);
        
        return unit;
    }
    else return -1;
}

void ResourceManager::freeAllImageUnits()
{
    mFreeImageUnitList.resize(mGLConsts.maxImageUnits);    
    std::iota(rbegin(mFreeImageUnitList), rend(mFreeImageUnitList), 0);

    for(auto unit = 0; unit < mBindedImageArrays.size(); ++unit)
        if(mBindedImageArrays[unit] != nullptr)
        {
            mBindedImageArrays[unit]->setImageUnit(-1);
            mBindedImageArrays[unit] = nullptr;            
        }
}


Texture* ResourceManager::createTexture(std::string name, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const std::vector<glm::byte>& data, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, bool generate)
{
    int_fast64_t paramKey = (static_cast<int_fast64_t>(internalFormat) << 32) | (static_cast<int_fast64_t>(width) << 16) | (static_cast<int_fast64_t>(height));    
    SamplerType samplerKey = getSamplerType(internalFormat);
    
    GLenum arrayTarget = (target == GL_TEXTURE_2D) ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_1D_ARRAY;
    
    if(containTextureArray(arrayTarget, samplerKey, paramKey))
    {
        for(const auto& glTexArray : mGLTexArrays[arrayTarget][samplerKey][paramKey])
        {
            auto foundTexture = glTexArray->texture(name);
            if(foundTexture != nullptr)
                return foundTexture;
        }
    }
    else
    {
        if (!generate)
            createTextureArray(15, {}, arrayTarget, internalFormat, width, height, format, type, magFilter, minFilter, anitropyLevel, GL_TRUE);
        else
            createTextureArray(1, {}, arrayTarget, internalFormat, width, height, format, type, magFilter, minFilter, anitropyLevel, GL_FALSE);
    }

    auto tex = std::make_unique<Texture>(name, target, internalFormat, width, height, format, type, data, magFilter, minFilter, anitropyLevel, false);
    tex->setSamplerType(samplerKey);
    auto texPtr = tex.get();
    mTextures[name] = std::move(tex);
    
    LOG("1A Create Texture: ", texPtr->name());
    
    mGLTexArrays[arrayTarget][samplerKey][paramKey].back()->commitFreeLayer(texPtr);
            
    return texPtr;
}

Texture * ResourceManager::copyTexture(std::string sourceName, std::string copyName)
{
    auto source = texture(sourceName);
    
    if(source != nullptr)
        copyTexture(source, copyName);
    
    return source;
}

Texture * ResourceManager::copyTexture(Texture* source, std::string copyName)
{
    auto copy = createTexture(!copyName.length() ? source->name() + "Duplicate" : copyName, source->target(), source->internalFormat(), source->width(), source->height(), source->format(), source->type(),{}, source->magFilter(), source->minFilter(), source->anisoFiltLevel(), false);    
    
    glCopyImageSubData(source->textureArrayId(), source->textureArray()->target(), 0, 0, 0, source->textureArrayLayerIndex(), copy->textureArrayId(), copy->textureArray()->target(), 0, 0, 0, copy->textureArrayLayerIndex(), source->width(), source->height(), 1);
    
    LOG("Copy Texture data from: ", source->name(), " to ", copy->name());

    return copy;
}

bool ResourceManager::deleteTexture(Texture* texture)
{
    if(texture != nullptr)
    {
        auto textureArray = texture->textureArray();
        textureArray->freeLayer(texture->textureArrayLayerIndex());
        mTextures.erase(texture->name());
        
        if(textureArray->isEmpty())
            deleteTextureArray(textureArray);
        
        return true;
    }
    else
        return false;
}

void ResourceManager::renameTexture(Texture * texture, const std::string & name)
{
    mTextures[name] = std::move(mTextures[texture->name()]);
    mTextures.erase(texture->name());
    texture->setName(name);
}

TextureArray* ResourceManager::createTextureArray(unsigned int layerCount, const std::vector< Texture* >& textures, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, GLboolean sparseness)
{
    int_fast64_t paramKey = (static_cast<int_fast64_t>(internalFormat) << 32) | (static_cast<int_fast64_t>(width) << 16) | (static_cast<int_fast64_t>(height));
    SamplerType samplerKey = getSamplerType(internalFormat);
    GLint levels = static_cast<GLint>(log2(std::max(width, height)) + 1);
    auto index = mFreeTexArrayIndices[{target, samplerKey}].back();
    mFreeTexArrayIndices[{target, samplerKey}].pop_back();
    
    auto textureArray = std::make_unique<TextureArray>(index, layerCount, textures, target, levels, internalFormat, width, height, format, type, magFilter, minFilter, anitropyLevel, sparseness);
    auto texArrayPtr = textureArray.get();
    mGLTexArrays[target][samplerKey][paramKey].push_back(std::move(textureArray));
    
    texArrayPtr->setSamplerType(samplerKey);
    texArrayPtr->bindToTextureUnit(mFreeTexUnitList.back());
    mFreeTexUnitList.pop_back();
   
    if(mBindedTexArrays[{target, samplerKey}].size() < (index + 1))
        mBindedTexArrays[{target, samplerKey}].resize(index + 1, nullptr);
    mBindedTexArrays[{target, samplerKey}][index] = texArrayPtr;// .insert(mBindedTexArrays[{target, samplerKey}].begin() + index, texArrayPtr);
    mDirtyTextureArrays.push_back(texArrayPtr);
    
    if(mTextureUnitPerSampler[{target, samplerKey}].size() < (index + 1))
        mTextureUnitPerSampler[{target, samplerKey}].resize(index + 1, -1);
    mTextureUnitPerSampler[{target, samplerKey}][index] = texArrayPtr->textureUnit();//.insert(mTextureUnitPerSampler[{target, samplerKey}].begin() + index, texArrayPtr->textureUnit());
    mDirtySampler.insert({target, samplerKey});
    mDirtySampler.insert({GL_TEXTURE_1D_ARRAY, getSamplerType(GL_RGBA32F)});
    
    LOG("1A ResourceManager::Creating Array Id: ", texArrayPtr->id() ," Index: ", index, ", target: ", target, ", samplerKey: ", static_cast<int>(samplerKey), ", intFormat: ", internalFormat, ", texUnit: ", texArrayPtr->textureUnit(), ", width: ", texArrayPtr->width(), ", height: ", texArrayPtr->height());     
    
    return texArrayPtr; 
}

bool ResourceManager::deleteTextureArray(TextureArray* texArray)
{
    if(texArray != nullptr)
    {
        auto index = texArray->index();
        auto target = texArray->target();
        int_fast64_t paramKey = (static_cast<int_fast64_t>(texArray->internalFormat()) << 32) | (static_cast<int_fast64_t>(texArray->width()) << 16) | (static_cast<int_fast64_t>(texArray->height()));
        SamplerType samplerKey = getSamplerType(texArray->internalFormat());
        
        LOG("1A ResourceManager::Deleting Array Id: ", texArray->id(), " Index: ", index, ", target: ", target, ", samplerKey: ", static_cast<int>(samplerKey), ", texUnit: ", texArray->textureUnit(), ", intFormat: ", texArray->internalFormat(), ", width: ", texArray->width(), ", height: ", texArray->height());        
        
        mFreeTexArrayIndices[{target, samplerKey}].push_back(index);
        mFreeTexUnitList.push_back(texArray->textureUnit());

        mBindedTexArrays[{target, samplerKey}][index] = nullptr;
        mTextureUnitPerSampler[{target, samplerKey}][index] = 0;

        auto foundDirtyTextureArray = std::find_if(begin(mDirtyTextureArrays), end(mDirtyTextureArrays), [&](const TextureArray * currentArray) { return (currentArray == texArray) ? true : false; });
        
        if (foundDirtyTextureArray != end(mDirtyTextureArrays))
            mDirtyTextureArrays.erase(foundDirtyTextureArray);
                
        auto found = std::find_if(begin(mGLTexArrays[target][samplerKey][paramKey]), end(mGLTexArrays[target][samplerKey][paramKey]), [&](const std::unique_ptr<TextureArray>& currentArray){ return (currentArray.get() == texArray) ? true : false;});
        
        if(found != end(mGLTexArrays[target][samplerKey][paramKey]))
        {
            mGLTexArrays[target][samplerKey][paramKey].erase(found);
            if(mGLTexArrays[target][samplerKey][paramKey].size() == 0)
                mGLTexArrays[target][samplerKey].erase(paramKey);
        }
        
        return true;
    }
    else
        return false;    
}


void ResourceManager::deleteShader(std::string name)
{
    auto shader = mPrograms.find(name);
    if (shader != end(mPrograms))
    {
        //delete shader->second;
        mPrograms.erase(shader);
    }
}

Layer* ResourceManager::createLayer(std::string name, Layer::Type type, std::pair<int, int> resolution, const std::vector<glm::byte>& data, const std::vector<glm::byte>& mask, Palette* palette, bool createPalette)
{ 
    //TODO : the map key doesn't include the type. Doesn't work if internalFormat doesn't determines the type
    auto internalFormat = getInternalFormat(type);
    auto formatType = getFormatAndType(internalFormat);
    
    Texture *dataTexture = createTexture(name + ".Data", GL_TEXTURE_2D, internalFormat, resolution.first, resolution.second, formatType[0], formatType[1], data, GL_NEAREST, GL_NEAREST, 0, false);
    
    Texture *maskTexture = createTexture(name + ".Mask", GL_TEXTURE_2D, GL_R8, resolution.first, resolution.second, GL_RED, GL_UNSIGNED_BYTE, mask, GL_NEAREST, GL_NEAREST, 0, false);
    
    Texture *paletteTexture = createTexture(name + ".Palette", GL_TEXTURE_1D, GL_RGBA32F, 4096, 1, GL_RGBA, GL_FLOAT, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 0, false); 
                
    std::vector<std::pair<Texture*, Texture*>> textures {{dataTexture, maskTexture}};
    
    Palette* layerPalette = palette;
    
    if(createPalette)
    {
        mLayerPalettes.push_back(std::make_unique<Palette>(*palette));
        layerPalette = mLayerPalettes.back().get();
        layerPalette->setName(name);
    }
    

    //copyPalette->setIndex(mPalettes.size() - 1);
    layerPalette->setTexture(paletteTexture);
    //copyPalette->setDirty(true);
    //mChisel->addLocalPalette(copyPalette);
    //createPaletteTexture(name, copyPalette);
    
    if(type != Layer::Type::Register)
        mLayers.push_back(std::make_unique<Layer>(name, type, resolution, textures, layerPalette));
    else
    {        
        if(!mSQLiteDatabaseManager->isDatabaseOpen())
        {
            auto filepath = mCHISelPath + mStdPaths[DB] + chiselName() + mFileExtensions[DB];
            mSQLiteDatabaseManager->openDatabase(filepath);
            mChisel->setDatabase(filepath);
            
            auto resourceDir = mCHISelPath + mStdPaths[RESOURCES];        
            if(!directoryExists(resourceDir))
                createDirectory(resourceDir);
            mChisel->updateDataBaseResourcePath(resourceDir);
        }
        
        auto layerResourceDir = mCHISelPath + mStdPaths[RESOURCES] + name;        
        if(!directoryExists(layerResourceDir))
            createDirectory(layerResourceDir);
        
        mLayers.push_back(std::make_unique<RegisterLayer>(name, resolution, textures, layerPalette));
    }

    mLayers.back().get()->updatePaletteInterpolation();
    //createPaletteTexture(palette);
    
    return mLayers.back().get();
}

Layer * ResourceManager::createLayer(std::string name, Layer::Type type, std::pair<int, int> resolution, Texture* dataTexture, const std::vector<glm::byte>& mask, Palette* palette, bool createPalette)
{
    auto internalFormat = getInternalFormat(type);
    auto formatType = getFormatAndType(internalFormat);
    
    Texture *maskTexture = createTexture(name + ".Mask", GL_TEXTURE_2D, GL_R8, resolution.first, resolution.second, GL_RED, GL_UNSIGNED_BYTE, mask, GL_NEAREST, GL_NEAREST, 0, false);
    
    Texture *paletteTexture = createTexture(name + ".Palette", GL_TEXTURE_1D, GL_RGBA32F, 4096, 1, GL_RGBA, GL_FLOAT, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 0, false); 
                
    std::vector<std::pair<Texture*, Texture*>> textures {{dataTexture, maskTexture}};
    
    Palette* layerPalette = palette;
    
    if(createPalette)
    {
        mLayerPalettes.push_back(std::make_unique<Palette>(*palette));
        layerPalette = mLayerPalettes.back().get();
        layerPalette->setName(name);
    }

    layerPalette->setTexture(paletteTexture);
    
    if(type != Layer::Type::Register)
        mLayers.push_back(std::make_unique<Layer>(name, type, resolution, textures, layerPalette));
    else
    {        
        if(!mSQLiteDatabaseManager->isDatabaseOpen())
        {
            auto filepath = mCHISelPath + mStdPaths[DB] + chiselName() + mFileExtensions[DB];
            mSQLiteDatabaseManager->openDatabase(filepath);
            mChisel->setDatabase(filepath);
            
            auto resourceDir = mCHISelPath + mStdPaths[RESOURCES];        
            if(!directoryExists(resourceDir))
                createDirectory(resourceDir);
            mChisel->updateDataBaseResourcePath(resourceDir);
        }
        
        auto layerResourceDir = mCHISelPath + mStdPaths[RESOURCES] + name;        
        if(!directoryExists(layerResourceDir))
            createDirectory(layerResourceDir);
        
        mLayers.push_back(std::make_unique<RegisterLayer>(name, resolution, textures, layerPalette));
    }

    mLayers.back().get()->updatePaletteInterpolation();
    
    return mLayers.back().get();    
}

Layer * ResourceManager::createLayerFromTableField(const RegisterLayer* layer, const DataBaseField& field)
{    
    Layer* tempLayer;    
    std::vector<int> idValues;
    std::vector<int> integerFieldValues;
    std::vector<float> floatFieldValues;    

    mSQLiteDatabaseManager->beginTransaction();
    auto results = mSQLiteDatabaseManager->query("select id, " + field.mName + " from " + layer->name() + ";");
    mSQLiteDatabaseManager->commitTransaction();    
    
    if(field.mType == DataBaseField::Type::Integer)
    {   
        int max, min;
        max = std::numeric_limits<int>::min();
        min = std::numeric_limits<int>::max();
        
        for(auto row : results)
        {
            idValues.push_back(std::stoi(row[0]));
            
            if(row[1].length() != 0)
                integerFieldValues.push_back(std::stoi(row[1]));
            else
                integerFieldValues.push_back(0);
            
            max = std::max(max, integerFieldValues.back());
            min = std::min(min, integerFieldValues.back());
        }
        
        //auto tempPalette = duplicatePalette(layer->palette());
        tempLayer = createLayer(layer->name() + "_" + field.mName, Layer::Type::Int32, layer->resolution(), layer->palette());
        tempLayer->palette()->rescaleTo(min, max, true);
    }
    else if(field.mType == DataBaseField::Type::Double)
    {
        float max, min;
        max = std::numeric_limits<float>::min();
        min = std::numeric_limits<float>::max();        
        
        for(auto row : results)
        {
            idValues.push_back(std::stoi(row[0]));
            
            if(row[1].length() != 0)            
                floatFieldValues.push_back(std::stof(row[1]));
            else
                floatFieldValues.push_back(0);
            
            max = std::max(max, floatFieldValues.back());
            min = std::min(min, floatFieldValues.back());            
        }
        
        tempLayer = createLayer(layer->name() + field.mName, Layer::Type::Float32, layer->resolution(), layer->palette());
        tempLayer->palette()->rescaleTo(min, max);
    }
    
    commitFreeImageUnit(layer->dataTexture()->textureArray());
    commitFreeImageUnit(layer->maskTexture()->textureArray());      

    commitFreeImageUnit(tempLayer->dataTexture()->textureArray());
    commitFreeImageUnit(tempLayer->maskTexture()->textureArray());          
        
    std::string shaderCode =    "#version 450\n"
                                "#extension GL_ARB_compute_variable_group_size : enable\n\n"
                                "layout(local_size_variable) in;\n";

    std::vector<const Layer*> layers = {layer, tempLayer};    
    for(auto clayer : layers)
    {
        shaderCode += "layout(" + toGLSLImageFormat(clayer->dataTexture()->internalFormat()) + ", binding = " + std::to_string( clayer->dataTexture()->textureArray()->imageUnit()) + ") uniform " + prefixImageFormat(clayer->dataTexture()->internalFormat()) + "image2DArray data" + clayer->name() + ";\n";
        
        shaderCode += "layout(" + toGLSLImageFormat(clayer->maskTexture()->internalFormat()) + ", binding = " + std::to_string( clayer->maskTexture()->textureArray()->imageUnit()) + ") uniform " + prefixImageFormat(clayer->maskTexture()->internalFormat()) + "image2DArray mask" + clayer->name() + ";\n";            
    }
    
    auto idVecSize = static_cast<int>(ceil(idValues.size()/4.0)); 
    
    shaderCode +=   "layout(std140, binding = 9) uniform CustomData\n"
                    "{\n"
                    "    ivec4 idValues" + ((idVecSize > 1) ? "[" + std::to_string(idVecSize) + "]" : "") + ";\n";

    if(field.mType == DataBaseField::Type::Integer)
        shaderCode += "    ivec4 fieldValues" + ((idVecSize > 1) ? "[" + std::to_string(idVecSize) + "]" : "") + ";\n";
    else
        shaderCode += "    vec4 fieldValues" + ((idVecSize > 1) ? "[" + std::to_string(idVecSize) + "]" : "") + ";\n";
                    
    shaderCode +=   "};\n\n"
                    "void main()\n"
                    "{\n"
                    "    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);\n"
                    "    int value = int(imageLoad(data" + layer->name() + ", ivec3(pixelCoords, " + std::to_string(layer->dataTexture()->textureArrayLayerIndex()) + ")).x);\n\n"
                    "    float mask = imageLoad(mask" + layer->name() + ", ivec3(pixelCoords, " + std::to_string(layer->maskTexture()->textureArrayLayerIndex()) + ")).x;\n\n";
                    
    if(field.mType == DataBaseField::Type::Integer)
        shaderCode += "    for(int i = 0; i < " + std::to_string(integerFieldValues.size()) + "; ++i)\n";
    else
        shaderCode += "    for(int i = 0; i < " + std::to_string(floatFieldValues.size()) + "; ++i)\n";
    
    shaderCode +=   "    {\n"
                    "        int id;\n";
    if(field.mType == DataBaseField::Type::Integer)
        shaderCode += "        int newValue;\n"
                      "        int tempIndex = int(ceil(i/4.0));\n"
                      "        int index = (tempIndex > " + std::to_string(integerFieldValues.size()/4) + ") ? tempIndex : int(floor(i/4.0));\n";
    else
        shaderCode += "        float newValue;\n"
                      "        int tempIndex = int(ceil(i/4.0));\n"
                      "        int index = (tempIndex > " + std::to_string(floatFieldValues.size()/4) + ") ? tempIndex : int(floor(i/4.0));\n";
    shaderCode +=   "        if(i % 4 == 0)\n"
                    "        {\n"
                    "            id = idValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".x;\n"
                    "            newValue = fieldValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".x;\n"
                    "        }\n"
                    "        else if(i % 4 == 1)\n"
                    "        {\n"                    
                    "            id = idValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".y;\n"
                    "            newValue = fieldValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".y;\n"
                    "        }\n"                    
                    "        else if(i % 4 == 2)\n"
                    "        {\n"                    
                    "            id = idValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".z;\n"
                    "            newValue = fieldValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".z;\n"
                    "        }\n"                    
                    "        else if(i % 4 == 3)\n"
                    "        {\n"                    
                    "            id = idValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".w;\n"
                    "            newValue = fieldValues" + std::string(((idVecSize > 1) ? "[index]" : "")) + ".w;\n"
                    "        }\n"
                    
                    "        if(mask > 0 && value == id)\n"
                    "        {\n"
                    "            imageStore(data" + tempLayer->name() + ", ivec3(pixelCoords, " + std::to_string(tempLayer->dataTexture()->textureArrayLayerIndex()) + "), " + prefixImageFormat(tempLayer->dataTexture()->internalFormat()) + "vec4(newValue));\n"
                    "            imageStore(mask" + tempLayer->name() + ", ivec3(pixelCoords, " + std::to_string(tempLayer->maskTexture()->textureArrayLayerIndex()) + "), vec4(1.0));\n"
                    "        }\n" 
                    "    }\n"
                    "}\n";
    
    LOG("Shader code:\n", shaderCode);
    
    auto compShader = createComputeShader("Compute" + tempLayer->name(), shaderCode);
    
    std::vector<glm::byte> uniformData;
    
    auto idSize = idValues.size() * sizeof(int);
    idSize = ceil(idSize / 16.0) * 16.0;
    auto integerSize = integerFieldValues.size() * sizeof(int);
    integerSize = ceil(integerSize / 16.0) * 16.0;
    auto floatSize = floatFieldValues.size() * sizeof(float);
    floatSize = ceil(floatSize / 16.0) * 16.0;
    
    uniformData.resize(idSize + integerSize + floatSize);
    
     memcpy(uniformData.data(), idValues.data(), idValues.size() * sizeof(int));
     if(integerSize)
        memcpy(uniformData.data() + idSize, integerFieldValues.data(), integerFieldValues.size() * sizeof(int));
     else
        memcpy(uniformData.data() + idSize, floatFieldValues.data(), floatFieldValues.size() * sizeof(int));         
    
    mRenderer->computeShader(compShader, uniformData);
    
    return tempLayer;
}


Layer* ResourceManager::loadLayer(std::string name, std::string path)
{
    Layer::Type type;
    std::pair<int, int> resolution;
    float opacity;
    std::string paletteName;
    std::vector<glm::byte> data;
    std::vector<glm::byte> mask;    
    
    std::string absoluteFileName = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[LAYER]) + name + mFileExtensions[LAYER];
    
    std::ifstream input(absoluteFileName, std::ios::binary);
    
    Layer* newLayer = nullptr;
    
    if(input.is_open())
    {
        cereal::PortableBinaryInputArchive archive(input);
        archive(type, resolution, opacity, paletteName, data, mask);
        
        Palette* layerPalette = this->layerPalette(paletteName);
        
        bool createPalette = false;
        
        if(layerPalette == nullptr)
        {
            layerPalette = mPalettes[0].get();
            createPalette = true;
        }
        
        newLayer = createLayer(name, type, resolution, data, mask, layerPalette, createPalette);
            
        if(type == Layer::Type::Register)
        {
            DataBaseTable schema;
            int primaryColumn;
            std::vector<bool> secondaryColumns;
            archive(schema, primaryColumn, secondaryColumns);
            auto registerLayer = dynamic_cast<RegisterLayer *>(newLayer);
            registerLayer->setPrimaryColumn(primaryColumn);
            registerLayer->setSecondaryColumns(secondaryColumns);
            registerLayer->copySchema(schema);            
        }
        newLayer->setOpacity(opacity);
        
        newLayer->setDirty(false);
    }
    
    return newLayer;
}

void ResourceManager::saveLayer(Layer* layer, std::string path)
{    
    auto data = mRenderer->readTexture(layer->dataTexture());
    auto mask = mRenderer->readTexture(layer->maskTexture());

    auto absolutePath = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[LAYER]);    
    if(!directoryExists(absolutePath))
        createDirectory(absolutePath);
        
    std::string absoluteFileName = absolutePath + layer->name() + mFileExtensions[LAYER];
    
    std::ofstream output(absoluteFileName, std::ios::binary);
    cereal::PortableBinaryOutputArchive archive(output);
    archive(layer->type(), layer->resolution(), layer->opacity(), layer->palette()->name(), data, mask);    
    
    if(layer->type() == Layer::Type::Register)
    {
        auto registerLayer = dynamic_cast<RegisterLayer *>(layer);
        archive(*registerLayer->tableSchema(), registerLayer->primaryColumn(), registerLayer->secondaryColumns());
    }    
}

void ResourceManager::saveRawLayer(Layer* layer, std::string path)
{
    auto data = mRenderer->readTexture(layer->dataTexture());
    auto mask = mRenderer->readTexture(layer->maskTexture());

    auto absolutePath = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[LAYER]);    
    if(!directoryExists(absolutePath))
        createDirectory(absolutePath);
    
    std::string absoluteFileName = absolutePath + layer->name() + mFileExtensions[LAYER];

    std::ofstream outputRaw(absoluteFileName, std::ios::binary);
    auto type = static_cast<int>(layer->type());
    auto resolution = layer->resolution();
    outputRaw.write(reinterpret_cast<const char *>(&type), sizeof(type));
    outputRaw.write(reinterpret_cast<const char *>(&resolution.first), sizeof(resolution.first));
    outputRaw.write(reinterpret_cast<const char *>(&resolution.second), sizeof(resolution.second));
    auto dataSize = static_cast<uint64_t>(data.size());
    outputRaw.write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
    outputRaw.write(reinterpret_cast<const char *>(data.data()), data.size());
    auto maskSize = static_cast<uint64_t>(mask.size());
    outputRaw.write(reinterpret_cast<const char *>(&maskSize), sizeof(maskSize));
    outputRaw.write(reinterpret_cast<const char *>(mask.data()), mask.size());

    if(layer->type() ==Layer::Type::Register)
    {
        auto registerLayer = dynamic_cast<RegisterLayer *>(layer);

        auto numFields = static_cast<uint64_t>(registerLayer->tableSchema()->fields().size());
        outputRaw.write(reinterpret_cast<const char *>(&numFields), sizeof(numFields));

        for(auto fields : registerLayer->tableSchema()->fields())
        {
            auto fieldType = fields.mType;
            outputRaw.write(reinterpret_cast<const char *>(&fieldType), sizeof(fieldType));
        }
    }                
}

Layer* ResourceManager::duplicateLayer(Layer* sourceLayer)
{
    auto search = std::find_if(begin(mLayers), end(mLayers), [&](const std::unique_ptr<Layer>& currentLayer){ return (sourceLayer == currentLayer.get()) ? true : false;});

    if(search != end(mLayers))
    {
        auto newName = createValidLayerName(sourceLayer->name());        
        auto data = mRenderer->readTexture(sourceLayer->dataTexture());
        auto mask = mRenderer->readTexture(sourceLayer->maskTexture());        
        
        Layer* newLayer = createLayer(newName, sourceLayer->type(), sourceLayer->resolution(), data, mask, sourceLayer->palette());
        newLayer->setOpacity(sourceLayer->opacity());
        
        if(newLayer->registerType())
        {
            auto sourceDbLayer = static_cast<RegisterLayer*>(sourceLayer);
            auto dbLayer = static_cast<RegisterLayer*>(newLayer);
            
            dbLayer->copySchemaAndVisibility(*sourceDbLayer->tableSchema(), sourceDbLayer->fieldsVisibility());
            
            mSQLiteDatabaseManager->duplicateTable(sourceDbLayer->name(), dbLayer->name());
        }

        return newLayer;
    }
    
    return nullptr;
}

void ResourceManager::unloadLayer(Layer* layer)
{
    auto search = std::find_if(begin(mLayers), end(mLayers), [&](const std::unique_ptr<Layer>& currentLayer){ return (layer->name() == currentLayer->name()) ? true : false;});
    
    if(search != end(mLayers))
    {
        LOG("Delete layer texture data: ", layer->dataTexture()->textureArrayId(), ", ", layer->dataTexture()->textureArrayLayerIndex());
        LOG("Delete layer texture mask: ", layer->maskTexture()->textureArrayId(), ", ", layer->maskTexture()->textureArrayLayerIndex());
        //LOG("Delete layer texture palette: ", layer->paletteTexture()->textureArrayId(), ", ", layer->paletteTexture()->textureArrayLayerIndex());
        deleteTexture(layer->dataTexture());                
        deleteTexture(layer->maskTexture());
        deleteTexture(layer->paletteTexture());
        
        mLayers.erase(search);
    }
}

void ResourceManager::deleteLayer(Layer* layer)
{
    auto filePath = mCHISelPath + mStdPaths[LAYER] + layer->name() + mFileExtensions[LAYER];    
    std::remove(filePath.c_str());   
    
    if(layer->registerType())
    {
        mSQLiteDatabaseManager->deleteTable(layer->name());
        deleteDirectory(mCHISelPath + mStdPaths[RESOURCES] + layer->name());
    }
    auto palette = layer->palette();
        
    unloadLayer(layer);        
    deleteLayerPalette(palette);
}

void ResourceManager::deleteDiskLayer(std::string name)
{
    auto layerFilePath = mCHISelPath + mStdPaths[LAYER] + name + mFileExtensions[LAYER];
    auto paletteFilePath = mCHISelPath + mStdPaths[PALETTE] + name + mFileExtensions[PALETTE];
    
    std::remove(paletteFilePath.c_str());
    std::remove(layerFilePath.c_str());
    
    if(fileExists(mCHISelPath + mStdPaths[DB], chiselName() + mFileExtensions[DB]))        
    {
        if(!mSQLiteDatabaseManager->isDatabaseOpen())
        {
            auto filepath = mCHISelPath + mStdPaths[DB] + chiselName() + mFileExtensions[DB];
            mSQLiteDatabaseManager->openDatabase(filepath);
            mChisel->setDatabase(filepath);
        }
        
        mSQLiteDatabaseManager->deleteTable(name);
        deleteDirectory(mCHISelPath + mStdPaths[RESOURCES] + name);        
    }
}

void ResourceManager::renameLayer(Layer * layer, const std::string& newName)
{
    auto filename = mCHISelPath + mStdPaths[LAYER] + layer->name() + mFileExtensions[LAYER];
    auto newFilename = mCHISelPath + mStdPaths[LAYER] + newName + mFileExtensions[LAYER];
    rename(filename.c_str(), newFilename.c_str());

    layer->setName(newName);

    renameTexture(layer->dataTexture(), newName + ".Data");
    renameTexture(layer->maskTexture(), newName + ".Mask");
    renameTexture(layer->paletteTexture(), newName + ".Palette");
}

void ResourceManager::exportLayerAsImage(Layer* layer, const std::string& pathName)
{
    auto mask = mRenderer->readTexture(layer->maskTexture());
    
    switch(layer->dataTexture()->type())
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        {
            exportImage<glm::byte>(pathName, layer, mRenderer->readTexture(layer->dataTexture()), mask);
            break;
        }
        case GL_SHORT:
        {
            exportImage<int16_t>(pathName, layer, mRenderer->readShortTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_UNSIGNED_SHORT:
        {
            exportImage<uint16_t>(pathName, layer, mRenderer->readUShortTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_INT:
        {
            exportImage<int32_t>(pathName, layer, mRenderer->readIntTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_UNSIGNED_INT:
        {
            exportImage<uint32_t>(pathName, layer, mRenderer->readUIntTexture(layer->dataTexture()), mask);
            break;            
        }
        case GL_HALF_FLOAT:
        {
            exportImage<half_float::half>(pathName, layer, mRenderer->readHalfFloatTexture(layer->dataTexture()), mask);
            break;
        }
        case GL_FLOAT:
        {
            exportImage<float>(pathName, layer, mRenderer->readFloatTexture(layer->dataTexture()), mask);
            break;
        }
    }      
}

template<class T>
void ResourceManager::exportImage(std::string pathName, Layer* layer, const std::vector<T>& data, const std::vector<glm::byte>& mask)
{
    std::vector<unsigned char> pixelData(layer->width() * layer->height() * 4);
        
    for (std::size_t texel = 0; texel < mask.size(); ++texel)
    {
        if (mask[texel] > 0)
        {            
            auto color = layer->palette()->color(data[texel]) * 255;
            pixelData[4 * texel] = static_cast<unsigned char>(color.r);
            pixelData[4 * texel + 1] = static_cast<unsigned char>(color.g);
            pixelData[4 * texel + 2] = static_cast<unsigned char>(color.b);
            pixelData[4 * texel + 3] = static_cast<unsigned char>(color.a);
        }
        else
        {
            pixelData[4 * texel] = 0;
            pixelData[4 * texel + 1] = 0;
            pixelData[4 * texel + 2] = 0;
            pixelData[4 * texel + 3] = 0;
        }
    }
    
    QImage layerImage(pixelData.data(), layer->width(), layer->height(), QImage::Format_RGBA8888);
    layerImage = layerImage.mirrored(false, true);
    layerImage.save(pathName.data());    
}

void ResourceManager::exportImage2(std::string pathName, uint16_t width, uint16_t height, const std::vector<float>& data)
{
    std::vector<unsigned char> pixelData(width * height * 4);
        
    for (std::size_t texel = 0; texel < data.size(); texel+=2)
    {        
        if(data[texel] > 0)
        {
            glm::uvec2 texelIndex((texel/2) % width, texel /(2 * height));
            LOG("Texel[", texelIndex.x, ", ", texelIndex.y, "] -> [", data[texel], ", ", data[texel + 1], "] -> [", static_cast<uint32_t>(data[texel] * 2048), ", ", static_cast<uint32_t>(data[texel + 1] * 2048),"] -> [", static_cast<uint32_t>(data[texel] * 4096), ", ", static_cast<uint32_t>(data[texel + 1] * 4096),"]");
            
            glm::ivec2 neighborTexel(data[texel], data[texel + 1]);
            
            pixelData[4 * neighborTexel.y * height + 4 * neighborTexel.x] = static_cast<unsigned char>(0);
            pixelData[4 * neighborTexel.y * height + 4 * neighborTexel.x + 1] = static_cast<unsigned char>(200);
            pixelData[4 * neighborTexel.y * height + 4 * neighborTexel.x + 2] = static_cast<unsigned char>(255);
            pixelData[4 * neighborTexel.y * height + 4 * neighborTexel.x + 3] = static_cast<unsigned char>(255);  
            
//             pixelData[4 * texel/2] = static_cast<unsigned char>(255);
//             pixelData[4 * texel/2 + 1] = static_cast<unsigned char>(0);
//             pixelData[4 * texel/2 + 2] = static_cast<unsigned char>(0);
//             pixelData[4 * texel/2 + 3] = static_cast<unsigned char>(255);
        }
    }
    
    QImage layerImage(pixelData.data(), width, height, QImage::Format_RGBA8888);
    layerImage = layerImage.mirrored(false, true);
    layerImage.save(pathName.data());    
}


std::string ResourceManager::layerResourceDir(std::string name) const
{
    return mCHISelPath + mStdPaths[RESOURCES] + name;
}

std::vector<std::string> ResourceManager::layerResourceFiles(std::string name) const
{
    return directoryFiles(mCHISelPath + mStdPaths[RESOURCES] + name);
}

Palette* ResourceManager::createPalette(std::string name, Palette::Type type, const std::map<double, glm::vec4>& controlPoints)
{
    //Texture *paletteTexture = createTexture(name, GL_TEXTURE_1D, GL_RGBA32F, 4096, 1, GL_RGBA, GL_FLOAT, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 0, false);        
    
    if(controlPoints.size())
        mPalettes.push_back(std::make_unique<Palette>(mPalettes.size(), name, "", type, controlPoints));
    else
        mPalettes.push_back(std::make_unique<Palette>(mPalettes.size(), name, "", type, std::map<double, glm::vec4>({{0.0, glm::vec4(1.0, 0.647, 0.0, 1.0)},{10.0, glm::vec4(0.0, 0.0, 1.0, 1.0)}})));
    
    auto newPalette = mPalettes.back().get();
    //newPalette->setTexture(paletteTexture);
    newPalette->setDirty(true);        
    newPalette->setDiskDirty(true);

    return newPalette;
}

void ResourceManager::renamePaletteFile(unsigned int index, std::string newName)
{    
    auto renamed = renameFile("palettes/", mPalettes[index]->name() + mFileExtensions[PALETTE], newName + mFileExtensions[PALETTE]);
    
    if(renamed) mPalettes[index]->setName(newName);
}

Palette* ResourceManager::duplicatePalette(Palette* palette, bool validateName)
{
    auto search = std::find_if(begin(mPalettes), end(mPalettes), [&](const std::unique_ptr<Palette>& currentPalette){ return (palette == currentPalette.get()) ? true : false;});

    if(search != end(mPalettes))
    {
        if(validateName)
        {
            auto newName = createValidLayerName(palette->name());            
            mPalettes.push_back(std::make_unique<Palette>(mPalettes.size(), newName, "", palette->type(), palette->controlPoints()));
        }
        else
            mPalettes.push_back(std::make_unique<Palette>(mPalettes.size(), palette->name(), "", palette->type(), palette->controlPoints()));
        
        auto newPalette = mPalettes.back().get();
        //newPalette->setTexture(paletteTexture);
        newPalette->setDirty(true);        
        newPalette->setDiskDirty(true);

        return newPalette;
    }
    
    return nullptr;    
}

Palette* ResourceManager::copyPaletteToCollection(Palette* palette)
{
    bool valid = false;
    std::string newName = palette->name();

    for(unsigned int i = 0; i < std::numeric_limits<unsigned int>::max() && !valid; ++i)
    {
        auto search = std::find_if(begin(mPalettes), end(mPalettes), [&](const std::unique_ptr<Palette>& currentPalette) {
            return (newName == currentPalette->name()) ? true : false;
        });
            
        if(search == end(mPalettes))
            valid = true;
        else
            newName = palette->name() + "_" + std::to_string(i + 1);
    }
    
    mPalettes.push_back(std::make_unique<Palette>(mPalettes.size(), newName, "", palette->type(), palette->controlPoints()));
    savePalette(mPalettes.back(), "", "palettes/");
    auto newPalette = mPalettes.back().get();
    
    return newPalette;
}

void ResourceManager::deletePalette(unsigned int index)
{
    if (index < mPalettes.size())
    {
        auto filePath = mAppPath + mStdPaths[PALETTE] + mPalettes[index]->name() + mFileExtensions[PALETTE];
        std::remove(filePath.c_str());

        mPalettes.erase(mPalettes.begin() + index);
    }
}

void ResourceManager::deletePalette(Palette* palette)
{
    auto search = std::find_if(begin(mPalettes), end(mPalettes), [&](const std::unique_ptr<Palette>& currentPalette){ return (palette->name() == currentPalette->name()) ? true : false;});
    
    if(search != end(mPalettes))
    {
        auto filePath = mCHISelPath + mStdPaths[PALETTE] + palette->name() + mFileExtensions[PALETTE];    
        std::remove(filePath.c_str());    

        mPalettes.erase(search);        
    }        
}

void ResourceManager::deleteLastPalette()
{
    if (mPalettes.size())
    {
        auto filePath = mCHISelPath + mStdPaths[PALETTE] + mPalettes.back()->name() + mFileExtensions[PALETTE];
        std::remove(filePath.c_str());

        mPalettes.pop_back();
    }
}

bool ResourceManager::deleteLayerPalette(Palette* palette)
{
    auto search = std::find_if(begin(mLayerPalettes), end(mLayerPalettes), [&](const std::unique_ptr<Palette>& currentPalette){ return (palette == currentPalette.get()) ? true : false;});
    
    if(search != end(mLayerPalettes))
    {     
        auto filePath = mCHISelPath + mStdPaths[PALETTE] + palette->name() + mFileExtensions[PALETTE];    
        std::remove(filePath.c_str()); 
        
        mLayerPalettes.erase(search);
        return true;
    }
    
    return false;
}

Palette * ResourceManager::loadPalette(std::string name, std::string path, bool layerPalette)
{
    std::string absoluteFileName = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[PALETTE]) + name + mFileExtensions[PALETTE];    
    std::ifstream input(absoluteFileName, std::ios::binary);    
    
    Palette* newPalette = nullptr;
    
    if(input.is_open())
    {
        std::unique_ptr<Palette> palette;
        
        cereal::PortableBinaryInputArchive archive(input);
        archive(palette);
        
        palette->setName(name);
        palette->setDiskDirty(false);

        newPalette = palette.get();
        
        if(!layerPalette)
        {
            palette->setIndex(mPalettes.size());            
            mPalettes.push_back(std::move(palette));
        }
        else
            mLayerPalettes.push_back(std::move(palette));
    }
    
    return newPalette;
}

void ResourceManager::loadPalettes(std::string directoryName)
{
    auto files = directoryFiles(directoryName);
    
    for(const auto& file : files)
        loadPalette(file, directoryName);
}

void ResourceManager::savePalette(unsigned int index, std::string name, std::string path)
{
    if(index < mPalettes.size())
    {
        const auto& palette = mPalettes[index];
        
        auto absolutePath = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[PALETTE]);    
        if(!directoryExists(absolutePath))
            createDirectory(absolutePath);
        
        std::string absoluteFileName = absolutePath + ((name.length() > 0) ? name : palette->name()) + mFileExtensions[PALETTE];
    
        std::ofstream output(absoluteFileName, std::ios::binary);
        
        cereal::PortableBinaryOutputArchive archive(output);
        archive(palette);
    }
}

void ResourceManager::savePalette(const std::unique_ptr<Palette>& palette, std::string name, std::string path)
{
    if(palette.get() != nullptr)
    {
        auto absolutePath = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[PALETTE]);    
        if(!directoryExists(absolutePath))
            createDirectory(absolutePath);

        std::string absoluteFileName = absolutePath + ((name.length() > 0) ? name : palette->name()) + mFileExtensions[PALETTE];

        std::ofstream output(absoluteFileName, std::ios::binary);

        cereal::PortableBinaryOutputArchive archive(output);
        archive(palette);
    }    
}

void ResourceManager::saveLayerPalette(const std::unique_ptr<Palette>& palette, std::string name, std::string path)
{
    if(palette != nullptr)
    {
        auto absolutePath = ((path.length() > 0) ? path : mCHISelPath + mStdPaths[PALETTE]);    
        if(!directoryExists(absolutePath))
            createDirectory(absolutePath);
        
        std::string absoluteFileName = absolutePath + ((name.length() > 0) ? name : palette->name()) + mFileExtensions[PALETTE];
    
        std::ofstream output(absoluteFileName, std::ios::binary);
        
        cereal::PortableBinaryOutputArchive archive(output);
        archive(palette);
    }    
}

void ResourceManager::deleteTempPalette(unsigned int index)
{
//     for(auto i = index + 1; i < mTempPalettes.size(); ++i)
//         mTempPalettes[i]->setIndex(mTempPalettes[i]->index() - 1);
    if(index < mPalettes.size())
    {
        mLayerPalettes.push_back(std::move(mPalettes.at(index)));
        //mPalettes.back()->setIndex(static_cast<int>(mPalettes.size() - 1));        
        mPalettes.erase(begin(mPalettes) + index);
    }
}

void ResourceManager::changePaletteToPermanent(Palette* palette)
{
    auto searchTempNames = std::find_if(begin(mLayerPalettes), end(mLayerPalettes), [&](const std::unique_ptr<Palette>& currentPalette)
    { 
        auto other = currentPalette->name();
        auto comp = palette->name() == currentPalette->name();

        return (palette == currentPalette.get()) ? true : false;        
    });
    
    if(searchTempNames != end(mLayerPalettes))
    {                   
        auto search = std::find_if(begin(mPalettes), end(mPalettes), [&](const std::unique_ptr<Palette>& currentPalette){ return (palette->name() == currentPalette->name()) ? true : false;});
        
        if(search != end(mPalettes))
        {        
            bool valid = false;
            std::string newName;
            
            for(unsigned int i = 1; i < std::numeric_limits<unsigned int>::max() && !valid; ++i)
            {
                newName = palette->name() + "_" + std::to_string(i);
                
                auto search = std::find_if(begin(mPalettes), end(mPalettes), [&](const std::unique_ptr<Palette>& currentPalette){
                    auto other = currentPalette->name();
                    auto comp = newName ==      currentPalette->name();
                    return (newName == currentPalette->name()) ? true : false;});
                
                if(search == end(mPalettes))
                    valid = true;
            }
            
            palette->setName(newName);
        }
        auto ptr = *searchTempNames->get();
        mPalettes.push_back(std::move(*searchTempNames));
        mPalettes.back()->setIndex(static_cast<int>(mPalettes.size() - 1));
        mLayerPalettes.erase(searchTempNames);
    }

    auto hola = 0;
}

void ResourceManager::createPaletteTexture(Palette* palette)
{
//     auto type = palette->type();
//     if(palette->texture(type) == nullptr)
//     {
//         Texture *paletteTexture = createTexture(palette->name() + "_"  + std::to_string(static_cast<int>(type)), GL_TEXTURE_1D, GL_RGBA32F, 4096, 1, GL_RGBA, GL_FLOAT, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 0, false);   
//         
//         palette->setTexture(paletteTexture, type);
//     }
}

void ResourceManager::createPaletteTexture(const std::string& layerName, Palette* palette)
{
    std::pair<int, int> resolution {4096, 1};
    
    Texture *paletteTexture = createTexture(layerName + ".Palette", GL_TEXTURE_1D, GL_RGBA32F, resolution.first, resolution.second, GL_RGBA, GL_FLOAT, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 0, false);    
        
    palette->setTexture(paletteTexture);    
}


void ResourceManager::deletePaletteTexture(Palette* palette, Palette::Type type)
{
//     if(palette->texture(type) != nullptr)
//     {
//         deleteTexture(palette->texture(type));
//         palette->setTexture(nullptr, type);
//     }
}


void ResourceManager::loadScene3D(std::string name, std::string path)
{
    std::string absoluteFileName = ((path.length() > 0) ? path : mCHISelPath) + ((name.length() > 0) ? name : mCHISelName) + mFileExtensions[SCENE];
    std::ifstream input(absoluteFileName, std::ios::binary);

    cereal::PortableBinaryInputArchive archive(input);

    std::string sceneName;
    archive(sceneName);
    
    auto scene = std::make_unique<Scene3D>(sceneName);
    
    std::size_t cameraCount;
    archive(cameraCount);
    for (auto i = 0; i < cameraCount; ++i)
    {
        Camera camera;
        archive(camera); 
        auto uniqueCamera = std::make_unique<Camera>(camera);
        scene->insertCamera(uniqueCamera.get());
        mCameras[camera.name()] = std::move(uniqueCamera);        
    }
    
    std::string currentCameraName;
    archive(currentCameraName);    
    scene->updateCamera(mCameras.at(currentCameraName).get());    

    std::size_t lightCount;
    archive(lightCount);
    for (auto i = 0; i < lightCount; ++i)
    {
        Light light;
        archive(light);
        auto uniqueLight = std::make_unique<Light>(light);
        scene->insertLight(uniqueLight.get());
        mLights[light.name()] = std::move(uniqueLight);
    }

    std::size_t meshCount;
    archive(meshCount);
    for(auto i = 0; i < meshCount; ++i)
    {
        Mesh mesh;
        archive(mesh);
        // temp fix
        auto resized = mesh.resizeBufferCount();
        if(resized)
            mChisel->setSessionDirty(true);
        
        mMeshes[mesh.name()] = std::make_unique<Mesh>(mesh);
    }
    
    std::size_t materialCount;
    archive(materialCount);
    for(auto i = 0; i < materialCount; ++i)
    {
        std::string name;
        std::string shaderName;
        archive(name, shaderName);
        
        auto material = std::make_unique<Material>(name, mPrograms[shaderName].get());
        
        ColorChannel diffuse;
        std::string diffuseTextureName;
        archive(diffuse.mColor, diffuseTextureName);
        
        if(diffuseTextureName.length() > 0)
            diffuse.mTexture = texture(diffuseTextureName);
        
        material->setDiffuse(diffuse);
        
        mMaterials[material->name()] = std::move(material);
    }

    std::size_t modelCount;
    archive(modelCount);    
    for (auto i = 0; i < modelCount; ++i)
    {
        std::string modelName;
        std::string meshName;
        glm::mat4 matrix;
        
        archive(modelName, meshName, matrix);
        
        auto model = std::make_unique<Model3D>(modelName, mesh(meshName), matrix);
                        
        std::size_t matCount;
        archive(matCount);
        for(auto i = 0; i < matCount; ++i)
        {
            std::string name;
            archive(name);
            model->insertMaterial(material(name));            
        }
        
        if(mModels.find(modelName) == end(mModels))
        {
            scene->insertModel(model.get());
            mModels[modelName] = std::move(model);
        }
    }
    
    auto scenePtr = scene.get();
    
    //mCHISelName = scene->name();
    mScenes[name] = std::move(scene);
    
    mRenderer->swapChiselScene(scenePtr);
}

void ResourceManager::importScene3D(std::string name, std::string extension, std::string path)
{
    if(mCHISelName.length() > 0)
        unloadCHiselFile();
    
    auto scene = std::make_unique<Scene3D>(name);
    std::transform(begin(extension), end(extension), begin(extension), ::tolower);
    auto fullPath = path + name + "." + extension;
        
    if(extension == "fbx")
        FBXLoader loader(this, scene.get(), fullPath);                
    else if(extension == "ply")
        PLYLoader loader(this, scene.get(), fullPath);
    else if(extension == "obj")
        OBJLoader loader(this, scene.get(), fullPath);
        
    auto scenePtr = scene.get();
        
    setDefaultChiselPath(mDefaultChiselPath);
    mCHISelPath = mDefaultChiselPath;
   
    if(!directoryExists(mCHISelPath + mStdPaths[LAYER]))
        createDirectory(mCHISelPath + mStdPaths[LAYER]);
    mChisel->updateLayerPath(mCHISelPath + mStdPaths[LAYER], true);
    
    mSQLiteDatabaseManager->close();
    
    mCHISelName = scene->name();
    mScenes[scene->name()] = std::move(scene);
    
    mRenderer->swapChiselScene(scenePtr);        
}

void ResourceManager::saveScene3D(std::string name, std::string path)
{    
    std::string absoluteFileName = ((path.length() > 0) ? path : mCHISelPath) + ((name.length() > 0) ? name : mCHISelName) + mFileExtensions[SCENE];
    std::ofstream output(absoluteFileName, std::ios::binary);

    cereal::PortableBinaryOutputArchive archive(output);
    
    auto scene = mRenderer->scene();
    
    archive(scene->name());

    archive(scene->cameras().size());
    for (auto camera : scene->cameras())
        archive(*camera);    
    archive(scene->camera()->name());

    archive(scene->lights().size());
    for (auto light : scene->lights())
        archive(*light);

    archive(scene->meshes().size());
    for(auto mesh : scene->meshes())
        archive(*mesh);

    archive(scene->materials().size());
    for (auto material : scene->materials())
    {
        archive(material->name(), material->shader()->name());
        archive(material->diffuseChannel().mColor, (material->diffuseChannel().mTexture != nullptr) ? material->diffuseChannel().mTexture->name() : "");
    }

    archive(scene->models().size());
    for(auto model : scene->models())
    {
        archive(model->name(), model->mesh()->name(), model->modelMatrix());
        archive(model->materials().size());
        for(auto mat : model->materials())
            archive(mat->name());
    }
}

void ResourceManager::unloadScene3D(std::string name)
{
    auto& scene = mScenes.at(name);
    
    mRenderer->removeScene(scene.get());    
    
    for(const auto& camera : scene->cameras())
        mCameras.erase(camera->name());
    
    for(const auto& light : scene->lights())
        mLights.erase(light->name());
    
    for(const auto& mesh : scene->meshes())
        mMeshes.erase(mesh->name());
    
    for(const auto& material : scene->materials())
    {
        deleteTexture(material->diffuseTexture());
        mMaterials.erase(material->name());
    }
    
    for(const auto& model : scene->models())
        mModels.erase(model->name());
        
    mScenes.erase(name);
}

void ResourceManager::createTopology(std::string name)
{
    auto found = mScenes.find(name);

    if (found != end(mScenes))
    {
        auto topologyMesh = copyMesh(*(found->second.get()->meshes()[0]), ":neighborhoodMesh");
        topologyMesh->generateAdjacencyInformation();
        auto topologyModel = createModel(":neighborhoodModel", topologyMesh, { material("Dummy") });
        auto topologyScene = createScene(":neighborEgdesScene", { camera("orthoCamera") }, { topologyModel });
        topologyScene->setPrimitive(Primitive::Lines);

        mRenderer->swapTopologyScene(topologyScene);
    }
}

void ResourceManager::loadTopology(std::string name, std::string path)
{
    std::string absoluteFileName = ((path.length() > 0) ? path : mCHISelPath) + ((name.length() > 0) ? name : mCHISelName) + mFileExtensions[TOP];
    std::ifstream input(absoluteFileName, std::ios::binary);

    cereal::PortableBinaryInputArchive archive(input);

    auto topologyMesh = createMesh(":neighborhoodMesh");

    archive(*topologyMesh);

    auto normOrthoCamera = camera("orthoCamera");
    auto topologyModel = createModel(":neighborhoodModel", topologyMesh, { material("Dummy") });
    auto topologyScene = createScene(":neighborEgdesScene", { normOrthoCamera }, { topologyModel });

    topologyScene->setPrimitive(Primitive::Lines);

    mRenderer->swapTopologyScene(topologyScene);
}

void ResourceManager::saveTopology(std::string name, std::string path)
{
    std::string absoluteFileName = ((path.length() > 0) ? path : mCHISelPath) + ((name.length() > 0) ? name : mCHISelName) + mFileExtensions[TOP];
    std::ofstream output(absoluteFileName, std::ios::binary);

    cereal::PortableBinaryOutputArchive archive(output);

    auto topologyMesh = mesh(":neighborhoodMesh");
    archive(*topologyMesh);
}

void ResourceManager::unloadTopology()
{
    auto topologyScene = scene(":neighborEgdesScene");
    
    mRenderer->removeScene(topologyScene);

    mMeshes.erase(":neighborhoodMesh");
    mModels.erase(":neighborhoodModel");

    mScenes.erase(":neighborEgdesScene");
}

void ResourceManager::exportScene(std::string name, std::string extension, std::string path)
{
   if(extension == "fbx")
   {
       FBXExporter exporter;
       //exporter.exportSceneToFile(path + name + "." + extension, mRenderer->scene());
   }
}

void ResourceManager::exportModel(std::string filePath, std::string extension, Model3D* model, const std::map<std::string, std::vector<uint32_t> >& segmentation, Camera* camera, bool exportCamera)
{
    if(extension == "fbx")
    {
        FBXExporter exporter;

        exporter.exportModelToFile(filePath, model, segmentation, camera, exportCamera);
    }
}

void ResourceManager::exportChiselProjectToUnity(std::string name, std::string path)
{
    if(path.length() > 0 && name.length() > 0)
    {
        createDirectory(path);
        
        exportModel(path + name, "fbx", mRenderer->scene()->models()[0], {}, mRenderer->scene()->camera(), false);
            
        copyFile(mCHISelPath + mStdPaths[DB] + mCHISelName + mFileExtensions[DB], path + mStdPaths[DB] + name + mFileExtensions[DB]);
        
        if(directoryExists(mCHISelPath + mStdPaths[RESOURCES]))
            copyDirectory(mCHISelPath + mStdPaths[RESOURCES], path + mStdPaths[RESOURCES], false);                
                    
        createDirectory(path + mStdPaths[LAYER]);
        createDirectory(path + "textures");
                        
        for(const auto& layer: mChisel->layers())
        {
            saveRawLayer(layer, path + mStdPaths[LAYER]);
            exportLayerAsImage(layer, path + "textures/" + layer->name() + ".png");
        }              
    }        
}


void ResourceManager::setDefaultChiselPath(std::string path)
{
    (!directoryExists(path)) ? createDirectory(path) : clearDirectory(path);
    
    mDefaultChiselPath = path;    
}

void ResourceManager::loadChiselFile(std::string name, std::string path)
{
    if(mCHISelName.length() > 0)
        unloadCHiselFile();
    
    mCHISelPath = path;
    mCHISelName = name;
    
    std::string absoluteFileName = mCHISelPath + mCHISelName + mFileExtensions[CHIS];
    std::ifstream input(absoluteFileName, std::ios::binary);

    if(input.is_open())
    {
        cereal::JSONInputArchive archive(input);
        
        try
        {
            archive(cereal::make_nvp("version", mVersion));
        }
        catch(cereal::Exception& e)
        {
            mVersion = 0.0;
        }
        
        archive(cereal::make_nvp("scenePath", mStdPaths[SCENE]));
        archive(cereal::make_nvp("sceneName", mCHISelName));
        loadScene3D(mCHISelName, "");

        loadTopology(mCHISelName, "");
        
        std::string databaseName;
        archive(cereal::make_nvp("databasePath", mStdPaths[DB]));
        archive(cereal::make_nvp("databaseName", databaseName));
        if(databaseName.length())
        {
            auto filepath = mCHISelPath + mStdPaths[DB] + chiselName() + mFileExtensions[DB];
            mSQLiteDatabaseManager->openDatabase(filepath);
            mChisel->setDatabase(filepath);
            mChisel->updateDataBaseResourcePath(mCHISelPath + mStdPaths[RESOURCES]);
        }            
        
        archive(cereal::make_nvp("palettePath", mStdPaths[PALETTE]));
        std::vector<std::string> paletteNames;
        archive(CEREAL_NVP(paletteNames));
        for(const auto& name: paletteNames)
        {
/*            auto loadedPalette = */loadPalette(name, "", true);
//             if(loadedPalette != nullptr)
//                 mChisel->addLocalPalette(loadedPalette);
        }        
        
        archive(cereal::make_nvp("layerPath", mStdPaths[LAYER]));
        mChisel->updateLayerPath(mCHISelPath + mStdPaths[LAYER], true);
        std::vector<std::string> layerNames;
        archive(CEREAL_NVP(layerNames));
        std::reverse(begin(layerNames), end(layerNames));
        for(const auto& name: layerNames)
        {
            auto loadedLayer = loadLayer(name, "");
            if(loadedLayer != nullptr)
                mChisel->addActiveLayer(loadedLayer);
        }        
    }
}

void ResourceManager::saveChiselProject(std::string name, std::string path)
{
    std::string oldPath;
    std::string oldName;
    
    bool newPath = path.length() > 0;
    if(newPath)
    {
        oldPath = mCHISelPath;
        mCHISelPath = path;
        createDirectory(path);
    }
    
    if(name.length() > 0 && mCHISelName != name)
    {
        oldName = mCHISelName;
        mCHISelName = name;

        mScenes[name] = std::move(mScenes[oldName]);
        mScenes.erase(oldName);
    }
    
    std::string absoluteFileName = mCHISelPath + mCHISelName + mFileExtensions[CHIS];
    std::ofstream output(absoluteFileName, std::ios::binary);
    
    cereal::JSONOutputArchive archive(output);
    
    archive(cereal::make_nvp("version", 1.0));
    archive(cereal::make_nvp("scenePath", mStdPaths[SCENE]));
    archive(cereal::make_nvp("sceneName", mCHISelName));
    saveScene3D(mCHISelName);

    saveTopology(mCHISelName);
    
    archive(cereal::make_nvp("databasePath", mStdPaths[DB]));
    if(mSQLiteDatabaseManager->isDatabaseOpen())
        archive(cereal::make_nvp("databaseName", mCHISelName));
    else
        archive(cereal::make_nvp("databaseName", std::string("")));
        
    if(newPath)
    {
        copyFile(oldPath + mStdPaths[DB] + oldName + mFileExtensions[DB], mCHISelPath + mStdPaths[DB] + mCHISelName + mFileExtensions[DB]);
        
        if(directoryExists(oldPath + mStdPaths[RESOURCES]))
            copyDirectory(oldPath + mStdPaths[RESOURCES], mCHISelPath + mStdPaths[RESOURCES], false);
    }
            
    archive(cereal::make_nvp("layerPath", mStdPaths[LAYER]));
    if(!directoryExists(mCHISelPath + mStdPaths[LAYER]))
        createDirectory(mCHISelPath + mStdPaths[LAYER]);        
    std::vector<std::string> layerNames;
    for(const auto& layer: mChisel->layers())
    {
        layerNames.push_back(layer->name());
        if(layer->isDirty() || newPath)
        {
            layer->setDirty(false);
            saveLayer(layer);
        }
    }
    archive(CEREAL_NVP(layerNames));
        
    if(newPath)
        copyDirectory(oldPath + "/" + mStdPaths[LAYER], mCHISelPath + mStdPaths[LAYER], false);
    
    mChisel->updateLayerPath(mCHISelPath + mStdPaths[LAYER], false);
    
    
    archive(cereal::make_nvp("palettePath", mStdPaths[PALETTE]));
    if(!directoryExists(mCHISelPath + mStdPaths[PALETTE]))
        createDirectory(mCHISelPath + mStdPaths[PALETTE]);    
    std::vector<std::string> paletteNames;
    for(const auto& palette : mLayerPalettes)
    {
        paletteNames.push_back(palette->name());
        
        if(palette->isDiskDirty() || newPath)
        {
            palette->setDiskDirty(false);
            saveLayerPalette(palette);
        }        
    }
    
    archive(CEREAL_NVP(paletteNames));
}

void ResourceManager::unloadCHiselFile()
{
    for(const auto& layer : mLayers)
    {
        deleteTexture(layer->dataTexture());
        deleteTexture(layer->maskTexture());
//         auto type = (layer->discrete()) ? Palette::Type::Discrete : Palette::Type::Linear;
//         deletePaletteTexture(layer->palette(), type);
        deleteTexture(layer->paletteTexture());
    }    
    mLayers.clear();
    
    mLayerPalettes.clear();

    while(mLocalPaletteIndex < mPalettes.size())
    {
        //deleteTexture(mPalettes.back()->texture());
        mPalettes.pop_back();        
    }
    
    unloadTopology();
    unloadScene3D(mCHISelName);
    
    mChisel->clearChiselSession();
    
    mCHISelName = "";
    mCHISelPath = "";
}


// *** Private Methods *** //

bool ResourceManager::renameFile(std::string path, std::string oldName, std::string newName)
{
    return QFile::rename(QString::fromStdString(path + oldName), QString::fromStdString(path + newName));
}

bool ResourceManager::copyFile(std::string source, std::string destination)
{
    return QFile::copy(QString::fromStdString(source), QString::fromStdString(destination));
}

std::vector<std::string> ResourceManager::directoryFiles(std::string path) const
{
    QDir dir(QString::fromStdString(path));
    std::vector<std::string> files;
    
    if(dir.exists())
    {
        foreach(QString fileName, dir.entryList(QDir::Files))
            if (fileName.endsWith(".plt"))
            {
                fileName.chop(4);
                files.push_back(fileName.toStdString());
            }
            else
                files.push_back(fileName.toStdString());
    }
    
    return files;
}

bool ResourceManager::fileExists(std::string path, std::string name) const
{
    QDir dir(QString::fromStdString(path));
    
    if(dir.exists())
        return dir.entryList(QDir::Files).contains(QString::fromStdString(name));

    return false;
}


bool ResourceManager::directoryExists(std::string path) const
{
    QDir dir(QString::fromStdString(path));
    return dir.exists();
}

bool ResourceManager::isDirectoryEmpty(std::string path) const
{
    QDir dir(QString::fromStdString(path));
    return dir.entryList(QDir::Files).size() == 0;    
}


bool ResourceManager::createDirectory(std::string path)
{
    QDir dir;
    return dir.mkpath(QString::fromStdString(path));
}

bool ResourceManager::clearDirectory(std::string path)
{
    QDir dir(QString::fromStdString(path));

    if (dir.exists())
    {
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
        foreach(QString dirItem, dir.entryList())
            dir.remove(dirItem);

        dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
        foreach(QString dirItem, dir.entryList())
        {
            QDir subDir(dir.absoluteFilePath(dirItem));
            subDir.removeRecursively();
        }

        return true;
    }
    
    return false;
}

bool ResourceManager::deleteDirectory(std::string path)
{
    QDir dir(QString::fromStdString(path));

    if (dir.exists())
        return dir.removeRecursively();
    
    return false;
}


bool ResourceManager::copyDirectory(std::string source, std::string destination, bool overwriteFiles)
{
    QString sourceDir = QString::fromStdString(source);
    QString destinationDir = QString::fromStdString(destination);
    
    QDir originDirectory(sourceDir);
    if (!originDirectory.exists())
        return false;

    QDir destinationDirectory(destinationDir);
//     if(destinationDirectory.exists() && !overwriteDir)
//         return false;
//     else if(destinationDirectory.exists() && overwriteDir)
//         destinationDirectory.removeRecursively();
// 
//     originDirectory.mkpath(destinationDir);

    foreach (QString directoryName, originDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QString destinationPath = destinationDir + "/" + directoryName;
        originDirectory.mkpath(destinationPath);
        copyDirectory((sourceDir + "/" + directoryName).toStdString(), destinationPath.toStdString(), overwriteFiles);
    }

    foreach (QString fileName, originDirectory.entryList(QDir::Files))
        if(!QFile::exists(destinationDir + "/" + fileName) || overwriteFiles)
            QFile::copy(sourceDir + "/" + fileName, destinationDir + "/" + fileName);

    /*! Possible race-condition mitigation? */
    QDir finalDestination(destinationDir);
    finalDestination.refresh();
    if(finalDestination.exists())
        return true;

    return false;    
}



bool ResourceManager::containTextureArray(GLenum targetKey, SamplerType samplerKey, int_fast64_t paramKey)
{
    //TODO: Need to recheck the whole process to see the app behave correctly when the texture array is full
    if(mGLTexArrays.count(targetKey) > 0 && mGLTexArrays[targetKey].count(samplerKey) > 0 && mGLTexArrays[targetKey][samplerKey].count(paramKey) > 0 && mGLTexArrays[targetKey][samplerKey][paramKey].back()->isFull())
        LOG("ResourceManager::containTextureArray ", mGLTexArrays[targetKey][samplerKey][paramKey].back()->id(), " is full");
    
    return mGLTexArrays.count(targetKey) > 0 && mGLTexArrays[targetKey].count(samplerKey) > 0 && mGLTexArrays[targetKey][samplerKey].count(paramKey) > 0 && !mGLTexArrays[targetKey][samplerKey][paramKey].back()->isFull();
}


GLenum ResourceManager::getInternalFormat(Layer::Type type)
{
    switch(type)
    {
        case Layer::Type::Int8:
            return GL_R8I;
        case Layer::Type::UInt8:
            return GL_R8UI;            
        case Layer::Type::Int16:
            return GL_R16I;
        case Layer::Type::UInt16:
            return GL_R16UI;
        case Layer::Type::Int32:
            return GL_R32I;
        case Layer::Type::UInt32:
            return GL_R32UI;
        case Layer::Type::Float16:
            return GL_R16F;
        case Layer::Type::Float32:
            return GL_R32F;
        case Layer::Type::Register:
            return GL_R32UI;    
    }
}


SamplerType ResourceManager::getSamplerType(GLenum internalFormat)
{
    switch (internalFormat)
    {
    case GL_R8I:
    case GL_R16I:
    case GL_R32I:
    case GL_RG16I:
    case GL_RG32I:
    case GL_RGB16I:
    case GL_RGB32I:
        return SamplerType::Signed;
    case GL_R8UI:
    case GL_R16UI:
    case GL_R32UI:
    case GL_RG16UI:
    case GL_RG32UI:
    case GL_RGB16UI:
    case GL_RGB32UI:
        return SamplerType::Unsigned;
    case GL_R16F:
    case GL_R32F:
    case GL_RG16F:
    case GL_RG32F:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_RGB:
    case GL_RGBA:
    case GL_RGB8:
    case GL_RGBA8:        
        return SamplerType::Float;
    default:
        return SamplerType::Float;
    }
}

std::array< GLenum, int(2) > ResourceManager::getFormatAndType(GLenum internalFormat)
{
    std::array<GLenum, 2> formatType;
    
    switch(internalFormat)
    {

        case GL_R8I:
        case GL_R16I:
        case GL_R32I:
        case GL_R8UI:
        case GL_R16UI:
        case GL_R32UI:
        {
            formatType[0] = GL_RED_INTEGER;
            break;
        }
        case GL_R8:
        case GL_R16F:
        case GL_R32F:
        {
            formatType[0] = GL_RED;
            break;
        }
        case GL_RG8I:
        case GL_RG16I:
        case GL_RG32I:
        case GL_RG8UI:
        case GL_RG16UI:
        case GL_RG32UI:
        {
            formatType[0] = GL_RG_INTEGER;
            break;            
        }
        case GL_RG8:       
        case GL_RG16F:
        case GL_RG32F:
        {
            formatType[0] = GL_RG;
            break;
        }
        case GL_RGB8I:
        case GL_RGB16I:
        case GL_RGB32I:
        case GL_RGB8UI:
        case GL_RGB16UI:
        case GL_RGB32UI:
        {
            formatType[0] = GL_RGB_INTEGER;
            break;
        }        
        case GL_RGB8:       
        case GL_RGB16F:
        case GL_RGB32F:
        {
            formatType[0] = GL_RGB;
            break;
        }
        case GL_RGBA8I:
        case GL_RGBA16I:
        case GL_RGBA32I:
        case GL_RGBA8UI:
        case GL_RGBA16UI:
        case GL_RGBA32UI:
        {
            formatType[0] = GL_RGBA_INTEGER;
            break;            
        }
        case GL_RGBA8:       
        case GL_RGBA16F:
        case GL_RGBA32F:
        {
            formatType[0] = GL_RGBA;
            break;
        }
    }
    
    switch(internalFormat)
    {        
        case GL_R8I:            
        case GL_RG8I:
        case GL_RGB8I:
        case GL_RGBA8I:
        {
            formatType[1] = GL_BYTE;
            break;
        }
        case GL_R8UI:
        case GL_R8:
        case GL_RG8UI:            
        case GL_RG8:
        case GL_RGB8UI: 
        case GL_RGB8:
        case GL_RGBA8UI:            
        case GL_RGBA8: 
        {
            formatType[1] = GL_UNSIGNED_BYTE;
            break;
        }
        case GL_R16I:        
        case GL_RG16I:
        case GL_RGB16I:
        case GL_RGBA16I:
        {
            formatType[1] = GL_SHORT;
            break;            
        }
        case GL_R16UI:
        case GL_RG16UI:            
        case GL_RGB16UI:
        case GL_RGBA16UI:
        {
            formatType[1] = GL_UNSIGNED_SHORT;
            break;            
        }        
        case GL_R32I:
        case GL_RG32I:
        case GL_RGB32I:
        case GL_RGBA32I:
        {
            formatType[1] = GL_INT;
            break;
        }        
        case GL_R32UI:
        case GL_RG32UI:

        case GL_RGB32UI: 
        case GL_RGBA32UI:
        {
            formatType[1] = GL_UNSIGNED_INT;
            break;
        }
        case GL_R16F:
        case GL_RG16F:
        case GL_RGB16F:
        case GL_RGBA16F:
        {
            formatType[1] = GL_HALF_FLOAT;
            break;
        }
        case GL_R32F:
        case GL_RG32F:
        case GL_RGB32F:
        case GL_RGBA32F:
        {
            formatType[1] = GL_FLOAT;
            break;
        }
    }
    
    return formatType;
}

