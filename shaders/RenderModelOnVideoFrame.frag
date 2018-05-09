#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform float ContrastThreshold;
uniform sampler2D ModelMatte;
uniform sampler2D ColorBuffer0;
uniform sampler2D ColorBuffer3;

void main()
{
  float RenderChoices = texture(ModelMatte, CoordTex).r;
  if (RenderChoices > 0.5)
    FragColor = texture(ColorBuffer0, CoordTex) + (ContrastThreshold - 1) / ContrastThreshold;
  else
    FragColor = texture(ColorBuffer3, CoordTex);
}