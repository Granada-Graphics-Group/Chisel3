#version 330
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

float PI = 3.1416;

smooth out vec2 TexCoord;
smooth out vec2 GTexCoord;
smooth out vec2 CGTexCoord;
smooth out vec4 Color;
out float Type;

uniform float DeltaX;
uniform float DeltaY;
uniform int NumParticleX;
uniform int NumParticleY;
uniform float ProbabilityMediumThreshold;
uniform int WindowWidth;
uniform int WindowHeight;

uniform sampler2D FlowBuffer0;
uniform sampler2D Probability;

vec2 g_positions[4] = vec2[]
(
  vec2( -1, 1),
  vec2( 1, 1),
  vec2( -1, -1),
  vec2( 1, -1)
);

vec2 g_texcoords[4] = vec2[]
( 
  vec2(0,1), 
  vec2(1,1),
  vec2(0,0),
  vec2(1,0)
);

float scaleMap = 0.32;
uniform mat4 g_mWorldViewProj0;
uniform mat4 g_mInvView;

void main()
{
  vec2 index;
  index.x = (gl_in[0].gl_Position.x * NumParticleX - NumParticleY * 0.5) * WindowWidth * scaleMap /NumParticleX;
  index.x = (gl_in[0].gl_Position.y * NumParticleY - NumParticleY * 0.5) * WindowWidth * scaleMap /NumParticleX;
  
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  vec2 opticalFlowValue = texture(FlowBuffer0, texcoord).rg * 2 - vec2(1, 1);
  float randomNumber = texture(Probability, texcoord).r;
  
  bool bDraw = true;
  float angle;
  
  if(abs(opticalFlowValue.x) < 0.0001 && abs(opticalFlowValue.y) > 0.0001)
  {
    angle = PI * 0.5;
    if(opticalFlowValue.y <= 0)
      angle = -angle;
  }
  else if (abs(opticalFlowValue.x) < 0.0001 && abs(opticalFlowValue.y) < 0.0001)
    bDraw = false;
  else
    angle = atan(-opticalFlowValue.y, opticalFlowValue.x);
  
  float sine, cosine;
  sine = sin(angle);
  cosine = cos(angle);
  
  vec3 position;
  
  Type = 1;
  CGTexCoord = texcoord;

  float ofLength = clamp(length(opticalFlowValue), 0.3, 1);

  float randomNumber0 = texture(Probability, texcoord - vec2( 0 , DeltaX * 2)).r;
  float randomNumber1 = texture(Probability, texcoord - vec2( DeltaX * 2, 0)).r;
  float randomNumber2 = texture(Probability, texcoord - vec2( - DeltaX * 2, 0)).r;
  Color = vec4(randomNumber0, randomNumber1, randomNumber2, 1);

  if (randomNumber < ProbabilityMediumThreshold * 0.7 && bDraw)
  {
    for(int i=0; i<4; i++)
    {	
      float x = g_positions[i].r * 4 * ofLength;
      float y = g_positions[i].g * 0.3;
      if ( i == 0 || i == 2 )
	y = y * 3;		

      
      //orient the stroke according to the gradient angle
      position.r = x * cosine - y * sine;
      position.g = x * sine + y * cosine;
      position.b = 0;

      vec2 pos = gl_in[0].gl_Position.xy + vec2(position.r * DeltaX, position.g * DeltaY);
      GTexCoord = vec2(pos.r, 1 - pos.g);

      position =  mat3(g_mInvView) * position + vec3(index, 0);
      gl_Position = g_mWorldViewProj0 * vec4(position,1.0);

      TexCoord = g_texcoords[i];
      EmitVertex();
    }
    EndPrimitive();
  }
}
