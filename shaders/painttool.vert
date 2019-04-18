#version 450
#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 in_Posicion;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_CoordTextura;
layout(location = 3) in ivec2 inTexArrayIndex;

smooth out vec2 CoordST;
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
        TexArrayIndex = inTexArrayIndex;
        
        int index1 = int((gl_BaseInstanceARB + gl_InstanceID) / 1000);
        int index2 = int(mod(gl_BaseInstanceARB + gl_InstanceID, 1000));

        gl_Position = cameraToClipMatrix * worldToCameraMatrix * arrayMat[index1].modelMatrix[index2] * vec4(in_Posicion, 1.0);
}
