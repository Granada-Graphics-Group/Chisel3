#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;

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

layout(location = 0) out uvec2 FragColor;

layout(bindless_sampler) uniform;
uniform sampler2DArray ArrayTexture[20];
uniform usampler2DArray uArrayTexture[6];


void main()
{
    uvec2 readData = uvec2(0, 0);

    readData.x = texture(uArrayTexture[layers[index].layer.x], vec3(CoordST, layers[index].layer.y)).r;
    readData.y = uint(texture(ArrayTexture[layers[index].layerMask.x], vec3(CoordST, layers[index].layerMask.y)).r);

    
    FragColor = readData;
}
