#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "texture.h"
#include "texturearray.h"
#include "material.h"
#include "mesh.h"
#include "quad.h"
#include "disk.h"
#include "circle.h"
#include "cylinder.h"
#include "model3d.h"
#include "camera.h"
#include "light.h"
#include "renderpass.h"

#include "layer.h"
#include "registerlayer.h"
#include "palette.h"

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>

typedef std::map<int_fast64_t, std::vector<std::unique_ptr<TextureArray>>> ParamMap;
typedef std::map<SamplerType, ParamMap> SamplerParamMap;

class Renderer;
class RenderTarget;
class RenderTechnique;
class Chisel;
class SQLiteDatabaseManager;

struct GLConstants
{
    int maxUniformBlockSize = 0;
    int maxUniformBufferBindings = 0;
    int maxUniformBufferAlignment = 0;

    int maxTextureUnits = 0;
    int maxArrayTextureLayers = 0;
    int maxTextureSize = 0;
    int max3DTextureSize = 0;
    int maxColorAttachments = 0;

    int maxImageUnits = 0;
    std::array<int, 3> maxGroupCount;
    std::array<int, 3> maxGroupSize;
    int maxGroupInvocations = 0;
    int maxVariableGroupInvocations = 0;     
};

class ResourceManager
{
public:
    ResourceManager(Chisel* chisel, Renderer* renderer);
    ~ResourceManager();
    
    const GLConstants& glConstants() const { return mGLConsts; }
    Texture* texture(std::string texture) const;
    Program* shaderProgram(std::string program) const;
    const std::map<std::string, std::unique_ptr<Program> >& programs() const { return mPrograms; }
    Material* material(std::string material) const;
    Mesh* mesh(std::string mesh) const;
    Model3D* model(std::string model) const;
    
    bool isValidName(const std::string candidate) const;
    std::string createValidName(const std::string candidate, std::vector<std::string> existingNames) const;
    std::string createValidLayerName(const std::string name) const;
    std::string createValidPaletteName(const std::string name) const;
    std::vector<std::string> existingLayerNames() const;
    std::vector<std::string> existingPaletteNames() const;
    bool layerExists(const std::string layerName) const;
    bool paletteExists(const std::string paletteName) const;     
    
    Layer* layer(std::string layer) const;
    std::vector<Layer*> layers() const;
    
    Palette* palette(std::string palette) const;
    Palette* layerPalette(std::string palette) const;
    Palette* palette(unsigned int index) const;
    std::vector<Palette*> palettes() const;
    std::vector<std::string> paletteNames() const;
    std::size_t paletteCount() const { return mPalettes.size(); }
    bool paletteFileExits(std::string palette) const;
    SQLiteDatabaseManager* databaseManager() const { return mSQLiteDatabaseManager.get(); }

    bool containMesh(std::string mesh) const;
    bool containModel(std::string model) const;
    bool containMaterial(std::string material) const;    
    bool containCamera(std::string camera) const;
    bool containScene(std::string scene) const;
    bool containRenderPass(std::string renderPass) const;
    bool containRenderTarget(std::string renderTarget) const;
    bool containRenderTechnique(std::string renderTechnique) const;
    bool containTexture(std::string texture) const;
    
    Mesh* createMesh(std::string meshName);    
    Quad* createQuad(std::string name, glm::vec2 orig, glm::vec2 dimensions, glm::vec4 color = {0.5, 0.5, 0.5, 1.0});
    Disk* createDisk(std::string name, float innerRadius, float outerRadius, int slices);
    Circle* createCircle(std::string name, glm::vec3 center, float radius, int slices);
    Cylinder* createCylinder(std::string name, float bottomRadius, float topRadius, float height, uint32_t radialSlices, uint32_t heightSteps, bool isOpen, glm::vec4 color = {0.5, 0.5, 0.5, 1.0});
    Model3D* createModel(std::string modelName);
    Model3D* createModel(std::string modelName, Mesh* mesh, std::vector< Material* > materials);
    Material* createMaterial(std::string name, std::string shaderName = "");
    Camera* createCamera(std::string name);
    Camera* createCamera(std::string name, glm::mat4 projectionMatrix, glm::mat4 viewMatrix = glm::mat4(1.0));
    Camera* createCamera(std::string name, Model3D* model);
    Scene3D* createScene(std::string name, std::vector<Camera* > cameras, std::vector<Model3D* > models);
    
    Mesh* copyMesh(const Mesh& sourceMesh, std::string meshName);
    
    RenderPass* createRenderPass(std::string name, Scene3D* scene, Material* sceneMaterial, std::vector<BlendingState> blendingStates = {BlendingState()}, bool depthTest = true, GLenum depthFunc = GL_GREATER);
    RenderTarget* createRenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes = {}, bool defaultTarget = false, GLuint defaultTargetID = 0);
    RenderTarget* createRenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, const std::vector< Texture* >& colorTargets, bool defaultTarget, GLuint defaultTargetID = 0) { return createRenderTarget(name, manager, viewport, passes, colorTargets, nullptr, defaultTarget, defaultTargetID);}
    RenderTarget* createRenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, const std::vector< Texture* >& colorTargets, Texture* depthTarget, bool defaultTarget, GLuint defaultTargetID = 0);
    RenderTechnique* createRenderTechnique(std::string name, std::vector<RenderTarget *> targets, int life = -1);
        
    void setRenderer(Renderer* renderer);

    void initGLResourceData();
    Program* createComputeShader(const std::string& name, const std::string& source);
    void loadEffects(const std::string& path, const std::vector<std::string>& filters);    
    Texture* loadTexture(std::string path, bool create = false);    
    void generateTextureArrays();

    const SamplerParamMap& texture1DArrays() const { return mGLTexArrays.at(GL_TEXTURE_1D_ARRAY); }
    const SamplerParamMap& texture2DArrays() const { return mGLTexArrays.at(GL_TEXTURE_2D_ARRAY); }
    const std::map< std::pair<GLenum, SamplerType>, std::vector<TextureArray*> >& bindedTextureArrays() const { return mBindedTexArrays; }
    const std::vector< TextureArray* >& dirtyTextureArrays() const { return mDirtyTextureArrays; }
    void clearDirtyTextureArrays() { mDirtyTextureArrays.clear(); }
    const std::map< std::pair<GLenum, SamplerType>, std::vector<GLint>>& bindedTexUnits() const { return mTextureUnitPerSampler; }
    const std::set< std::pair<GLenum, SamplerType> > dirtySampler() const { return mDirtySampler; }
    void clearDirtySampler() { mDirtySampler.clear(); }
    
    int commitFreeImageUnit(TextureArray* imageArray);
    void freeAllImageUnits();
    void freeImageUnit(unsigned int unit) { mFreeImageUnitList.push_back(unit); }
    const std::vector<TextureArray*>& dirtyImageArrays() const { return mDirtyImageArrays; }
    void clearDirtyImageArrays() { mDirtyImageArrays.clear(); }
    

    Texture* createTexture(std::string name, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const std::vector<glm::byte>& data, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, bool generate);
    Texture* copyTexture(std::string sourceName, std::string copyName = "");
    Texture* copyTexture(Texture* source, std::string copyName = "");
    bool deleteTexture(Texture* texture);
    void renameTexture(Texture* texture, const std::string& name);
    
    TextureArray* createTextureArray(unsigned int layerCount, const std::vector<Texture*>& textures, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, GLboolean sparseness);
    bool deleteTextureArray(TextureArray* texArray);
    
    void deleteShader(std::string name);
    
    Layer* createLayer(std::string name, Layer::Type type, std::pair<int, int> resolution, Palette* palette = nullptr, bool createPalette = true){ return createLayer(name, type, resolution, std::vector<glm::byte>{}, std::vector<glm::byte>{}, palette, createPalette); };    
    Layer* createLayer(std::string name, Layer::Type type, std::pair<int, int> resolution, const std::vector<glm::byte>& data, const std::vector<glm::byte>& mask, Palette* palette = nullptr, bool createPalette = true);
    Layer* createLayer(std::string name, Layer::Type type, std::pair<int, int> resolution, Texture* dataTexture, const std::vector<glm::byte>& mask, Palette* palette = nullptr, bool createPalette = true);    
    Layer* createLayerFromTableField(const RegisterLayer* layer, const DataBaseField& field);
    Layer* loadLayer(std::string name, std::string path = "");
    void saveLayer(Layer* layer, std::string path = "");
    void saveRawLayer(Layer* layer, std::string path = "");
    Layer* duplicateLayer(Layer* sourceLayer);
    void unloadLayer(Layer* layer);
    void deleteLayer(Layer* layer);
    void deleteDiskLayer(std::string name);
    void renameLayer(Layer* layer, const std::string& newName);
    void exportLayerAsImage(Layer* layer, const std::string& pathName);
    template<class T>
    void exportImage(std::string pathName, Layer* layer, const std::vector<T>& data, const std::vector<glm::byte>& mask);
    void exportImage2(std::string pathName, uint16_t width, uint16_t height, const std::vector<float>& data);    
    std::string layerResourceDir(std::string name) const;
    std::vector<std::string> layerResourceFiles(std::string name) const;    

    Palette* createPalette(std::string name, Palette::Type type, const std::map<double, glm::vec4>& controlPoints);
    void renamePaletteFile(unsigned int index, std::string newName);
    Palette* duplicatePalette(Palette* palette, bool validateName = true);
    Palette* copyPaletteToCollection(Palette* palette);
    void deletePalette(unsigned int index);
    void deletePalette(Palette* palette);
    void deleteLastPalette();
    bool deleteLayerPalette(Palette* palette);
    Palette* loadPalette(std::string name, std::string path, bool layerPalette = false);
    void loadPalettes(std::string directoryName);
    void savePalette(unsigned int index, std::string name= "", std::string path = "");
    void savePalette(const std::unique_ptr<Palette>& palette, std::string name= "", std::string path = "");
    void saveLayerPalette(const std::unique_ptr<Palette>& palette, std::string name= "", std::string path = "");
    void deleteTempPalette(unsigned int index);
    void changePaletteToPermanent(Palette* palette);
    void createPaletteTexture(Palette* palette);
    void createPaletteTexture(const std::string& layerName, Palette* palette);
    void deletePaletteTexture(Palette* palette, Palette::Type type);
    
    void loadScene3D(std::string name, std::string path);    
    void importScene3D(std::string name, std::string extension, std::string path);    
    void saveScene3D(std::string name, std::string path = "");
    void unloadScene3D(std::string name);
    void loadTopology(std::string name, std::string path = "");
    void saveTopology(std::string name, std::string path = "");
    void exportScene(std::string name, std::string extension, std::string path);
    void exportModel(std::string filePath, std::string extension, Model3D* model, const std::map<std::string, std::vector<uint32_t>>& segmentation, Camera* camera, bool exportCamera);
    void exportChiselProjectToUnity(std::string name, std::string path = "");
    
    std::string chiselName() const { return mCHISelName; }
    std::string defaultChiselPath() const { return mDefaultChiselPath; }
    std::string layerPath() const { return mCHISelPath + mStdPaths[LAYER]; }
    std::string palettePath() const { return mAppPath + mStdPaths[PALETTE]; }
    bool isChiselPathValid() const { return mCHISelPath.size() > 0 && mCHISelPath != mDefaultChiselPath; }
    
    void setDefaultChiselPath(std::string path);
    void loadChiselFile(std::string name, std::string path);
    void saveChiselProject(std::string name, std::string path = "");
    void unloadCHiselFile();
    
private:
    bool renameFile(std::string path, std::string oldName, std::string newName);
    bool copyFile(std::string source, std::string destination);
    std::vector<std::string> directoryFiles(std::string path) const;
    bool fileExists(std::string path, std::string name) const;
    bool directoryExists(std::string path) const;
    bool isDirectoryEmpty(std::string path) const;
    bool createDirectory(std::string path);
    bool clearDirectory(std::string path);
    bool deleteDirectory(std::string path);
    bool copyDirectory(std::string source, std::string destination, bool overwriteFiles);
    
    bool containTextureArray(GLenum targetKey, SamplerType samplerKey, int_fast64_t paramKey);
    
    GLenum getInternalFormat(Layer::Type type);
    SamplerType getSamplerType(GLenum internalFormat);
    std::array<GLenum, 2> getFormatAndType(GLenum internalFormat);

    GLConstants mGLConsts;
    
    Chisel* mChisel;
    Renderer* mRenderer;
    
    std::map<std::string, Shader* > mShaders;
    std::map<std::string, std::unique_ptr<Program>> mPrograms;
    std::vector<unsigned int> mFreeTexUnitList;
    std::vector<unsigned int> mFreeImageUnitList;
       
    std::unordered_map< std::string, std::unique_ptr<Texture> > mTextures;
    std::map< int_fast64_t, std::vector<Texture* > > mTextureArrays;

    std::map< GLenum, SamplerParamMap > mGLTexArrays;
    std::map< std::pair<GLenum, SamplerType>, std::vector<unsigned int> > mFreeTexArrayIndices;
    std::map< std::pair<GLenum, SamplerType>, std::vector<TextureArray*> > mBindedTexArrays;    
    std::vector< TextureArray* > mDirtyTextureArrays;
    std::map< std::pair<GLenum, SamplerType>, std::vector<GLint>> mTextureUnitPerSampler;
    std::set< std::pair<GLenum, SamplerType> > mDirtySampler;
    std::vector< TextureArray* > mBindedImageArrays;    
    std::vector< TextureArray* > mDirtyImageArrays;


    std::unordered_map<std::string, std::unique_ptr<Camera>> mCameras;
    std::unordered_map<std::string, std::unique_ptr<Light>> mLights;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mMeshes;
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
    std::unordered_map<std::string, std::unique_ptr<Model3D>> mModels;
    std::unordered_map<std::string, std::unique_ptr<Scene3D>> mScenes;
    std::unordered_map<std::string, std::unique_ptr<RenderPass>> mRenderPasses;
    std::unordered_map<std::string, std::unique_ptr<RenderTarget>> mRenderTargets;
    std::unordered_map<std::string, std::unique_ptr<RenderTechnique>> mRenderTechniques;
    
    enum FType : int
    {
        CHIS = 0,
        SCENE = 1,
        TOP = 2,
        DB = 3,
        LAYER = 4,
        PALETTE = 5,
        RESOURCES = 6
    };
    
    std::vector<std::string> mFileExtensions = { ".chl", ".scn", ".top", ".db", ".lay", ".plt", ".*" };
    std::vector<std::string> mStdPaths = { "", "", "", "", "layers/", "palettes/", "resources/" };
    
    std::string mAppPath;
    std::string mCHISelPath;
    std::string mDefaultChiselPath;
    std::string mCHISelName;
    float mVersion;
    unsigned int mLocalPaletteIndex;
    std::vector<std::unique_ptr<Layer>> mLayers;
    std::vector<std::unique_ptr<Palette>> mPalettes;
    std::vector<std::unique_ptr<Palette>> mLayerPalettes;
    
    std::unique_ptr<SQLiteDatabaseManager> mSQLiteDatabaseManager;
};

#endif // RESOURCEMANAGER_H
