#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D MipRendering;

void main()
{
  vec4 color = texture(MipRendering, CoordTex);
  if (color.a > 0.7 ) 
    FragColor = vec4(1,0,0,1);
  else FragColor = vec4(0,0,0,1);
  //return MipRendering.texture(CoordTex, 0 );
}