#version 330
layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

float PI = 3.1416;

in vec4 inInfo[2];
in vec4 inColor[2];

smooth out vec2 TexCoord;
smooth out vec2 GTexCoord;
smooth out vec2 CGTexCoord;
smooth out vec4 Color;
out float Type;

uniform float DeltaX;
uniform float DeltaY;
uniform int NumParticleX;
uniform int NumParticleY;
uniform float SizeMediumThreshold;
uniform int StrokeLength;
uniform int WindowWidth;
uniform int WindowHeight;


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
  vec2 strokeSize = inInfo[0].rg;
  vec4 strokeColor = inColor[0];
  float strokeAngle = inInfo[0].b;

  vec2 index;
  index.x = (gl_in[0].gl_Position.x * NumParticleX - NumParticleY * 0.5) * WindowWidth * scaleMap / NumParticleX;
  index.y = (gl_in[0].gl_Position.y * NumParticleY - NumParticleY * 0.5) * WindowHeight * scaleMap / NumParticleY;
  vec3 position;
  float angle = (strokeAngle * 2 - 0.5) * PI;
  float sine = sin(angle);
  float cosine = cos(angle);
  
  vec2 texcoord = vec2(gl_in[0].gl_Position.x, 1 - gl_in[0].gl_Position.y);

  float pLength = strokeSize.x, nLength = strokeSize.y;

  Color = strokeColor;
  CGTexCoord = texcoord;
  Type = 0;
  
  if(abs(StrokeLength - inInfo[0].x) < 0.01)
    Type = 1;
  else if (abs(StrokeLength * SizeMediumThreshold - inInfo[0].x) < 0.01)
    Type = 0.5;
  else
    Type = 0;

  for(int i=0; i<4; i++)
  {
    float x;
    float y = g_positions[i].y * strokeSize.y;

    if (g_positions[i].x < 0)
      x = g_positions[i].x * pLength;
    else
      x = g_positions[i].x * nLength;

    position.r = x * cosine - y * sine;
    position.g = x * sine + y * cosine;
    position.b = 0;

    vec2 pos = gl_in[0].gl_Position.xy + vec2(position.x * DeltaX, position.y * DeltaY);
    GTexCoord = vec2(pos.x, 1 - pos.y);

    position = mat3(g_mInvView) * position + vec3(index, 0);
    gl_Position = g_mWorldViewProj0 * vec4(position, 1.0);
    EmitVertex();
  }
  EndPrimitive();
}
