#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

in vec4 inInfo[2];
in vec4 inColor[2];

layout(stream = 0) out vec4 Color;
layout(stream = 0) out vec4 Info;

uniform float ContrastThreshold;
uniform float FadeOutSpeedThreshold;
uniform float FadeInSpeedThreshold;
uniform float GradientLargeThreshold;
uniform float GradientMediumThreshold;
uniform float SizeMediumThreshold;
uniform float SizeSmallThreshold;
uniform int StrokeLength;

uniform sampler2D ColorBuffer1;
uniform sampler2D ColorBuffer2;
uniform sampler2D ColorBuffer3;
uniform sampler2D FlowBuffer0;
uniform sampler2D RenderInfo;

void main()
{
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  
  vec2 Displacement = texture(FlowBuffer0, texcoord).rg * 2 - vec2(1, 1);	
  
  vec2 DisplacedPos = gl_in[0].gl_Position.xy + vec2(Displacement.x, Displacement.y); //the new location of the updated stroke
  
  //discard the particle if it is out of the image boundary
  //discard the particle if the alpha value is too small
  if (min(DisplacedPos.r, DisplacedPos.g) >=0 && max(DisplacedPos.r, DisplacedPos.g) <= 1 && inColor[0].a > FadeOutSpeedThreshold)
  {
    vec2 newTexcoord = vec2(DisplacedPos.r, 1 - DisplacedPos.g); //the texture coordinate of the new stroke location
    
    vec4 RenderInformation = texture(RenderInfo, newTexcoord);
    float size = RenderInformation.r;
    float strokeGrad = RenderInformation.b;
    float strokeAngle = RenderInformation.a;
    float alpha = RenderInformation.g;
    
    Color.a = inColor[0].a;
    Info = inInfo[0];
    //update the position, size, angle, color (as well as alpha value) of the particle
    gl_Position = vec4(DisplacedPos, 0.0, 0.0);
    
    
    //the stroke is normal
    if (inInfo[0].a > 0.6)
    {
      if (abs(size * StrokeLength - inInfo[0].r) > 0.01) //the stroke size has changed
      {		
	Info.a = 0; //indicate the stroke is going to be deleted gradually
	Color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
      }
    }
    //the stroke is newly constructed
    else if (inInfo[0].a > 0.4)
    {
      //the upper bound is subject to change
      if (inColor[0].a > alpha - FadeInSpeedThreshold)
	Info.a = 1;
      else
	Color.a += FadeInSpeedThreshold;
      
    }
    //the stroke is to be deleted, decrease the alpha value
    else
    {
      //if the stroke has come back to the correct frequency, then treat it as a new stroke again
      if (strokeGrad >= GradientLargeThreshold - 0.001 && strokeGrad <= GradientMediumThreshold + 0.01)
	Info.a = 0.5;
      else
	Color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
    }
    
    
    Displacement = DisplacedPos - gl_in[0].gl_Position.xy;
    //if (strokeGrad.r >= GradientMediumThreshold || Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.01)
	//if the stroke size hasn't changed, stay in the right frequency
    if (abs(size * StrokeLength - inInfo[0].r) <= 0.01 && Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.04)
      //if (Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.01 && )
      Info.b = strokeAngle;
    else
    {
      if (abs(strokeAngle - Info.b) < 0.002)
	Info.b = strokeAngle;
      else
      {
	if (strokeAngle > Info.b)
	  Info.b = Info.b + 0.001;
	else
	  Info.b = Info.b - 0.001;
      }
    }
    
    vec3 c1 = texture(ColorBuffer1, newTexcoord).rgb;
    vec3 c2 = texture(ColorBuffer2, newTexcoord).rgb;
    vec3 c3 = texture(ColorBuffer3, newTexcoord).rgb;
    
    //average the color a little bit over color values of the previous frames		
    //Color.rgb = (c1 * 0.2 + c2 * 0.6 + c3 * 0.2) * ContrastThreshold - (ContrastThreshold - 1);
    Color.rgb = (0.3 * inColor[0].rgb + 0.7 * c2 ) * ContrastThreshold - (ContrastThreshold - 1);
    EmitStreamVertex(0);
  }
  EndStreamPrimitive(0);
}
