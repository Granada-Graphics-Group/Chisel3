#version 430

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

layout(location = 0) out uvec2 FragColor;

uniform sampler2DArray ArrayTexture[20];
uniform usampler2DArray uArrayTexture[6];


void main()
{
    uvec2 readData = uvec2(0, 0);
    
    float distance = dot(normalize(PlaneNormal), EyePosition.xyz - PlanePoint);
    
    if(distance < 0) 
    {
        readData.x = texture(uArrayTexture[layers[index].layer.x], vec3(CoordST, layers[index].layer.y)).r;
        readData.y = uint(texture(ArrayTexture[layers[index].layerMask.x], vec3(CoordST, layers[index].layerMask.y)).r);
    }
    
    FragColor = readData;
}
