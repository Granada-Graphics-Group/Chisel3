#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
};

layout(location = 0) out vec4 FragColor;

uniform sampler2DArray ArrayTexture[13];

void main()
{   
    ivec3 texSize = textureSize(ArrayTexture[paintIndices.x], 0);
    float depth = texture(ArrayTexture[paintIndices.x], vec3(CoordST, paintIndices.y)).x;

    if(depth == 0.0)
    {
        for(int x = -2; x <= 2; x++)
            for(int y = -2; y <= 2; y++)
            {
                vec2 offset = vec2(float(x)/texSize.x, float(y)/texSize.y);                
                float depthOffset = texture(ArrayTexture[paintIndices.x], vec3(CoordST + offset, paintIndices.y)).x;
                
                if(depthOffset != 0.0)
                    FragColor = vec4(1.0);
            }
    }
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
