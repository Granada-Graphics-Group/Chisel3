#version 330

smooth in vec4 Color;
smooth in vec2 CGTexCoord;
smooth in vec2 GTexCoord;
smooth in vec2 TexCoord;
in float Type;

out vec4 FragColor;

uniform float GradientLargeThreshold;
uniform float GradientMediumThreshold;
uniform bool OnClipping;
uniform float StrokeLength;

uniform sampler2D RenderInfo;
uniform sampler2D StrokeBuffer;


void main()
{ 	
  vec3 mask = texture(StrokeBuffer, TexCoord).rgb;
  
  vec4 color = Color * mask.y * vec4(1, 1, 1, mask.x);
  if (Type > 0.6 && OnClipping) //large stroke
  {
    //color = vec4(1,0,1,1);
    vec2 step = (CGTexCoord - GTexCoord) * 0.5 / StrokeLength;
    int i = 0;
    for (i = 0; i <= 2 * StrokeLength; i++)
    {
      //float size = StrokeBuffer.texture(GTexCoord + step * i, 0 ).b;
      vec2 texcoord = GTexCoord + step * i;
      float gradient = texture(RenderInfo, texcoord).b;
      
      //if encounter both the medium and small gradient during the searching
      if (gradient >= GradientLargeThreshold)
      {
	color = vec4(0,0,0,0);
	//color = vec4(1,0,0,1);
	break;
      }	
    }
  }
  else if (Type > 0.4 && OnClipping) //medium stroke
  {
    //color = vec4(1,0,0,1);
    vec2 step = (CGTexCoord - GTexCoord) * 0.5 / StrokeLength;
    int i = 0;
    for (i = 0; i <= 2 * StrokeLength; i++)
    {
      vec2 texcoord = GTexCoord + step * i;
      float gradient = texture(RenderInfo, texcoord).b;
      
      //if encounter small gradient during the searching
      if (gradient >= GradientMediumThreshold)
      {
	color = vec4(0,0,0,0);
	//color = vec4(0,0,1,1);
	break;
      }
    }
    //color = vec4(0,0,1,1);
  }
  
  FragColor = color;
  
  //return vec4(1,0,0,1);
}
