#version 430
#extension GL_ARB_shader_draw_parameters : enable


layout(location = 0) in vec3 in_Posicion;
layout(location = 2) in vec2 in_CoordTextura;
layout(location = 3) in ivec2 inTexArrayIndex;
layout(location = 4) in vec4 in_Color;


smooth out vec2 CoordST;
smooth out vec3 PosST;
flat out vec4 Line;
flat out ivec2 TexArrayIndex;

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

layout(std140, binding = 1) uniform ModelMatrices
{
    mat4 modelMatrix[1000];
}arrayMat[3];



void main(void)
{
    CoordST = in_CoordTextura;
    PosST = in_Posicion.xyz;
    TexArrayIndex = inTexArrayIndex;
    Line = in_Color;

    gl_Position = paintTexCameraMatrix * vec4(in_Posicion.xyz, 1.0);
    //gl_Position = paintTexCameraMatrix * vec4(in_CoordTextura, 0.0, 1.0);
}
