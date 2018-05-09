#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;

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
};

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor2;
layout(location = 2) out vec4 FragColor3;

uniform sampler2DArray ArrayTexture[13];
uniform sampler2D PPTextures[4];

layout(binding=0, rgba8) uniform image2D LayerImage;

void main()
{
    ivec2 imageCoord = ivec2(floor(CoordST.x * 2048), floor(CoordST.y * 2048));
    //if(gl_FragCoord.x > 400 && gl_FragCoord.x < 500 && gl_FragCoord.y > 400 && gl_FragCoord.y < 500)
    vec2 coords = vec2(gl_FragCoord.x/1280.0, gl_FragCoord.y/720.0);
    float depth = texture(PPTextures[3], coords).x;
    
    if(depth < 1.0)
    {
        memoryBarrier();
        imageStore(LayerImage, imageCoord, texture(PPTextures[2], coords));
        memoryBarrier();
    }
    else
    {
        memoryBarrier();			
        imageStore(LayerImage, imageCoord, texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y)));
        //imageStore(LayerImage, imageCoord, texture(PPTextures[2], CoordST));
        memoryBarrier();
    }
    
    FragColor = vec4(0.0,1.0, 0.0, 1.0);
}