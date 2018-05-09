#version 430
#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 in_Posicion;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_CoordTextura;
layout(location = 3) in ivec2 inTexArrayIndex;

smooth out vec2 CoordST;
flat out ivec2 TexArrayIndex;


uniform mat4 ModeloVista;
uniform mat4 Proyeccion;

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
	CoordST = in_CoordTextura;
	TexArrayIndex = inTexArrayIndex;
	
	int index1 = int((gl_BaseInstanceARB + gl_InstanceID) / 1000);
	int index2 = int(mod(gl_BaseInstanceARB + gl_InstanceID, 1000));

	mat4 modelView = worldToCameraMatrix * arrayMat[index1].modelMatrix[index2];
	
	float xScale = length(vec3(modelView[0]));
	float yScale = length(vec3(modelView[1]));
	float zScale = length(vec3(modelView[2]));
	
	mat4 billboardMat = mat4(1.0);
	billboardMat[0][0] = xScale;
	billboardMat[1][1] = yScale;
	billboardMat[2][2] = zScale;
	billboardMat[3][0] = modelView[3][0];
	billboardMat[3][1] = modelView[3][1];
	billboardMat[3][2] = modelView[3][2];
	
	
	gl_Position = cameraToClipMatrix * billboardMat * vec4(in_Posicion, 1.0);
}