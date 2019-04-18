#version 450
#extension GL_ARB_bindless_texture : require

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
    uvec2 neighborEdgeIndices;
    uvec2 tempNeighborhoodIndices;
};

layout(location = 0) out vec2 FragColor;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{

// Lines
//     ivec3 layerSize = textureSize(ArrayTexture[paintIndices.x], 0);
//     float outlineMask = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy, outlineMaskIndices.y)).x;
//     vec4 preNeighbor = texture(ArrayTexture[pre.x], vec3(CoordST.xy, pre.y));
//     
//     if(outlineMask != 0.0)
//     {
//         if(preNeighbor.xy != vec2(0.0, 0.0))
//             FragColor = preNeighbor.xy;
//         else
//         {
//             ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};
//             
//             int cont = 0;
//             
//             for(int i = 0; i < 8; ++i)
//             {
//                 vec4 preOffset = texture(ArrayTexture[pre.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), pre.y));
//                 float maskOffset = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), outlineMaskIndices.y)).x;
//                         
//                 if(preOffset.xy != vec2(0.0, 0.0) && immediateOffsets[i] == round(-preOffset.zw) && maskOffset == 0.0)
//                 {   
//                     cont++;
//                     FragColor = vec2(cont, 0.6);
//                 }
//             }
//             
//             if(cont == 0)
//                 discard;
//         }
//     }
//     else
//         discard;

    ivec3 layerSize = textureSize(ArrayTexture[paintIndices.x], 0);
    float outlineMask = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy, outlineMaskIndices.y)).x;
    vec2 neighbor = texture(ArrayTexture[tempNeighborhoodIndices.x], vec3(CoordST.xy, tempNeighborhoodIndices.y)).xy;
    
    if(outlineMask != 0.0 && neighbor.x == 0.0)
    {
        bool written = false;

        ivec2 immediateOffsets[8] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, 1}, {-1, -1}, {1, 1}, {1, -1}};
        
        for(int i = 0; i < 8; ++i)
        {
            float outlineMaskOffset = texture(ArrayTexture[outlineMaskIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), outlineMaskIndices.y)).x;
            vec2 neighborOffset = texture(ArrayTexture[tempNeighborhoodIndices.x], vec3(CoordST.xy + immediateOffsets[i]/float(layerSize.x), tempNeighborhoodIndices.y)).xy;
                    
            if(!written && outlineMaskOffset != 0.0 && neighborOffset.x != 0.0)
            {
                written = true;
                FragColor = neighborOffset;                    
            }
        }
            
        if(!written) discard;
    }
    else
        discard;
}
