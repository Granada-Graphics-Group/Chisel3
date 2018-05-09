#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D ModelMatte;

void main()
{
  float RenderChoices = texture(ModelMatte, CoordTex).r;
  if (RenderChoices > 0.5)
    //return vec4(1,0,0,1);
    FragColor = vec4(1,0,0,0);
  else FragColor = vec4(0,0,0,1);
}