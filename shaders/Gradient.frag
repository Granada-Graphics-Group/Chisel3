uniform float AngleThreshold;
uniform bool OnRealGradientAngle;

vec2 Gradient(vec2 Blur3x3[9], Texture2D BufferName)
{
  vec4 color[9];
  
  for (int i = 0; i < 9; i++)
    color[i] = texture(BufferName, Blur3x3[i]);  
  
  /*vec3 gX = ul - ur + 2 * l - 2 * r + bl - br;
   *   f l*oat3 gY = ul - bl + 2 * u - 2 * b + ur - br;
   *   vec3 G = sqrt(gX * gX + gY * gY);
   * float Gradient = (G.r + G.g + G.b) * 0.33;*/
  
  vec4 gX = color[0] - color[2]  + 2 * color[3] - 2 * color[5] + color[6] - color[8];
  vec4 gY = color[0] - color[6]  + 2 * color[1] - 2 * color[7] + color[2] - color[8];
  
  float gX1 = 0.299 * gX.r + 0.557 * gX.g + 0.144 * gX.b;
  float gY1 = 0.299 * gY.r + 0.557 * gY.g + 0.144 * gY.b;
  
  float gradient = sqrt(gX1 * gX1 + gY1 * gY1);
  
  float gradientAngle = 0;
  if (!OnRealGradientAngle && gradient< AngleThreshold)		
    gradientAngle = texture(BufferName, CoordTex).r;//CHANGE	
    else
    {
      float y = sqrt(gY * gY);
      float x = sqrt(gX * gX);
      
      if (gX.r < 0) x = -x;
      if (gY.r > 0) y = -y;
      
      float angle;
      if (abs(x) < 0.001)
	angle = 0;
      else
	angle= atan2(y, x);
      
      gradientAngle = ( angle / PI + 1 ) * 0.5;
    }
    
    return vec2(gradient, gradientAngle);
}

