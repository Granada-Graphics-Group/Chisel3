//For Generating particles
#version 330

in vec3 inPosition;
in vec4 inColor;
in vec4 inInfo;

smooth out vec4 outColor;
smooth out vec4 outInfo;

void main()
{
  outColor = inColor;
  outInfo = inInfo;
  
  gl_Position = vec4(inPosition, 1.0);
}