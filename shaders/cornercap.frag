#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 outlineMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    uvec2 brushMaskIndices;
    uvec2 areaIndices;
    uvec2 lockIndices;
    uvec2 neighborhoodIndices;    
};

layout(location = 0) out vec2 FragColor;

uniform sampler2DArray ArrayTexture[13];

void main()
{   
    ivec3 layerSize = textureSize(ArrayTexture[paintIndices.x], 0);
    float outlineMask = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy, outlineMaskIndices.y)).x;
    vec2 neighbor = texture(ArrayTexture[neighborhoodIndices.x], vec3(CoordST.xy, neighborhoodIndices.y));
    
    if(outlineMask != 0.0 && neighbor.x == 0.0)
    {
        bool written = false;

        ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};
        
        for(int i = 0; i < 8; ++i)
        {
            float outlineMaskOffset = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), outlineMaskIndices.y)).x;
            vec2 neighborOffset = texture(ArrayTexture[neighborhoodIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), neighborhoodIndices.y));
                    
            if(!written && outlineMaskOffset != 0.0 && neighborOffset.x != 0.0)
            {
                written = true;
                FragCoords = neighborOffset;                    
            }
        }
            
        if(!written) discard;
    }
    else
        discard;
}
