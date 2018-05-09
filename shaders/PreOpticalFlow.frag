#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform float DeltaX;
uniform float DeltaY;
uniform sampler2D ColorBuffer2;
uniform sampler2D ColorBuffer3;

void main()
{
  vec4 currentDifference = texture(ColorBuffer2, CoordTex) - texture(ColorBuffer3, CoordTex);
  vec4 gradientX = (texture(ColorBuffer2, CoordTex + vec2( DeltaX,   0)) 
  - texture(ColorBuffer2, CoordTex - vec2( DeltaX,   0))
  + texture(ColorBuffer3, CoordTex + vec2( DeltaX,   0)) 
  - texture(ColorBuffer3, CoordTex - vec2( DeltaX,   0))) * 0.5;
  
  vec4 gradientY = (texture(ColorBuffer2, CoordTex + vec2( 0,   DeltaY)) 
  - texture(ColorBuffer2, CoordTex - vec2( 0,   DeltaY))
  + texture(ColorBuffer3, CoordTex + vec2( 0,   DeltaY)) 
  - texture(ColorBuffer3, CoordTex - vec2( 0,   DeltaY))) * 0.5;
  
  float cD = (currentDifference.r + currentDifference.g + currentDifference.b) / 3.0f;
  
  float gX = 0.33 * gradientX.r + 0.34 * gradientX.g + 0.33 * gradientX.b;
  float gY = 0.33 * gradientY.r + 0.34 * gradientY.g + 0.33 * gradientY.b;
  
  cD = (cD + 1) * 0.5;
  gX = (gX + 1) * 0.5;
  gY = (gY + 1) * 0.5;
  
  //float gradientMagnitude = sqrt(gX * gX + gY * gY + Lambda);
  //float gradientMagnitude = gX * gX + gY * gY + Lambda;
  
  /*if (gX > 1)
   *   r *eturn vec4(1,0,0,1);
   * else
   *   return vec4(0,1,0,1);*/
  
  FragColor =  vec4(cD, gX, gY, 1);
  //return vec4(1,0,0,1);
}
