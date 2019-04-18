// #version 450
// #extension GL_ARB_compute_variable_group_size : enable
// 
// struct LayerData
// {
//     uvec2 layer;
//     uvec2 layerMask;
//     uvec2 palette;
//     vec2 paletteRange;
//     float opacity;
//     uint samplerType;
// };
// 
// layout(std140, binding = 12) uniform PerMaterialData
// {
//     float value;
//     uint index;
//     uint layerCount;
//     bool textureVisibility;
//     vec4 defaultColor;
//     vec4 specularAlbedoAndPower;    
//     LayerData layers[10];
// };
// 
// layout(local_size_variable) in;
// 
// layout(r32f, binding = 0) uniform image2DArray ValueArray;
// layout(r8, binding = 1) uniform image2DArray MaskArray;
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
    uvec2 brushMaskIndices;
};

layout(std140, binding = 11) uniform TransientData
{
    uvec2 copyIndices;
    uvec2 maskIndices;
};

layout(location = 0) out float FragValue;
layout(location = 1) out float FragMask;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{
    float seamMask = texture(ArrayTexture[seamMaskIndices.x], vec3(CoordST, seamMaskIndices.y)).x;
    ivec3 layerSize = textureSize(ArrayTexture[seamMaskIndices.x], 0);
    
    int count = 0;
    float value = 0;

    if(seamMask != 0.0)
    {
        for(int y = 1; y >= -1; y--)
            for(int x = -1; x <= 1; x++)
            {
                vec2 offsetCoord = CoordST + vec2(float(x)/layerSize.x, float(y)/layerSize.y);
                                        
                if(texture(ArrayTexture[maskIndices.x], vec3(offsetCoord, maskIndices.y)).x != 0.0 && count == 0)
                {
                    value = texture(ArrayTexture[copyIndices.x], vec3(offsetCoord, copyIndices.y)).x;
                    count = count + 1;
                }
            }

        if(count > 0)
        {
            FragValue = value;
            FragMask = 1.0;
        }
        else
            discard;
    }
    else
        discard;    
}
