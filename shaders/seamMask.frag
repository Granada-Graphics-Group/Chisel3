#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

/*
struct Light
{
    vec4 position;
    vec4 direction; 
    vec4 color;
};

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 cameraToClipMatrix;
    mat4 worldToCameraMatrix;
    mat4 projectorCameraMatrix;
    mat4 paintTexCameraMatrix;
    Light lights[5];
};*/

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

    for(int x = -2; x <= 2; x++)
        for(int y = -2; y <= 2; y++)
        {
            vec2 offset = vec2(float(x)/texSize.x, float(y)/texSize.y);
            float depth = texture(ArrayTexture[paintIndices.x], vec3(CoordST, paintIndices.y)).x;
            float depthOffset = texture(ArrayTexture[paintIndices.x], vec3(CoordST + offset, paintIndices.y)).x;
            
            if(depth == 0.0 && depthOffset != 0.0)
                FragColor = vec4(1.0);
        }
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
