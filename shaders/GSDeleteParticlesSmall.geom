#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

in vec4 inInfo[2];
in vec4 inColor[2];

layout(stream = 0) out vec4 Color;
layout(stream = 0) out vec4 Info;

uniform float CurTime;
uniform float ProbabilitySmallThreshold;

uniform sampler2D Probability;
uniform sampler2D TempImage;

void main()
{
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  float prob = texture(Probability, texcoord + vec2(CurTime * 0.05, CurTime * 0.03)).r;
  float overlap = texture(TempImage, texcoord).r;
  //float overlap = TempImage.texture(texcoord, 0 ).a;

  if (overlap >= ProbabilitySmallThreshold * 0.7 && prob < (1 - ProbabilitySmallThreshold * 1.2))
  {
    //VSParticle output = input[0];
    //Color.rgb = vec3(1,1,0);
    //ParticleOutputStream.Append(output);
  }
  else
  {
      Color = inColor[0];
      Info = inInfo[0];
      gl_Position = gl_in[0].gl_Position;
      EmitStreamVertex(0);      
  }
  EndStreamPrimitive(0);
}
