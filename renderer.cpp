#include "renderer.h"
#include "glrenderer.h"

#include <cstdint>

Renderer::Renderer(RendererType type, int windowWidth, int windowHeight, ResourceManager* manager)
{
    switch(type)
    {
        case RendererType::GL:
        {
            mGLRenderer = std::make_unique<GLRenderer>(windowWidth, windowHeight, manager);
            mRender = [this](){this->mGLRenderer->render();};
            mResize = [this](int width, int height){ this->mGLRenderer->resize(width, height); };
            mManager = [this](){return this->mGLRenderer->resourceManager();};
            mSetManager = [this](ResourceManager* manager){this->mGLRenderer->setResourceManager(manager); };
            mSetLayer = [this](unsigned int index){this->mGLRenderer->setCurrentLayer(index);};
            mChangeLayerPosition = [this](int previous, int current){ this->mGLRenderer->changeLayerPosition(previous, current);};
            mInit = [this](int defaultFB){this->mGLRenderer->init(defaultFB);};
            mOnMouseButtons = [this](int button, int action, int mods, double xPos, double yPos){ this->mGLRenderer->onMouseButtons(button, action, mods, xPos, yPos); };
            mOnMousePosition = [this](double xPos, double yPos){ this->mGLRenderer->onMousePosition(xPos, yPos); };
            mOnMouseWheel = [this](double xOffset, double yOffset){ this->mGLRenderer->onMouseWheel(xOffset, yOffset); };
            mRenderMode = [this](){ this->mGLRenderer->setRenderMode(); };
            mMarkMode = [this](bool active){ this->mGLRenderer->setMarkMode(active); };
            mEraseMode = [this](bool erase){ this->mGLRenderer->setEraseMode(erase); };
            mReadMode = [this](bool read){ this->mGLRenderer->setReadMode(read); };
            mPickMode = [this](bool pick){ this->mGLRenderer->setPickMode(pick); };
            mSliceMode = [this](bool slice){ this->mGLRenderer->setSliceMode(slice); };
            mClearSlice = [this]() { this->mGLRenderer->clearSlice(); };
            mAlignMainCameraToModel = [this](){ this->mGLRenderer->alignMainCameraToModel(); };
            mAlignCameraToModel = [this](Camera* camera, Model3D* model) { this->mGLRenderer->alignCameraToModel(camera, model); };
            mInsertMarkTool = [this](){ this->mGLRenderer->insertMarkTool(); };
            mRemoveMarkTool = [this](){ this->mGLRenderer->removeMarkTool(); };
            mUpdateMarkToolPosition = [this](int x, int y){ this->mGLRenderer->updateMarkToolPosition(x, y); };
            mUpdateMarkToolRadius = [this](float radius){ this->mGLRenderer->updateMarkToolRadius(radius); };            
        }
    }
}


Renderer::~Renderer()
{

}

//*** Public Methods ***//

inline GLRenderer* Renderer::GL()
{
    return mGLRenderer.get();
}

void Renderer::init(unsigned int defaultFB)
{
    mInit(defaultFB);
}


void Renderer::render()
{
    mRender();
}

void Renderer::resize(int width, int height)
{
    mResize(width, height);
}


ResourceManager* Renderer::resourceManager()
{
    return mManager();
}

int Renderer::windowHeight()
{
    return mGLRenderer->windowHeight();
}

int Renderer::windowWitdh()
{
    return mGLRenderer->windowWitdh();
}

std::array<int, 2> Renderer::windowDimensions()
{
    return mGLRenderer->windowDimensions();
}

bool Renderer::isRenderModeActive()
{
    return mGLRenderer->isRenderModeActive();
}

bool Renderer::isMarkModeActive()
{
    return mGLRenderer->isMarkModeActive();
}

bool Renderer::isEraseModeActive()
{
    return mGLRenderer->isEraseModeActive();
}

bool Renderer::isPickModeActive()
{
    return mGLRenderer->isPickModeActive();
}

bool Renderer::isReadModeActive()
{ 
    return mGLRenderer->isReadModeActive();     
}

bool Renderer::isSliceModeActive()
{
    return mGLRenderer->isSliceModeActive();
}

bool Renderer::isResized()
{
    return mGLRenderer->isResized();
}

float Renderer::markToolRadius()
{
    return mGLRenderer->markToolRadius();
}

void Renderer::updateSizeDependentResources()
{
    return mGLRenderer->updateSizeDependentResources();
}

void Renderer::setRenderMode()
{
    mRenderMode();
}

void Renderer::setMarkMode(bool mark)
{
    mMarkMode(mark);
}

void Renderer::setEraseMode(bool erase)
{
    mEraseMode(erase);
}


void Renderer::setReadMode(bool read)
{
    mReadMode(read);
}

void Renderer::setPickMode(bool pick)
{
    mPickMode(pick);
}

void Renderer::setSliceMode(bool slice)
{
    mSliceMode(slice);
}

void Renderer::clearSlice()
{
    mClearSlice();
}

void Renderer::insertMarkTool()
{
    mInsertMarkTool();
}

void Renderer::removeMarkTool()
{
    mRemoveMarkTool();
}

void Renderer::updateMarkToolPosition(int x, int y)
{
    mUpdateMarkToolPosition(x, y);
}

void Renderer::updateMarkToolRadius(float radius)
{
    mUpdateMarkToolRadius(radius);
}

void Renderer::setCurrentPaintingValue(float value)
{
    mGLRenderer->setCurrentPaintingValue(value);
}

void Renderer::updateControlPointText(const std::vector<std::tuple<std::string, int, int>>& data)
{
    mGLRenderer->updateControlPointText(data);
}

void Renderer::updatePaletteTexture(unsigned int index, const std::array<float, 4096 * 4>& data, std::pair<float, float> range)
{
    mGLRenderer->updatePaletteTexture(index, data, range);
}

void Renderer::updateCurrentPaletteTexture(const std::array< float, int(16384) >& data, std::pair< float, float > range)
{
    mGLRenderer->updateCurrentPaletteTexture(data, range);
}

Scene3D* Renderer::scene()
{
    return mGLRenderer->scene();
}

Mesh * Renderer::mesh()
{
    return mGLRenderer->meshScene();
}

glm::vec4 Renderer::backgroundColor() const
{
    return mGLRenderer->clearColor();
}

glm::vec4 Renderer::defaultModelColor() const
{
    return mGLRenderer->defaultModelColor();
}

glm::vec3 Renderer::specularColor() const
{
    return mGLRenderer->specularColor();
}

float Renderer::specularPower() const
{
    return mGLRenderer->specularPower();
}

void Renderer::setChisel(Chisel* chisel)
{
    mGLRenderer->setChisel(chisel);
}

void Renderer::changeLayerPosition(int previous, int current)
{
    mChangeLayerPosition(previous, current);
}


void Renderer::setResourceManager(ResourceManager* manager)
{
    mSetManager(manager);
}

void Renderer::setCurrentLayer(unsigned int index)
{
    mSetLayer(index);
}

void Renderer::insertLayer(unsigned int index, Texture* data, Texture* mask, Texture* palette)
{
    mGLRenderer->insertLayer(index, data, mask, palette);
}

void Renderer::setCurrentPaintTextures(Texture* color, Texture* data, Texture* palette)
{
    mGLRenderer->setCurrentPaintTextures(color, data, palette);
}

void Renderer::setCurrentPaletteTexture(Texture* palette)
{
    mGLRenderer->setCurrentPaletteTexture(palette);
}

void Renderer::eraseLayer(unsigned int layerIndex)
{
    mGLRenderer->eraseLayer(layerIndex);
}


void Renderer::setOpacity(unsigned int layerIndex, float opacity)
{
    mGLRenderer->setOpacity(layerIndex, opacity);
}

void Renderer::computeExpression(const std::vector<std::string>& expression)
{
    mGLRenderer->computeExpression(expression);
}

void Renderer::computeShader(Program* shader, const std::vector<glm::byte>& uniformData)
{
    mGLRenderer->computeShader(shader, uniformData);
}

void Renderer::loadChiselScene(Scene3D* scene)
{
    mGLRenderer->loadChiselScene(scene);
}

void Renderer::swapChiselScene(Scene3D* scene)
{
    mGLRenderer->swapChiselscene(scene);
}

void Renderer::removeScene(Scene3D* scene)
{
    mGLRenderer->removeScene(scene);
}

void Renderer::toggleVertexColorVisibility(bool checked)
{
    mGLRenderer->toggleVertexColorVisibility(checked);
}

void Renderer::setDefaultModelColor(glm::vec4 color)
{
    mGLRenderer->setDefaultModelColor(color);
}

void Renderer::setSpecularColor(glm::vec3 color)
{
    mGLRenderer->setSpecularColor(color);
}

void Renderer::setSpecularPower(float power)
{
    mGLRenderer->setSpecularPower(power);
}

void Renderer::setBackgroundColor(glm::vec4 color)
{
	mGLRenderer->setClearColor(color);
}

void Renderer::alignMainCameraToModel()
{
	mAlignMainCameraToModel();
}

void Renderer::alignCameraToModel(Camera * camera, Model3D * model)
{
    mAlignCameraToModel(camera, model);
}

std::vector<glm::byte> Renderer::readLayerData(unsigned int layerIndex)
{
    return mGLRenderer->readLayerData(layerIndex);
}

std::vector<glm::byte> Renderer::readLayerMask(unsigned int layerIndex)
{
    return mGLRenderer->readLayerMask(layerIndex);
}

std::vector<glm::byte> Renderer::readTexture(Texture* texture)
{
    return mGLRenderer->readTexture(texture);
}

std::vector<char> Renderer::readCharTexture(Texture* texture)
{
    return mGLRenderer->readCharTexture(texture);
}


std::vector<uint16_t> Renderer::readUShortTexture(Texture* texture)
{
    return mGLRenderer->readUShortTexture(texture);
}

std::vector<int16_t> Renderer::readShortTexture(Texture* texture)
{
    return mGLRenderer->readShortTexture(texture);
}

std::vector<uint32_t> Renderer::readUIntTexture(Texture* texture)
{
    return mGLRenderer->readUIntTexture(texture);
}

std::vector<int32_t> Renderer::readIntTexture(Texture* texture)
{
    return mGLRenderer->readIntTexture(texture);
}

std::vector<float> Renderer::readFloatTexture(Texture* texture)
{
    return mGLRenderer->readFloatTexture(texture);
}

void Renderer::sliceModelWithPlane()
{
    mGLRenderer->sliceModelWithPlane();
}

void Renderer::onMouseButtons(int button, int action, int mods, double xPos, double yPos)
{
    mOnMouseButtons(button, action, mods, xPos, yPos);
}

void Renderer::onMousePosition(double xPos, double yPos)
{
    mOnMousePosition(xPos, yPos);
}

void Renderer::onMouseWheel(double xOffset, double yOffset)
{
    mOnMouseWheel(xOffset, yOffset);
}
