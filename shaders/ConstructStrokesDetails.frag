#version 330

smooth in vec2 CoordTex;
in float StrokeTexLength;
in float StrokeTexWidth;
in int RenderStyle;

out vec4 FragColor;

uniform sampler2D RenderInfo;
uniform sampler2D MipRendering;
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
  vec4 finalColor;
  
  finalColor.rg = DetermineStrokeDetail(CoordTex);
  finalColor.b = texture(RenderInfo, CoordTex).b;
  finalColor.a = texture(MipRendering, CoordTex).a; //the alpha value of the rendering of previous frame
  
  FragColor = finalColor;
}