#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "const.h"
#include "camera.h"
#include "resourcemanager.h"
#include "vertexarray.h"
#include "arraybufferobject.h"
#include "pixelbufferobject.h"
#include "fbobject.h"
#include "scene3d.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <half.hpp>
#include <map>
#include <deque>
#include <set>
#include <array>
#include <tuple>
#include <chrono>

class FontManager;
class Scene3D;
class Shader;
class Program;
class UniformBufferObject;
class RenderTechnique;
class ComputeJob;
class RenderTarget;
class RenderPass;
class TextureView;

class Chisel;

struct GLLayer
{
    GLLayer(Texture* value, Texture* mask, Texture* palette): GLLayer(value, mask, palette, 1.0, static_cast<unsigned int>(value->samplerType())){};
    GLLayer(Texture* value, Texture* mask, Texture* palette, float opacity, unsigned int samplerType): mValue(value), mMask(mask), mPalette(palette), mOpacity(opacity), mSamplerType(samplerType){};
    Texture* mValue = nullptr;
    Texture* mMask = nullptr;
    Texture* mPalette = nullptr;
    glm::vec2 mPaletteRange;
    float mOpacity;
    unsigned int mSamplerType;
};

class GLRenderer
{
public:
    
    enum class Mode : unsigned char
    {
        Render = 0,
        Mark = 1,
        Erase = 2,
        Pick = 3,
        Read = 4,
        Slice = 5,
    };

    enum class State : unsigned char
    {
        Rendering = 0,
        Marking = 1,
        Erasing = 2,
        Picking = 3,
        Reading = 4,
        Slicing = 5
    };
    
    GLRenderer(unsigned int windowWidth, unsigned int windowHeight, ResourceManager* manager);
    ~GLRenderer();
    
    void init(GLuint defaultFB = 0);

    int windowWitdh() { return mWindowWidth; }
    int windowHeight() { return mWindowHeight; }
    std::array<int, 2> windowDimensions() { return {mWindowWidth, mWindowHeight}; }

    bool isRenderModeActive() { return mMode == Mode::Render; }
    bool isMarkModeActive() { return mMode == Mode::Mark; }
    bool isEraseModeActive() { return mMode == Mode::Erase; }
    bool isPickModeActive() { return mMode == Mode::Pick; }
    bool isReadModeActive() { return mMode == Mode::Read; }
    bool isSliceModeActive() { return mMode == Mode::Slice; }
    bool isResized() { return mResized; }
    float markToolRadius();
    
    void setState(State state);
    
    void clear(bool color, bool depth, bool stencil);
    void clearColorBuffers();
    void clearDepthBuffer();
    void clearStencilBuffer();
    glm::vec4 clearColor() const { return mClearColor; }
    glm::vec4 defaultModelColor() const { return mDefaultModelColor; }
    glm::vec3 specularColor() const { return mSpecularColor; }
    float specularPower() const { return mSpecularPower; }
        
    void setClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void setClearColor(glm::vec4 color);
    void setAutoClear(bool clear);
    void setAutoClearColor(bool clear);
    void setAutoClearDepth(bool clear);
    void setAutoClearStencil(bool clear);
    
    void setViewportCamera(Camera* camera) { mCamera = camera; }
    void toggleMode(Mode mode) { (mMode != mode) ? mMode = mode : mMode = Mode::Render; }
    void setRenderMode();
    void setMarkMode(bool mark);// { (paint) ? mMode = Mode::Paint : mMode = Mode::Render; }
    void setEraseMode(bool erase);// { (erase) ? mMode = Mode::Erase : mMode = Mode::Render; }
    void setReadMode(bool read);// { (read) ? mMode = Mode::Read : mMode = Mode::Render; }
    void setPickMode(bool pick);// { (pick) ? mMode = Mode::Pick : mMode = Mode::Render; }
    void setSliceMode(bool slice);
    void alignMainCameraToModel();
    void alignCameraToModel(Camera *camera, Model3D * model);
    void insertMarkTool();
    void removeMarkTool();
    void updateMarkToolPosition(int x, int y);
    void updateMarkToolRadius(float radius);
    void toggleVertexColorVisibility(bool checked);
    void setDefaultModelColor(glm::vec4 color);
    void setSpecularColor(glm::vec3 color);
    void setSpecularPower(float power);
    void sliceModelWithPlane();
    
    void render();
    void resize(int width, int height);
    void onMouseButtons(int button, int action, int mods, double xPos, double yPos);
    void onMousePosition(double xPos, double yPos);
    void onMouseWheel(double xOffset, double yOffset);
    ResourceManager* resourceManager(){ return mManager; }

    void updateMultiDrawCommand(Scene3D* scene);
    void processComputeJobs();
    
    void generateMultiDrawCommand(Scene3D* scene);
    
    void updateTexArrayIndexBuffer();
    void updateInstanceMatrixBuffer();
    
    void setResourceManager(ResourceManager* manager){ mManager = manager; }
    void updateSizeDependentResources();
    void setCurrentLayer(unsigned int index);
    void changeLayerPosition(int previous, int current);
    void insertLayer(unsigned int index, Texture* data, Texture* mask, Texture* palette);
    void setCurrentPaintTextures(Texture* data, Texture* mask, Texture* palette);
    void updateTechniqueDataWithLayer(const GLLayer& layer);
    void setCurrentPaletteTexture(Texture* palette);
    void eraseLayer(unsigned int layerIndex);
    void setCurrentPaintingValue(float value);
    void updateControlPointText(const std::vector<std::tuple<std::string, int, int>>& data);
    void updatePaletteTexture(unsigned int index, const std::array<float, 4096 * 4>& data, std::pair<float, float> range);
    void updateCurrentPaletteTexture(const std::array<float, 4096 * 4>& data, std::pair<float, float> range);
    void updateCurrentPaletteRange(std::pair<float, float> range);
    void setOpacity(unsigned int layerIndex, float opacity);
    void computeExpression(const std::vector<std::string>& expression);
    void computeShader(Program* shader, const std::vector<glm::byte>& uniformData = {});
    
    std::vector<glm::byte> readTexture(Texture* texture);
    template<typename T>
    std::vector<T> readTexture(Texture* texture);
    std::vector<char> readCharTexture(Texture* texture);
    std::vector<uint16_t> readUShortTexture(Texture* texture);
    std::vector<int16_t> readShortTexture(Texture* texture);
    std::vector<uint32_t> readUIntTexture(Texture* texture);
    std::vector<int32_t> readIntTexture(Texture* texture);
    std::vector<half_float::half> readHalfFloatTexture(Texture* texture);
    std::vector<float> readFloatTexture(Texture* texture);
    std::vector<glm::byte> readLayerData(unsigned int layerIndex);
    std::vector<glm::byte> readLayerMask(unsigned int layerIndex);
    
    void insertTechnique(RenderTechnique* tech, int life = 0);
    void insertComputeJob(ComputeJob* tech, int life = 0);
    void insertScene(Scene3D* scene);
    void removeScene(Scene3D* scene);
    void swapScene(Scene3D* oldScene, Scene3D* newScene);
    void loadChiselScene(Scene3D* scene);
    void swapChiselscene(Scene3D* newScene);
    void setSceneDirty(int index);
        
    void invalidateVertexDataFrom(int index);
    void invalidateIndexDataFrom(int index);
    void invalidateInstanceDataFrom(int index);
    void invalidateDrawDataFrom(MeshOffsets offsets);
    void invalidateScenesFrom(unsigned int index);
    
    void setChisel(Chisel *chisel){ mChisel = chisel; }
   
    Scene3D* scene() { return mMainScene; }
    Mesh* meshScene() { return mScene->meshes()[0]; }
    
    glm::vec3 rayIntersectPlane(glm::vec2 position, float z, glm::mat4 projection, glm::mat4 view);
    void planeIntersectObject(Model3D* plane, Model3D* model);
    
    void useSliceVersionShaders(bool use);
    void clearSlice();

private:
    void update();
    void initGLBuffers();
    void initGLBuffers(unsigned int attribCount, unsigned int indexCount, unsigned int uniformCount);
    void updateGLBuffers();
    void resizeGLBuffers(unsigned int attribCount, unsigned int indexCount);
    void resizeInstanceBuffers(unsigned int instanceCount);
    void updateGLBuffer();
    void initUniformData();
    void initAppUniformData();
    void updateGenericUniformData();
    void updateSceneUniformData(Scene3D* scene, bool forceUpdate = false);
    void updateMaterialUniformData();
    void updateTransientUniformData(RenderTarget* target);
    void updateSlicePlaneUniformData();
    void updateMeshData(Scene3D* scene);

    Mode mMode = Mode::Render;
    State mState = State::Rendering;    
    
    int mWindowWidth = 0;
    int mWindowHeight = 0;
    glm::ivec2 mLayerSize {2048, 2048};
    bool mResized = false;
    
    bool mLeftButtonPressed = false;
    bool mRightButtonPressed = false;
    bool mMiddleButtonPressed = false;
    double mLastXPos = 0;
    double mLastYPos = 0;
        
    glm::vec4 mClearColor { 0.0, 0.0, 0.0, 1.0 };
    bool mAutoClear = true;
    bool mAutoClearColor = true;
    bool mAutoClearDepth = true;
    bool mAutoClearStencil = true;
    
    bool mDepthState = true;
    std::vector<bool> mBlendingStates;
            
    Camera* mCamera = nullptr;
    glm::vec3 mCameraInitPosition;
    std::unique_ptr<Camera> mOrthoCamera;
    std::unique_ptr<Camera> mNormOrthoCamera;
    bool mUpdateCamera = false;
    
    ResourceManager* mManager;
    FontManager* mFontManager;
    
    Scene3D* mScene = nullptr;
    Scene3D* mMainScene = nullptr;
    Scene3D* mPPScene = nullptr;
    Scene3D* mFontScene = nullptr;
    Scene3D* mSlicePlaneScene = nullptr;
    int mMainSceneIndex = -1;
    std::vector<Scene3D*> mScenes;
    std::set<int> mDirtyScenes;
    std::map<std::string, Shader* > mShaders;
    std::map<std::string, Program* > mPrograms;
    
    
    GLuint mVAO = 0;
    std::array<GLuint, 6> mVBO;//VS15 { {0} };
    
    size_t mBufferSize = 200000;
    size_t mUniformBufferSize = 1000; 
    
    std::vector<std::unique_ptr<VertexArray>> mVertexArrays;
    std::vector<std::unique_ptr<ArrayBufferObject>> mBuffers;
    std::vector<std::unique_ptr<PersistentBufferObject>> mPersistentBuffers;
    std::vector<std::unique_ptr<UniformBufferObject>> mUniformBuffers;
//     std::vector<std::unique_ptr<PixelBufferObject>> mPixelBuffers;
        
    std::vector<glm::byte> mVertexBufferCache;
    std::vector<glm::byte> mNormalBufferCache;
    std::vector<glm::byte> mUVBufferCache; 
    std::vector<GLint> mTexArrayIndexBufferCache;    
    std::vector<GLuint> mIndexBufferCache;
    std::vector<glm::byte> mColorBufferCache;    
    std::vector<glm::mat4> mMatBufferCache;
    std::vector<GLint> mSceneOverrideTexArrayIndexBufferCache;
    
    GLuint mDefaultFBIndex = 0;
    std::unique_ptr<FBObject> mFBO;    
    
    bool mDrawIndCommandsNeedUpdate = true;
    std::vector<DrawElementsIndirectCommand> mDrawIndirectCommands;
    std::map <std::string, std::vector<DrawElementsIndirectCommand>> mDrawIndirectCommands2;
    unsigned int mDrawIndCommandIndex = 0;
    std::map <std::string, unsigned int> mDrawIndCommandIndex2;
    unsigned int mIndexCount = 0;
    unsigned int mVertexCount = 0;
    unsigned int mInstanceOffset = 0;   
    
    Model3D* mPPQuad;
    
    std::vector<RenderTechnique *> mRenderQueue;
    std::vector<ComputeJob *> mComputeQueue;
    
    RenderTechnique* mDepthTech = nullptr;
    RenderTechnique* mSeamMaskTech = nullptr;
    RenderTechnique* mNormalTech = nullptr;
    RenderTechnique* mPaintTexTech = nullptr;
    RenderTechnique* mEraseTexTech = nullptr;
    RenderTechnique* mReadFBTech = nullptr;
    RenderTechnique* mProjTech = nullptr;
    RenderTechnique* mUITech = nullptr;
    RenderTechnique* mAreaPerPixelTech = nullptr;
    RenderTechnique* mViewTexTech = nullptr;
    RenderTechnique* mBrushShapeTech = nullptr;
    RenderTechnique* mImmediateNeighborsTech = nullptr;
    RenderTechnique* mNeighborsTech = nullptr;
    RenderTechnique* mCornerCapTech = nullptr;
    RenderTarget* mDepthTarget = nullptr;
    RenderTarget* mDepthTexTarget = nullptr;
    RenderTarget* mSeamMaskTarget = nullptr;
    RenderTarget* mPaintTexTarget = nullptr;
    RenderTarget* mDilationTarget = nullptr;
    RenderTarget* mReadFBTarget = nullptr;
    RenderTarget* mEraseTexTarget = nullptr;
    RenderTarget* mEraseDilationTarget = nullptr;
    RenderTarget* mBrushToolTarget = nullptr;
    RenderTarget* mProjTarget = nullptr;
    RenderTarget* mSlicePlaneTarget = nullptr;
    RenderTarget* mAreaPerPixelTarget = nullptr;
    RenderTarget* mImmediateNeighborsTarget = nullptr;
    RenderTarget* mNeighborsTarget = nullptr;
    RenderTarget* mCornerCapTarget = nullptr;
    RenderPass* mProjPass = nullptr;
    RenderPass* mPaintTexPass = nullptr;
    RenderPass* mDilationPass = nullptr;
    RenderPass* mReadFBPass = nullptr;
    RenderPass* mEraseTexPass = nullptr;
    
    Texture *mSeamMaskTexture = nullptr;
    Texture *mDepthTexTexture = nullptr;
    Texture *mBrushMaskTexture = nullptr;
    Texture *mCopyValueTexture = nullptr;
    Texture *mDilatedLayerTexture = nullptr;
    Texture *mReadFBTexture = nullptr;
    Texture *mReadFBTextureI = nullptr;
    Texture *mReadFBTextureUI = nullptr;
    Texture *mAreaPerPixelTexture = nullptr;
    Texture *mLockTexture = nullptr;
    Texture *mNeighborhoodTexture = nullptr;
    std::deque<GLLayer> mLayers;
    
    bool mPaintingDataNeedUpdate = false;
    bool mReadData = false;
    
    bool mSliced = false;
    glm::vec4 mPlanePoint {0.0, 0.0, 0.0, 1.0};
    glm::vec4 mPlaneNormal {0.0, 0.0, 1.0, 1.0};

    float mCurrentPaintingValue;
    unsigned int mCurrentLayerIndex = 0;
    unsigned int mLayerCount = 0;
    bool mVertexColorVisibility = true;
    glm::vec4 mDefaultModelColor {0.6, 0.6, 0.6, 1.0};
    glm::vec3 mSpecularColor {0.2};
    float mSpecularPower = 200;

    Chisel* mChisel = nullptr;
    
    Texture *mNewLayerTexture = nullptr;
    Texture *mPaintTex = nullptr;
    Texture *mCurrentPaletteTexture = nullptr;
    std::pair<float, float> mCurrentPaletteRange {0, 0}; 
    Texture *mBrushShapeTexture = nullptr;
    
    std::array<unsigned char, 2048 * 2048 * 4 * sizeof(float)> mPixelData;

    int frameCounter = 0;
    std::chrono::time_point<std::chrono::steady_clock> t_start;
};

#endif // GLRENDERER_H

