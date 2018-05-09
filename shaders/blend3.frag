#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

layout(location = 0) out vec4 FragColor;

layout(std140, binding = 12) uniform colorVec
{
    vec4 red;
    vec4 green;
    vec4 blue;
};

uniform sampler2D PPTextures[4];

void main()
{
    vec4 color0 = texture(PPTextures[0], vec2(CoordST.x, CoordST.y));
    vec4 color1 = texture(PPTextures[1], vec2(CoordST.x, CoordST.y));
    vec4 color2 = texture(PPTextures[2], vec2(CoordST.x, CoordST.y));
    vec4 depth = texture(PPTextures[3], vec2(CoordST.x, CoordST.y));
    
    float zNear = 0.1;
    float zFar = 5.0;
    float zBuffer = depth.x;

    float linearDepth = (2.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
    FragColor = vec4(vec3(linearDepth), 1.0);
//     if(FragColor.x == 1.0)
//         FragColor *= vec4(vec3(green), 1.0);
//     else
        FragColor = color2;
        //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    
    //FragColor = color1;
}