#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

in vec4 inInfo[2];
in vec4 inColor[2];

layout(stream = 0) out vec4 Color;
layout(stream = 0) out vec4 Info;

uniform float GradientLargeThreshold;
uniform float GradientMediumThreshold;
uniform float SizeMediumThreshold;
uniform float SizeSmallThreshold;
uniform float ContrastThreshold;
uniform float FadeOutSpeedThreshold;
uniform float FadeInSpeedThreshold;
uniform int StrokeLength;

uniform sampler2D FlowBuffer0;
uniform sampler2D RenderInfo;
uniform sampler2D ColorBuffer1;
uniform sampler2D ColorBuffer2;
uniform sampler2D ColorBuffer3;

void main()
{	
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  
  //if background of a model, do not update the particle
  /*if (ModelMatte.texture(texcoord, 0 ).r < 0.5)
   *	{
   *		ParticleOutputStream.Append(input[0]);
   *		return;
   }*/
  
  //vec4 currentDifference = ColorBuffer0.texture(texcoord, 0 ) - ColorBuffer1.texture(texcoord, 0 );
  //vec4 GradientX = (ColorBuffer0.texture(texcoord + vec2( DeltaX,   0), 0) 
  //	              - ColorBuffer0.g fSampleLevel( samLinear, texcoord - vec2( DeltaX,   0), 0)
  //				  + ColorBuffer1.texture(texcoord + vec2( DeltaX,   0), 0) 
  //	              - ColorBuffer1.texture(texcoord - vec2( DeltaX,   0), 0)) * 0.5;
  
  //vec4 GradientY = (ColorBuffer0.texture(texcoord + vec2( 0,   DeltaY), 0) 
  //	              - ColorBuffer0.texture(texcoord - vec2( 0,   DeltaY), 0)
  //				  + ColorBuffer1.texture(texcoord + vec2( 0,   DeltaY), 0) 
  //	              - ColorBuffer1.texture(texcoord - vec2( 0,   DeltaY), 0)) * 0.5;
  
  ////float gX = (GradientX.r + GradientX.g + GradientX.b ) * 0.33;
  ////float gY = (GradientY.r + GradientY.g + GradientY.b ) * 0.33;
  //float gX = 0.299 * GradientX.r + 0.557 * GradientX.g + 0.144 * GradientX.b;
  //float gY = 0.299 * GradientY.r + 0.557 * GradientY.g + 0.144 * GradientY.b;
  
  //float gradientMagnitude = sqrt(gX * gX + gY * gY + Lambda);
  
  ////since the value stored is not the actual optical, therefore need to retrieve the actual value
  ////vec2 preVectorBlured = blur(CoordTex).rg * 2 - vec2(1,1);
  //vec2 preVectorBlured = vec2(0,0);
  
  
  //float preDotGradient = preVectorBlured.r * gX + preVectorBlured.g * gY;
  
  //vec2 Displacement;
  ////float CD = (currentDifference.r + currentDifference.g + currentDifference.b) / 3.0f;
  //float CD = currentDifference.r * 0.299 + currentDifference.g * 0.557 + currentDifference.b * 0.144;
  //Displacement.r = preVectorBlured.r + (CD + preDotGradient) * (gX / gradientMagnitude);
  //Displacement.g = preVectorBlured.g + (CD + preDotGradient) * (gY / gradientMagnitude);
  
  vec2 Displacement = texture(FlowBuffer0, texcoord).rg * 2 - vec2(1, 1);
  /*if (Displacement.r > 0.01)
   *		Displacement.r = 0.01;
   *	else if(Displacement.r < -0.01)
   *		Displacement.r = -0.01;
   * 
   *	if (Displacement.g > 0.01)
   *		Displacement.g = 0.01;
   *	else if(Displacement.g < -0.01)
   *		Displacement.g = -0.01;*/
  //loading the optical flow value from disk files
  //vec2 Displacement = OpticalFlowTexture.texture(texcoord, 0).rg * 2 - vec2(1, 1);
  //Displacement = vec2(0.0, 0.0); //testing
  vec2 DisplacedPos = gl_in[0].gl_Position.xy + vec2(Displacement.x, Displacement.y); //the new location of the updated stroke
  
  if (min(DisplacedPos.r, DisplacedPos.g) >=0 && max(DisplacedPos.r, DisplacedPos.g) <= 1 && inColor[0].a > FadeOutSpeedThreshold)
  {
    vec2 newTexcoord = vec2(DisplacedPos.r, 1 - DisplacedPos.g); //the texture coordinate of the new stroke location
    
    vec4 renderInformation = texture(RenderInfo, newTexcoord);
    float size = renderInformation.r;
    float strokeGrad = renderInformation.b;
    float strokeAngle = renderInformation.a;
    //float alpha = renderInformation.g;
    
    //10/17
    //float RenderAlpha = StrokeBuffer.texture(newTexcoord, 0 );
    
    //if the alpha at the target position is very large, do not put new stroke there.
    //if (RenderAlpha < 0.8)
    //{
      

      Color.a = inColor[0].a;
      Info = inInfo[0];
      //update the position, size, angle, color (as well as alpha value) of the particle
      gl_Position = vec4(DisplacedPos, 0.0, 0.0);
      
      
      //the stroke is normal
      if (inInfo[0].a > 0.6)
      {
	if (abs(size * StrokeLength - inInfo[0].r) > 0.01) //if the stroke size has changed
	{		
	  Info.a = 0; //indicate the stroke is going to be deleted gradually
	  Color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
	}
      }
      //the stroke is newly constructed
      else if (inInfo[0].a > 0.4)
      {
	//the upper bound is subject to change
	if (inColor[0].a > 0.6)//alpha - FadeInSpeedThreshold
	  Info.a = 1;
	else
	  Color.a += FadeInSpeedThreshold;
	
      }
      //if the stroke is determined to be deleted
      else
      {
	//if the stroke has come back to the correct frequency, then treat it as a new stroke again
	if (strokeGrad < GradientLargeThreshold)
	  Info.a = 0.5;
	else
	  Color.a -= FadeOutSpeedThreshold; //gradually fade the stroke out
      }
      
      Displacement = DisplacedPos - gl_in[0].gl_Position.xy;
      //if (strokeGrad.r >= GradientMediumThreshold || Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.01)
      if (Displacement.r * Displacement.r + Displacement.g * Displacement.g >= 0.06)
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
      //Color.rgb = c2  * ContrastThreshold - (ContrastThreshold - 1);
      EmitStreamVertex(0);
      //}
  }	
  EndStreamPrimitive(0);
}