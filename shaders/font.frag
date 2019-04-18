#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

layout(location = 0) out vec4 FragColor;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{
    float font = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y)).r;
    FragColor = vec4(font);
}
