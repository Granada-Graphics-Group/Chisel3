#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
};

layout(location = 0) out vec2 FragColor;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{   
    ivec3 layerSize = textureSize(ArrayTexture[paintIndices.x], 0);
    float texel = texture(ArrayTexture[paintIndices.x], vec3(CoordST, paintIndices.y)).x;
    ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};
    
    if(texel != 0.0)
    {
        float immediateData = 0;
        
        for(int i = 0; i < 8; ++i)
        {
            float immediateNeighbor = texture(ArrayTexture[paintIndices.x], vec3(CoordST + immediateOffsets[i]/float(layerSize.x), paintIndices.y)).x;
            
            if(immediateNeighbor != 0.0)
                immediateData = uint(immediateData) | uint(1) << i;
        }
                
        FragColor = vec2(-immediateData, 0);
    }
    else
        discard;
}
