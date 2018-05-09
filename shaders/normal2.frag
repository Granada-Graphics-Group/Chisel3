#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

out vec4 FragColor;

uniform sampler2DArray ArrayTexture[13];

void main()
{
	vec4 color = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));
	
	if(color.a > 0.5)
	  FragColor = color;
	else
	  discard;
	
}
