#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D TempImage;

void main()
{
  //float overlap = TempImage.texture(CoordTex, 0 ).a;
  float overlap = texture(TempImage, CoordTex).r;
  //return vec4(overlap, 0,0,1);
  if (overlap >= 0.4)
    FragColor = vec4(1,0,0,1);
  else
    FragColor = vec4(0,0,0,1);
  //return vec4(1,0,0,1);
}