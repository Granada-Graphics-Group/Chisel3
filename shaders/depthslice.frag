#version 430

smooth in vec2 CoordST;
smooth in vec4 EyePosition;

smooth in vec3 PlaneNormal;
smooth in vec3 PlanePoint;

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

layout(location = 0) out float FragColor;


uniform sampler2DArray ArrayTexture[20];

void main()
{
    float distance = dot(normalize(PlaneNormal), EyePosition.xyz - PlanePoint);
    
    if(distance < 0) 
        FragColor = EyePosition.z * 0.98;
    else
        discard;
        
}
