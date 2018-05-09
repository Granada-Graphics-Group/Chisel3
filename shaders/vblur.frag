uniform float DeltaX;
unfiorm float DeltaY;

uniform sampler2D PreviousVector;

vec4 blur(vec2 texcoord)
{
  //Blur the previous result
  vec4 c  = PreviousVector.texture(texcoord);
  vec4 ul = PreviousVector.texture(texcoord - vec2( DeltaX,   DeltaY));
  vec4 u  = PreviousVector.texture(texcoord - vec2( 0,        DeltaY));
  vec4 ur = PreviousVector.texture(texcoord - vec2(-DeltaX,   DeltaY));
  vec4 l  = PreviousVector.texture(texcoord - vec2( DeltaX,   0));
  vec4 r  = PreviousVector.texture(texcoord - vec2(-DeltaX,   0));
  vec4 bl = PreviousVector.texture(texcoord - vec2( DeltaX,  -DeltaY));
  vec4 b  = PreviousVector.texture(texcoord - vec2( 0,       -DeltaY));
  vec4 br = PreviousVector.texture(texcoord - vec2(-DeltaX,  -DeltaY));	
  
  return (c + ul + u + ur + l + r + bl + b + br) / 9.0f;
}