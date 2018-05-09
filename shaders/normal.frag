#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

out vec4 FragColor;

uniform sampler2DArray ArrayTexture[13];

void main()
{
	FragColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, 1.0 - CoordST.y), TexArrayIndex.y));
	//FragColor.a = 1.0;
}
