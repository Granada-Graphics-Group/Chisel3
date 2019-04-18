#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;

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
    uvec2 neighborEdgeIndices;
};

layout(location = 0) out vec2 FragColor;
layout(location = 1) out vec2 FragTempColor;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{
    ivec3 layerSize = textureSize(ArrayTexture[paintIndices.x], 0);
    float outlineMask = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy, outlineMaskIndices.y)).x;
    vec4 neighborEdge = texture(ArrayTexture[neighborEdgeIndices.x], vec3(CoordST.xy, neighborEdgeIndices.y));
    
    if(outlineMask != 0.0)
    {
        if(neighborEdge.xy != vec2(0.0, 0.0))
        {
            FragColor = neighborEdge.xy;
            FragTempColor = neighborEdge.xy;
        }
        else
        {
            ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};            
            int count = 0;
            
            for(int i = 0; i < 8; ++i)
            {
                vec4 neighborEdgeOffset = texture(ArrayTexture[neighborEdgeIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), neighborEdgeIndices.y));
                float maskOffset = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), outlineMaskIndices.y)).x;
                        
                if(neighborEdgeOffset.xy != vec2(0.0, 0.0) && immediateOffsets[i] == round(-neighborEdgeOffset.zw) && maskOffset == 0.0)
                {   
                    count++;
                    FragColor = neighborEdgeOffset.xy;
                    FragTempColor = neighborEdgeOffset.xy;
                }
            }
            
            if(count == 0)
                discard;
        }
    }
    else
        discard;
}
