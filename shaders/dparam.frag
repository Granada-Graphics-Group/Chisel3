#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;

struct Light
{
    vec4 position;
    vec4 direction; 
    vec4 color;
};

layout(std140, binding = 0) uniform PerFrameData
{
    mat4 cameraToClipMatrix;
    mat4 worldToCameraMatrix;
    mat4 projectorCameraMatrix;
    mat4 paintTexCameraMatrix;
    Light lights[5];
};

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor2;
layout(location = 2) out vec4 FragColor3;

uniform sampler2DArray ArrayTexture[13];
uniform sampler2D PPTextures[4];

void main()
{
    vec2 realpos = vec2(ProjTexCoord.x/ProjTexCoord.z, ProjTexCoord.y/ProjTexCoord.z);
    
    if(realpos.x > -0.05 && realpos.x < 0.05 && realpos.y > -0.05 && realpos.y < 0.05 && ProjTexCoord.q >= 0)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        FragColor = texture(ArrayTexture[TexArrayIndex.x], vec3(CoordST, TexArrayIndex.y));
//     if(ProjTexCoord.q >= 0)
//          FragColor = textureProj(PPTextures[0], ProjTexCoord);
    

   /* vec2 finalCoords = (ProjTexCoord.st / ProjTexCoord.q) * 0.5 + 0.5;

    if(ProjTexCoord.q >= 0)
    {
        vec4 depth = texture(PPTextures[1], (ProjTexCoord.st / ProjTexCoord.q) * 0.5 + 0.5);
        float zeta = (Pos3D.z / Pos3D.q) * 0.5 + 0.5;
        if(finalCoords.x <= 1.0 && finalCoords.x >= 0.0 && finalCoords.y <= 1.0 && finalCoords.y >= 0.0)// && zeta > depth.x)
            FragColor = texture(PPTextures[0], finalCoords);
         else
            //FragColor = texture(ArrayTexture[GTexArrayIndex.x], vec3(vec2(GCoordST.x, GCoordST.y), GTexArrayIndex.y));
            FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);*/

    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);

//     vec4 color = texture(PPTextures[0], vec2(CoordST.x, CoordST.y));
//     vec4 depth = texture(PPTextures[1], vec2(CoordST.x, CoordST.y));
//     float zNear = -5.0;
//     float zFar = 90.0;
//     float zBuffer = Pos3D.z * 0.5 + 0.5;
//  
//     float linearDepth = (2.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
//     FragColor = vec4(vec3(linearDepth), 1.0);
//     //FragColor = color;
        //FragColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));
}