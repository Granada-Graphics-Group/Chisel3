#version 330

smooth in vec2 Blur3x3[9];
smooth in vec2 outCoordTex;

out vec4 FragColor;

uniform float ContrastThreshold;
uniform sampler2D ColorBuffer2;

void main()
{
//  vec4 finalColor;

//  for (int i = 0; i < 9; i++)
//    finalColor = texture(ColorBuffer2, Blur3x3[i]);

  //FragColor = finalColor/9.0f * ContrastThreshold - (ContrastThreshold - 1); //enhance contrast
  FragColor = texture(ColorBuffer2, outCoordTex);
  
}