#version 450

layout(location = 2) in vec2 in_CoordTextura;

smooth out vec2 CoordST;

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
              
        gl_Position = paintTexCameraMatrix * vec4(in_CoordTextura, 0.0, 1.0);
}
