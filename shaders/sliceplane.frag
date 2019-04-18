#version 450

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 vColor;
smooth in vec4 WorldVertex;
smooth in vec4 Pos3D;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = vColor;
}
