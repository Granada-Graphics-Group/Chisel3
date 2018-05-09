#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

layout(stream = 0) out vec4 Color;
layout(stream = 0) out vec4 Info;

uniform float AlphaLargeThreshold;
uniform float ContrastThreshold;
uniform float DeltaX;
uniform float DeltaY;
uniform float CurTime;
uniform float ProbabilityMediumThreshold;
uniform float ProbabilityLargeThreshold;
uniform int StrokeLength;
uniform int StrokeWidth;

uniform sampler2D ColorBuffer1;
uniform sampler2D ColorBuffer2;
uniform sampler2D ColorBuffer3;
uniform sampler2D Probability;
uniform sampler2D RenderInfo;
uniform sampler2D TempImage;

void main()
{
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  vec4 RenderInformation = texture(RenderInfo, texcoord);
  float size = RenderInformation.r;
  float strokeAngle = RenderInformation.a;
  //float alpha = RenderInformation.g;

  if (size > 0.9) //large stroke
  //if (abs(alpha - AlphaLargeThreshold) < 0.01) //large stroke
  {
    float prob = texture(Probability, texcoord + vec2(CurTime * 0.071, CurTime * 0.041)).r;		
    float prob1 = texture(Probability, texcoord + vec2(CurTime * 0.082, CurTime *0.03)).r;

    //jitter the position a little bit in order to fill in the blank, do not insert particle at exactly the same location each time
    gl_Position.x = gl_in[0].gl_Position.x + prob * DeltaX * 3; 
    gl_Position.y = gl_in[0].gl_Position.y + prob1 * DeltaY * 3; 

    vec3 c1 = texture(ColorBuffer1, texcoord).rgb;
    vec3 c2 = texture(ColorBuffer2, texcoord).rgb;
    vec3 c3 = texture(ColorBuffer3, texcoord).rgb;		

    //average the color a little bit over color values of the previous frames
    //Color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);
    Color.rgb = c2 * ContrastThreshold - (ContrastThreshold - 1);	
    Color.a = AlphaLargeThreshold * 0.5;
    //Color.rgb = vec3(0,0,1); //testing
    
    Info = vec4(size * StrokeLength, size * StrokeWidth, strokeAngle, 0.5);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		
    float Emptyness = texture(TempImage, texcoord).r;
		
    //when there are absolute emptiness, definitely add a stroke here
      if (Emptyness < ProbabilityLargeThreshold * 0.54) //0.07
      {
	if (texcoord.r < 0.9 && texcoord.g < 0.9 && texcoord.r > 0.1 && texcoord.g > 0.1)
	{
	  //Color.rgb = vec3(0,0,1);
	  //if(Prob < ProbabilityLargeThreshold * 1.5)
	  //Color.rgb = vec3(0,0,1); //blue
	  EmitStreamVertex(0);
	}
      }
      else if (Emptyness < ProbabilityLargeThreshold * 1.08) //when it is not really empty, add a new stroke probablisitcally
      {

	//Color.rgb = vec3(0,1,0);
	//Color.a = AlphaLargeThreshold - 0.1; //vec4(0,0,1,1);
	//Color.rgb = vec3(0,0,1);

	//if it is at the image boundary
	if (texcoord.r > 0.1 && texcoord.r < 0.9 && texcoord.g > 0.1 && texcoord.g < 0.9)
	{
	  if(prob < ProbabilityLargeThreshold )
	    EmitStreamVertex(0);
	}
	else
	{
	  //if not very empty, then add new strokes conditionally
	  if(prob < ProbabilityLargeThreshold * 0.3)
	    EmitStreamVertex(0);
	}			
      }
  }
  EndStreamPrimitive(0);
}
