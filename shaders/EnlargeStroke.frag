#version 330

smooth in vec2 Blur3x3[9];

out vec4 FragColor;

uniform sampler2D RenderInfo;

void main()
{
  float count = 0.0f;
  vec4 result = vec4(0.0f);
  vec4 c  = texture(RenderInfo, Blur3x3[4]);
  vec4 ul = texture(RenderInfo, Blur3x3[0]);
  vec4 u  = texture(RenderInfo, Blur3x3[1]);
  vec4 ur = texture(RenderInfo, Blur3x3[2]);
  vec4 l  = texture(RenderInfo, Blur3x3[3]);
  vec4 r  = texture(RenderInfo, Blur3x3[5]);
  vec4 bl = texture(RenderInfo, Blur3x3[6]);
  vec4 b  = texture(RenderInfo, Blur3x3[7]);
  vec4 br = texture(RenderInfo, Blur3x3[8]);	
  
  if (max(max(c.r, c.g), c.b) < 0.9)
  {
    result = result + c;
    count = count + 1.0f;
  }
  
  if (max(max(ul.r, ul.g), ul.b) < 0.9)
  {
    result = result + ul;
    count = count + 1.0f;
  }
  
  if (max(max(u.r, u.g), u.b) < 0.9)
  {
    result = result + u;
    count = count + 1.0f;
  }
  
  if (max(max(ur.r, ur.g), ur.b) < 0.9)
  {
    result = result + ur;
    count = count + 1.0f;
  }
  
  if (max(max(l.r, l.g), l.b) < 0.9)
  {
    result = result + l;
    count = count + 1.0f;
  }
  
  if (max(max(r.r, r.g), r.b) < 0.9)
  {
    result = result + r;
    count = count + 1.0f;
  }
  
  if (max(max(bl.r, bl.g), bl.b) < 0.9)
  {
    result = result + bl;
    count = count + 1.0f;
  }
  
  if (max(max(b.r, b.g), b.b) < 0.9)
  {
    result = result + b;
    count = count + 1.0f;
  }
  
  if (max(max(br.r, br.g), br.b) < 0.9)
  {
    result = result + br;
    count = count + 1.0f;
  }
  
  if (count > 0.0)
    FragColor = result / count ;// * 1.1 - 0.1;
  else
    FragColor = vec4(1,1,1,1);
}
