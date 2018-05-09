#version 330

smooth in vec2 CoordTex;

out vec4 FragColor;

uniform sampler2D FlowBuffer0;

void main()
{
  vec2 velocity = texture(FlowBuffer0, CoordTex).rg;
  /*if (velocity.r < 0.5)
   *   r *eturn vec4(1,0,0,1);
   *   else if (velocity.r > 0.5)
   * r e*turn vec4(0,1,0,1);
   * return vec4(0,0,0,1);*/
  //if (abs(velocity.r - 0.3) < 0.01)
  //return vec4(1,0,0,1);
  //else
  //return vec4(0,1,0,1);
  
  FragColor = vec4(velocity, 0, 1);
  //if (abs(velocity.r * velocity.r + velocity.g * velocity.g - 0.5) < 0.00001)
  //return vec4(0,0,0,1);
  //else
  //return vec4(velocity, 0, 1);
}
