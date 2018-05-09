#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D RenderInfo;

void main()
{
  float Stroke = texture(RenderInfo, CoordTex).g;
  //float Stroke = StrokeBuffer.texture(CoordTex, 0 ).a;
  if ( Stroke > 0.001)
    FragColor = vec4(1, 1, 1, 1);
  else
    FragColor = vec4(0, 0, 0, 1);
}
