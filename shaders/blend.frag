#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

out vec4 FragColor;

uniform sampler2DArray ArrayTexture[6];

uniform sampler2D SynthDepthTex;
uniform sampler2D SynthColorTex;
uniform sampler2D KinectDepthTex;
//uniform sampler2D KinectColorTex;

void main()
{
    vec4 synthDepth = texture(SynthDepthTex, vec2(CoordST.x, CoordST.y));
    vec4 synthColor = texture(SynthColorTex, vec2(CoordST.x, CoordST.y));
    vec4 videoDepth = texture(KinectDepthTex, vec2(CoordST.x, CoordST.y));
    vec4 videoColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));

    vec2 delta = vec2(1.0/1920.0, 1.0/1080.0);

    float left = CoordST.x - 15 * delta.x;
    float right = CoordST.x + 15 * delta.x;
    float up = CoordST.y - 15 * delta.y;
    float down = CoordST.y + 15 * delta.y;

    float depth;

    if(videoColor.a > 0.6)
    {
      depth = videoDepth;
      bool validColumn = true;

      for(float i = left ; i >= left && i <= right; i+= delta.x)
      {
	float lineDepth;

// 	if(validColumn == true)
// 	    lineDepth = depth;
// 	else
// 	    lineDepth = videoDepth;

	for(float j = up; j >= up && j <= down; j+= delta.y)
	{
	  vec4 ivideoColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(i, j), TexArrayIndex.y));
	
	  if(ivideoColor.a > 0.6)
	  {	  
	    vec4 ivideoDepth = texture(KinectDepthTex, vec2(i, j));
	    vec4 isynthDepth = texture(SynthDepthTex, vec2(i, j));

	    if(ivideoDepth.x < isynthDepth.x && ivideoDepth.x < depth)
	    {
	      depth = ivideoDepth.x;
// 	      validColumn = true;
	    }
	  }
// 	  else
// 	  {
// 	      if(j <= CoordST.y)
// 	      {
// 		depth = lineDepth;
// 		validColumn = false;
// 	      }
// 	      else
// 		j = down + delta.y;
// 	  }
	}
      }
      if(synthDepth.x < depth)
         FragColor = synthColor;
      else
         FragColor = videoColor;
    }
    else
      FragColor = synthColor;


//     float zNear = 0.1;
//     float zFar = 5.0;
//     float zBuffer = gl_FragCoord.z;
// 
//     if(videoDepth.x <= synthDepth.x)
//       zBuffer = videoDepth.x;
//     else
//       zBuffer = synthDepth.y;
// 
//     float linearDepth = (2.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
//     FragColor = vec4(vec3(linearDepth), 1.0);

}