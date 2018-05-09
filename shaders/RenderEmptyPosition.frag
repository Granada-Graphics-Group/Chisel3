#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D TempImage;

void main()
{
  float emptiness = texture(TempImage, CoordTex).r;
  if (emptiness < 0.14)
    FragColor = vec4(1,0,0,1);
  else
    FragColor = vec4(0,0,0,1);
}