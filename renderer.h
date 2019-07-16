#ifndef DRENDERER_H
#define DRENDERER_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <half.hpp>
#include <vector>
#include <tuple>
#include <memory>
#include <functional>

enum class RendererType : int 
{
    GL,
    Vulkan
};

class GLRenderer;
class VulkanRenderer;
class ResourceManager;
class Texture;
class Program;
class Chisel;
class Scene3D;
class Camera;
class Model3D;
class Mesh;

class Renderer
{
public:
    Renderer(RendererType type, int windowWidth, int windowHeight, ResourceManager* manager);
    ~Renderer();
    GLRenderer* GL();
    void init(unsigned int defaultFB = 0);
    ResourceManager* resourceManager();

    int windowWitdh();
    int windowHeight();
    std::array<int, 2> windowDimensions();

    bool isRenderModeActive();
    bool isMarkModeActive();
    bool isEraseModeActive();
    bool isPickModeActive(); 
    bool isReadModeActive();
    bool isSliceModeActive();
    bool isResized();
    float markToolRadius();

    void render();
    void resize(int width, int height);
    
    void updateSizeDependentResources();
    void setRenderMode();
    void setMarkMode(bool mark);
    void setEraseMode(bool erase);
    void setReadMode(bool read);
    void setPickMode(bool pick);
    void setSliceMode(bool slice);
    void clearSlice();
    void insertMarkTool();
    void removeMarkTool();
    void updateMarkToolPosition(int x, int y);
    void updateMarkToolRadius(float radius);
    void setCurrentPaintingValue(float value);
    void updateControlPointText(const std::vector<std::tuple<std::string, int, int>>& data);
    void updatePaletteTexture(unsigned int index, const std::array<float, 4096 * 4>& data, std::pair<float, float> range);
    void updateCurrentPaletteTexture(const std::array<float, 4096 * 4>& data, std::pair<float, float> range);
    Scene3D* scene();
    Mesh* mesh();
    
    glm::vec4 backgroundColor() const;
    glm::vec4 defaultModelColor() const;
    glm::vec3 specularColor() const;
    float specularPower() const;

    void setChisel(Chisel* chisel);    
    void setResourceManager(ResourceManager* manager);
    void setCurrentLayer(unsigned int index);
    void changeLayerPosition(int previous, int current);
    void insertLayer(unsigned int index, Texture* data, Texture* mask, Texture* palette);
    void setCurrentPaintTextures(Texture* color, Texture* data, Texture* palette);
    void setCurrentPaletteTexture(Texture* palette);
    void eraseLayer(unsigned int layerIndex);
    void padLayerTextures(uint32_t layerIndex);
    void setOpacity(unsigned int layerIndex, float opacity);
    void computeExpression(const std::vector<std::string>& expression);
    void computeShader(Program* shader, const std::vector<glm::byte>& uniformData = {}, bool computeImmediately = true);
    
    void computeLayerOperation(uint32_t layerOperation, const std::vector<glm::byte>& uniformData = {});
    Texture* computeAreaPerTexelTexture(std::pair<int, int> resolution);
    Texture* computeTopologyTexture(std::pair<int, int> resolution);
    
    void loadChiselScene(Scene3D* scene);
    void swapChiselScene(Scene3D* scene);
    void removeScene(Scene3D* scene);
        
    void toggleVertexColorVisibility(bool checked);
    void setDefaultModelColor(glm::vec4 color);
    void setSpecularColor(glm::vec3 color);
    void setSpecularPower(float power);
    void setBackgroundColor(glm::vec4 color);
    void alignMainCameraToModel();
    void alignCameraToModel(Camera* camera, Model3D* model);

    std::vector<double> readLayerData(unsigned int layerIndex);
    std::vector<double> readLayerDataTexture(Texture* texture);
    std::vector<glm::byte> readLayerMask(unsigned int layerIndex);    
    std::vector<glm::byte> readTexture(Texture* texture);
    std::vector<char> readCharTexture(Texture* texture);
    std::vector<uint16_t> readUShortTexture(Texture* texture);
    std::vector<int16_t> readShortTexture(Texture* texture);
    std::vector<uint32_t> readUIntTexture(Texture* texture);
    std::vector<int32_t> readIntTexture(Texture* texture);
    std::vector<half_float::half> readHalfFloatTexture(Texture* texture);
    std::vector<float> readFloatTexture(Texture* texture);

    
    void sliceModelWithPlane();
    
    void onMouseButtons(int button, int action, int mods, double xPos, double yPos);
    void onMousePosition(double xPos, double yPos);
    void onMouseWheel(double xOffset, double yOffset);

private:
    std::unique_ptr<GLRenderer> mGLRenderer;
    
    std::function<void(unsigned int)> mInit;
    std::function<void()> mRender;
    std::function<void(int, int)> mResize;
    std::function<ResourceManager*()> mManager;
    std::function<void(ResourceManager*)> mSetManager;
    std::function<void(unsigned int)> mSetLayer;
    std::function<void(int, int)> mChangeLayerPosition;
    std::function<void(int, int , int, double, double)> mOnMouseButtons;
    std::function<void(double, double)> mOnMousePosition;
    std::function<void(double, double)> mOnMouseWheel;
    std::function<void()> mRenderMode;
    std::function<void(bool)> mMarkMode;
    std::function<void(bool)> mEraseMode;
    std::function<void(bool)> mReadMode;
    std::function<void(bool)> mPickMode;
    std::function<void(bool)> mSliceMode;
    std::function<void()> mClearSlice;
    std::function<void()> mAlignMainCameraToModel;
    std::function<void(Camera*, Model3D*)> mAlignCameraToModel;
    std::function<void()> mInsertMarkTool;
    std::function<void()> mRemoveMarkTool;
    std::function<void(int, int)> mUpdateMarkToolPosition;
    std::function<void(float)> mUpdateMarkToolRadius;
    
    std::function<void(uint32_t)> mPadLayerTextures;
    std::function<void(uint32_t)> mComputeLayerOperation;
    std::function<void(std::pair<int, int>)> mComputeAreaPerPixelTexture;
    //std::unique_ptr<VulkanRenderer> mVkRenderer;
};

#endif // DRENDERER_H
