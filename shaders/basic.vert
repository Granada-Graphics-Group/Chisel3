#version 430
#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 in_Posicion;
layout(location = 4) in vec4 in_Color;

out vec4 vertexColor;
flat out float zPos;

layout(std140, binding = 0) uniform GlobalMatrices
{
    mat4 cameraToClipMatrix;
    mat4 worldToCameraMatrix;
};

layout(std140, binding = 1) uniform ModelMatrices
{
    mat4 modelMatrix[1000];
}arrayMat[3];

void main(void)
{
	vertexColor = in_Color;
	zPos = in_Posicion.z;

	int index1 = int((gl_BaseInstanceARB + gl_InstanceID) / 1000);
	int index2 = int(mod(gl_BaseInstanceARB + gl_InstanceID, 1000));

	//gl_Position = cameraToClipMatrix * worldToCameraMatrix * vec4(in_Posicion, 1.0);
	gl_Position = cameraToClipMatrix * worldToCameraMatrix * arrayMat[index1].modelMatrix[index2] * vec4(in_Posicion, 1.0);
}