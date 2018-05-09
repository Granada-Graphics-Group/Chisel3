#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D ColorBuffer0;

void main()
{
  FragColor = texture(ColorBuffer0, CoordTex);
}
