//propagate the overlapping value to the neighborhood
#version 330

smooth in vec2 Blur3x3[9];

out vec4 FragColor;

uniform sampler2D TempImage;

void main()
{
  vec4 finalColor;

  for (int i = 0; i < 9; i++)
    finalColor = texture(TempImage, Blur3x3[i]);

  return finalColor/9.0f;
}