#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D RenderInfo;

void main()
{
  float g = texture(RenderInfo, CoordTex).b;
  
  /*if (0.5 > RenderInfo.texture(CoordTex, 0).r && 0.5 - RenderInfo.texture(CoordTex, 0).r < 0.0001 )
   *   r *eturn vec4(0,0,1,1);
   *   else if (RenderInfo.texture(CoordTex, 0).r> 0.5 && RenderInfo.texture(CoordTex, 0).r - 0.5 < 0.0001 )
   *      r e*turn vec4(0,1,0,1);
   *      else
   *	return vec4(0,0,0,1);*/
  
  
  //return RenderInfo.texture(CoordTex, 0);
  FragColor =  vec4(g,g,g,1);
  //return vec4( 1, 0,0, 1);
}