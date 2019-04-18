#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;
smooth in vec4 EyePosition;

smooth in vec3 PlaneNormal;
smooth in vec3 PlanePoint;

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

layout(location = 0) out ivec2 FragColor;

layout(bindless_sampler) uniform;
uniform sampler2DArray ArrayTexture[20];
uniform isampler2DArray iArrayTexture[6];

void main()
{
    ivec2 readData = ivec2(0, 0);
    
    float distance = dot(normalize(PlaneNormal), EyePosition.xyz - PlanePoint);
    
    if(distance < 0) 
    {
        readData.x = texture(iArrayTexture[layers[index].layer.x], vec3(CoordST, layers[index].layer.y)).r;
        readData.y = int(texture(ArrayTexture[layers[index].layerMask.x], vec3(CoordST, layers[index].layerMask.y)).r);
    }
    
    FragColor = readData;
}
