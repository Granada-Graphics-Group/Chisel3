#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;
flat in vec4 EdgeNormal;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    uvec2 brushMaskIndices;
    uvec2 areaIndices;
    uvec2 lockIndices;
    uvec2 neighborhoodIndices;
};

layout(location = 0) out vec4 FragCoords;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{    
    ivec3 layerSize = textureSize(ArrayTexture[brushMaskIndices.x], 0);
    float inside = texture(ArrayTexture[paintIndices.x], vec3(CoordST, paintIndices.y)).x;
    float inside2 = texelFetch(ArrayTexture[paintIndices.x], ivec3(int(CoordST.x * layerSize.x), int(CoordST.y * layerSize.y), paintIndices.y), 0).x;
    
    if(inside2 != 0.0)
        FragCoords = vec4(int(CoordST.x * layerSize.x), int(CoordST.y * layerSize.y), EdgeNormal.x, EdgeNormal.y);
    else
    {
        vec2 newCoordST = vec2(int(CoordST.x * layerSize.x), int(CoordST.y * layerSize.y)) + round(-EdgeNormal.zw);
        FragCoords = vec4(newCoordST.xy, EdgeNormal.xy);
    }
}
