#version 330

float PI = 3.1416;

smooth in vec2 Blur3x3[9];
smooth in vec2 CoordTex;

out vec4 FragColor;

uniform float AngleThreshold;
uniform float ContrastThreshold;
uniform bool OnRealGradientAngle;
uniform int RenderStyle;
uniform int StrokeTexLength;
uniform int StrokeTexWidth;

uniform sampler2D ColorBuffer1;
uniform sampler2D StrokeTex;
uniform sampler2D AlphaMask;

vec2 DetermineStrokeDetail(vec2 texcoord)
{
  vec2 Result;
  //Calculate lighting factor and alpha mask value
  //TODO
  //float dx = 0.0149; //1 / stroke texture width
  //float dy = 0.0036; //1 / stroke texture height
  float dx = 1.0f / StrokeTexLength;
  float dy = 1.0f / StrokeTexWidth;
  vec3 v1, v2, v3, v4;
  float rr = texture(StrokeTex, vec2(texcoord.r + dx, texcoord.g     )).r;
  float ll = texture(StrokeTex, vec2(texcoord.r - dx, texcoord.g     )).r;
  float uu = texture(StrokeTex, vec2(texcoord.r,      texcoord.g + dy)).r;
  float bb = texture(StrokeTex, vec2(texcoord.r,      texcoord.g - dy)).r;
  
  Result.r = texture(AlphaMask, texcoord).r;
  
  v1 = vec3( 4 * dx, 0,      rr - ll);
  v2 = vec3( 0     , 4 * dy, uu - bb);
  vec3 normal = normalize(cross(v1,v2));
  float lightingFactor = abs(dot(normal, vec3(0,0,1)));
  
  if (RenderStyle == 0)
    lightingFactor = 1;
  Result.g = lightingFactor;
  return Result;
}

void main()
{
  vec4 color[9];
  
  for (int i = 0; i < 9; i++)
    color[i] = texture(ColorBuffer1, Blur3x3[i]);
  
  vec4 gX = color[0] - color[2]  + 2 * color[3] - 2 * color[5] + color[6] - color[8];
  vec4 gY = color[0] - color[6]  + 2 * color[1] - 2 * color[7] + color[2] - color[8];
  
  float gX1 = 0.299 * gX.r + 0.557 * gX.g + 0.144 * gX.b;
  float gY1 = 0.299 * gY.r + 0.557 * gY.g + 0.144 * gY.b;
  
  float gradient = sqrt(gX1 * gX1 + gY1 * gY1);

  float gradientAngle = 0;

  if (!OnRealGradientAngle && gradient< AngleThreshold)	
  {	
    //GradientAngle = 0;
    //GradientAngle = RGB_to_HSV(ColorBuffer1.texture(CoordTex, 0 )).r;
    gradientAngle = (ColorBuffer1, CoordTex).r;//CHANGE			
  }
  else
  {
    //finalColor.b = Gradient.y;	
  
    float y = sqrt(gY.r * gY.r);
    float x = sqrt(gX.r * gX.r);
  
    if (gX.r < 0) x = -x;
    if (gY.r > 0) y = -y;
  
    float angle;
    
    if (abs(x) < 0.001)
      angle = 0;
    else
      angle= atan(y, x);
  
    gradientAngle = ( angle / PI + 1 ) * 0.5;
  }

  vec2 detail = DetermineStrokeDetail(CoordTex);
  //return vec4(Gradient, Gradient, Gradient, GradientAngle);	
  FragColor = vec4(detail, gradient, gradientAngle);

//return vec4(0.6, 0.5, 0.2, 1);
//return ColorBuffer1.texture(CoordTex, 0 );
}