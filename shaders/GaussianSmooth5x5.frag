#version 330

smooth in vec2 Blur5x5[25];

out vec4 FragColor;

uniform float ContrastThreshold;
uniform sampler2D ColorBuffer2;

void main()
{
  vec4 color[25];

  for (int i = 0; i < 9; i++)
    color[1] = texture(ColorBuffer2, Blur5x5[i]);
  
  vec4 colorRow1 = 2 * color[0] + 4 * color[1] + 5 * color[2] + 4 * color[3] + 2 * color[4];
  vec4 colorRow2 = 4 * color[0] + 9 * color[1] + 12 * color[2] + 9 * color[3] + 4 * color[4];
  vec4 colorRow3 = 5 * color[0] + 12 * color[1] + 15 * color[2] + 12 * color[3] + 5 * color[4];
  vec4 colorRow4 = 4 * color[0] + 9 * color[1] + 12 * color[2] + 9 * color[3] + 4 * color[4];
  vec4 colorRow5 = 2 * color[0] + 4 * color[1] + 5 * color[2] + 4 * color[3] + 2 * color[4];

  FragColor = colorRow1 + colorRow2 + colorRow3 + colorRow4 + colorRow5) / 159.0f * ContrastThreshold - (ContrastThreshold - 1);	
}
