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
    int Offset;
};

layout(std140, binding = 12) uniform PerMaterialData
{
    uint value;
};

layout(location = 0) out uint FragValue;
layout(location = 1) out float FragMask;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{
    float seamMask = texture(ArrayTexture[seamMaskIndices.x], vec3(CoordST, seamMaskIndices.y)).x;
    ivec3 layerSize = textureSize(ArrayTexture[brushMaskIndices.x], 0);

    int count = 0;

    if(seamMask != 0.0)
    {
        for(int y = Offset; y >= -Offset; y--)
            for(int x = -Offset; x <= Offset; x++)
            {
                vec2 offsetCoord = CoordST + vec2(float(x)/layerSize.x, float(y)/layerSize.y);
                                        
                if(texture(ArrayTexture[brushMaskIndices.x], vec3(offsetCoord, brushMaskIndices.y)).x != 0.0)
                    count = count + 1;
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
