#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

layout(stream = 0) out vec4 Color;
layout(stream = 0) out vec4 Info;

uniform float AlphaSmallThreshold;
uniform float ContrastThreshold;
uniform int StrokeLength;
uniform int StrokeWidth;

uniform sampler2D ColorBuffer2;
uniform sampler2D RenderInfo;

void main()
{
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  
  vec4 RenderInformation = texture(RenderInfo, texcoord);
  float size = RenderInformation.r;
  float strokeAngle = RenderInformation.a;
  float alpha = RenderInformation.g;
  
  if (abs(alpha - AlphaSmallThreshold) < 0.01) //the alpha value indicate the existence of stroke
  {
    gl_Position = gl_in[0].gl_Position;
    Color = vec4(texture(ColorBuffer2, texcoord).rgb, alpha)  * ContrastThreshold - (ContrastThreshold - 1);			
    Info = vec4(size * StrokeLength, size * StrokeWidth, strokeAngle, 1);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
    EmitStreamVertex(0);
  }
  EndStreamPrimitive(0);
}
