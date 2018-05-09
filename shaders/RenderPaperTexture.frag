#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D PaperTex;

void main()
{
  FragColor = vec4(1,1,0.9,texture(PaperTex, CoordTex).r * 0.2);
}