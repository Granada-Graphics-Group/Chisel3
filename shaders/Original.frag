#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform float ContrastThreshold;
uniform sampler2D ColorBuffer2;

void main()
{
  
  /*if (OpticalFlowTexture.SampleLevel(samLinear, CoordTex, 0).r > 0.5001 || OpticalFlowTexture.SampleLevel(samLinear, CoordTex, 0).g > 0.5001)
   *   r *eturn vec4(1,0,0,1);
   *   else return vec4(0,0,0,1);*/
  //return vec4(OpticalFlowTexture.SampleLevel(samLinear, CoordTex, 0).rg, 0, 1);
  FragColor = texture(ColorBuffer2, CoordTex) * ContrastThreshold - (ContrastThreshold - 1);
  //return PaperTex.texture(CoordTex, 0);
  //return vec4(Probability.texture(CoordTex, 0));
}
