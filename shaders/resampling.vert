#version 450
#extension GL_ARB_shader_draw_parameters : enable


layout(location = 0) in vec3 in_Posicion;
layout(location = 2) in vec2 in_CoordTextura;
layout(location = 4) in vec4 in_EdgeNormal;


smooth out vec2 CoordST;
flat out vec4 EdgeNormal;


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
    EdgeNormal = in_EdgeNormal;

    gl_Position = paintTexCameraMatrix * vec4(in_Posicion.xyz, 1.0);
}

