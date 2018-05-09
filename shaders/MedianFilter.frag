#version 330

smooth in vec2 Blur3x3[9];

out vec4 FragColor;

uniform sampler2D RenderInfo;

float FindMedian(float a, float b, float c)
{
  float median;
  if( a < b )
  {
    if( b < c)
      median = b;
    else
      median = max(a,c);
  }
  else
  {
    if( a < c)
      median = a;
    else
      median = max(b,c);
  }
  return median;
}

void main()
{
  vec4 c  = texture(RenderInfo, Blur3x3[4]);
  vec4 ul = texture(RenderInfo, Blur3x3[0]);
  vec4 u  = texture(RenderInfo, Blur3x3[1]);
  vec4 ur = texture(RenderInfo, Blur3x3[2]);
  vec4 l  = texture(RenderInfo, Blur3x3[3]);
  vec4 r  = texture(RenderInfo, Blur3x3[5]);
  vec4 bl = texture(RenderInfo, Blur3x3[6]);
  vec4 b  = texture(RenderInfo, Blur3x3[7]);
  vec4 br = texture(RenderInfo, Blur3x3[8]);	
	
  
  float Ur = FindMedian(ul.r, u.r, ur.r);
  float Mr = FindMedian(c.r, l.r, r.r);
  float Br = FindMedian(bl.r, b.r, br.r);
  float Red = FindMedian(Ur, Mr, Br);
  
  float Ug = FindMedian(ul.g, u.g, ur.g);
  float Mg = FindMedian(c.g, l.g, r.g);
  float Bg = FindMedian(bl.g, b.g, br.g);
  float Green = FindMedian(Ug, Mg, Bg);
  
  float Ub = FindMedian(ul.b, u.b, ur.b);
  float Mb = FindMedian(c.b, l.b, r.b);
  float Bb = FindMedian(bl.b, b.b, br.b);
  float Blue = FindMedian(Ub, Mb, Bb);
  
  FragColor = vec4(Red, Green, Blue, c.a);	
}
