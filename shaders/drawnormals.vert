#version 450
#extension GL_ARB_shader_draw_parameters : enable

layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_CoordTextura;

smooth out vec3 Normal;


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
    Normal = in_Normal;

    gl_Position = paintTexCameraMatrix * vec4(in_CoordTextura, 0.0, 1.0);
}
