#version 430

in vec4 vertexColor;
flat in float zPos;

out vec4 FragColor;

void main()
{
    if(zPos < 2.6)
      FragColor = vertexColor;
    else
      discard;

//     float zNear = 0.1;
//     float zFar = 5.0;
//     float zBuffer = gl_FragCoord.z;
//     float linearDepth = (2.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
//     FragColor = vec4(vec3(linearDepth), 1.0);
}	