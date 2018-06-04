#version 430
#extension GL_ARB_shader_draw_parameters : enable

layout(location = 0) in vec3 in_Posicion;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_CoordTextura;
layout(location = 3) in ivec2 inTexArrayIndex;
layout(location = 4) in vec4 in_Color;

smooth out vec2 CoordST;
flat out ivec2 TexArrayIndex;
smooth out vec3 Normal;
smooth out vec4 vColor;
smooth out vec3 LightPos;
smooth out vec3 View;
smooth out vec4 ProjTexCoord;
smooth out vec4 Pos3D;

smooth out vec3 Posic;
smooth out vec3 PlaneNormal;
smooth out vec3 PlanePoint;


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

layout(std140, binding = 10) uniform AppData
{
    mat4 paddle;
    vec4 planeNormal;    
    vec4 planePoint;
};


void main(void)
{
    CoordST = in_CoordTextura;
    TexArrayIndex = inTexArrayIndex;
    vColor = in_Color;
        
    int index1 = int((gl_BaseInstanceARB + gl_InstanceID) / 1000);
    int index2 = int(mod(gl_BaseInstanceARB + gl_InstanceID, 1000));

    //gl_Position = cameraToClipMatrix * worldToCameraMatrix * ModeloVista * vec4(in_Posicion, 1.0);
    //gl_Position = cameraToClipMatrix * worldToCameraMatrix * modelMatrix[gl_BaseInstanceARB + gl_InstanceID] * vec4(in_Posicion, 1.0);
    mat4 modelViewMatrix = worldToCameraMatrix * arrayMat[index1].modelMatrix[index2];                
    vec4 viewPosition = modelViewMatrix * vec4(in_Posicion, 1.0);
    vec4 mvlight = vec4(100.0, 100.0, 100.0, 1.0);
    
    Posic = (modelViewMatrix * vec4(in_Posicion, 1.0)).xyz;
    PlaneNormal = planeNormal.xyz;
    PlanePoint = (worldToCameraMatrix * planePoint).xyz;
        
    Normal = mat3(transpose(inverse(modelViewMatrix))) * in_Normal;        
    ProjTexCoord = projectorCameraMatrix * viewPosition;
    Pos3D = cameraToClipMatrix * viewPosition;
    LightPos = mvlight.xyz - viewPosition.xyz;
    View = -viewPosition.xyz;
        
    gl_Position = Pos3D;
}
