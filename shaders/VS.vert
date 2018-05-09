#version 330

in vec4 inPosition;
in vec2 inTexCoord;

smooth out vec2 outTexCoord;

void main()
{
  outTexCoord = inTexCoord;
  
  gl_Position = inPosition;
}