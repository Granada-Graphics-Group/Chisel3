#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

in vec4 inInfo[2];
in vec4 inColor[2];

layout(stream = 0) out vec4 Color;
layout(stream = 0) out vec4 Info;

uniform float CurTime;
uniform float ProbabilityMediumThreshold;

uniform sampler2D Probability;
uniform sampler2D TempImage;

void main()
{
  vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
  float prob = texture(Probability, texcoord + vec2(CurTime * 0.055, CurTime * 0.035)).r;

  float overlap = texture(TempImage, texcoord).r;

  //if there are many strokes overlapped on each other, append the paricle conditionally
  if (overlap >= ProbabilityMediumThreshold) //there was && Prob < (1 - ProbabilityMediumThreshold)
  {
    //in highly overlapped region, delete the stroke conditionally
    if (prob < 1 - overlap * (1 - ProbabilityMediumThreshold) * 1.67 )
    {
      //Color.rgb = vec3(1,0,0);
      Color = inColor[0];
      Info = inInfo[0];
      gl_Position = gl_in[0].gl_Position;
      EmitVertex();  
    }

    //VSParticle output = input[0];		
    //Color.rgb = vec3(1,0,0);
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