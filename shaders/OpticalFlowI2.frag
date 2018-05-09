#version 330

smooth in vec2 Blur3x3[9];
smooth in vec2 CoordTex;

out vec4 FragColor;

uniform float DeltaX;
uniform float DeltaY;
uniform float Lambda;

uniform sampler2D ColorBuffer0;
uniform sampler2D ColorBuffer2;
uniform sampler2D ColorBuffer3;
uniform sampler2D PreviousVector0;

void main()
{
  vec4 imageInfo = texture(ColorBuffer0, CoordTex);
  float cD = imageInfo.r * 2 - 1;
  float gX = imageInfo.g * 2 - 1;
  float gY = imageInfo.b * 2 - 1;
  //float gradientMagnitude = sqrt(gX * gX + gY * gY + Lambda);
  float gradientMagnitude = gX * gX + gY * gY + Lambda;
  //float gradientMagnitude = imageInfo.a;
  
  vec2 c  = texture(PreviousVector0, Blur3x3[4]).rg;
  vec2 ul = texture(PreviousVector0, Blur3x3[0]).rg;
  vec2 u  = texture(PreviousVector0, Blur3x3[1]).rg;
  vec2 ur = texture(PreviousVector0, Blur3x3[2]).rg;
  vec2 l  = texture(PreviousVector0, Blur3x3[3]).rg;
  vec2 r  = texture(PreviousVector0, Blur3x3[5]).rg;
  vec2 bl = texture(PreviousVector0, Blur3x3[6]).rg;
  vec2 b  = texture(PreviousVector0, Blur3x3[7]).rg;
  vec2 br = texture(PreviousVector0, Blur3x3[8]).rg;
  
  vec2 result = (c + ul + u + ur + l + r + bl + b + br) / 9.0f;
  vec2 preVectorBlured = result.rg * 2 - vec2(1,1);
  
  float preDotGradient = preVectorBlured.r * gX + preVectorBlured.g * gY;
  
  vec2 velocity;
  velocity.r = preVectorBlured.r - (cD + preDotGradient) * (gX / gradientMagnitude);
  velocity.g = preVectorBlured.g - (cD + preDotGradient) * (gY / gradientMagnitude);
  
  velocity.r = (velocity.r * 0.5 + 1) * 0.5;
  velocity.g = (velocity.g * 0.5 + 1) * 0.5;
  
  FragColor = vec4(velocity, 0.0, 1.0);
  //return vec4(CD, gX, gY, gradientMagnitude);
  //return vec4(0,0,0,1);
}
