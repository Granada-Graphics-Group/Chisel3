#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 WorldVertex;
smooth in vec4 Pos3D;

// struct Light
// {
//     vec4 position;
//     vec4 direction; 
//     vec4 color;
// };
// 
// layout(std140, binding = 0) uniform PerFrameData
// {
//     mat4 cameraToClipMatrix;
//     mat4 worldToCameraMatrix;
//     mat4 projectorCameraMatrix;
//     mat4 paintTexCameraMatrix;
//     Light lights[5];
// };

layout(location = 0) out vec4 FragColor;


uniform sampler2DArray ArrayTexture[13];

void main()
{
//     vec4 baseColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));
//     FragColor = vec4(baseColor.xyz, 1.0);
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
