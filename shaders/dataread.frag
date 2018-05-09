#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;

// struct Light
// {
//     vec4 position;
//     vec4 direction; 
//     vec4 color;
// };
// 
// layout(std140, binding = 0) uniform PerFrameData
// {
//     mat4 cameraToClipMatrix;
//     mat4 worldToCameraMatrix;
//     mat4 projectorCameraMatrix;
//     mat4 paintTexCameraMatrix;
//     Light lights[5];
// };

struct LayerData
{
    uvec2 layer;
    uvec2 layerMask;
    uvec2 palette;
    vec2 paletteRange;
    float opacity;
    uint samplerType;    
};

layout(std140, binding = 12) uniform PerMaterialData
{
    float value;
    uint index;
    uint layerCount;
    bool textureVisibility;
    vec4 defaultColor;
    vec4 specularAlbedoAndPower;    
    LayerData layers[10];
};

layout(location = 0) out vec2 FragColor;

uniform sampler2DArray ArrayTexture[20];


void main()
{
    vec2 readData = vec2(0.0, 0.0);

// Multiple layers    
//     for(int i = 0; i < layerCount; ++i)
//     {
//         float value = texture(ArrayTexture[layers[i].layer.x], vec3(CoordST, layers[i].layer.y)).r;
//         float maskValue = texture(ArrayTexture[layers[i].layerMask.x], vec3(CoordST, layers[i].layerMask.y)).r;
//         
//         readData.x = (1 - maskValue) * readData.x + maskValue * value;
//         readData.y = (1 - maskValue) * readData.y + maskValue;
//     }

    readData.x = texture(ArrayTexture[layers[index].layer.x], vec3(CoordST, layers[index].layer.y)).r;
    readData.y = texture(ArrayTexture[layers[index].layerMask.x], vec3(CoordST, layers[index].layerMask.y)).r;
    
    FragColor = readData;
}
