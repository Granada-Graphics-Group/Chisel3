#version 430

smooth in vec2 GCoordST;
flat in ivec2 GTexArrayIndex;
smooth in vec3 GNormal;
smooth in vec4 GProjTexCoord;
smooth in vec4 GPos3D;
smooth in vec3 GLightDir;

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
    vec2 hola = GPos3D.xy/GPos3D.w * 0.5 + 0.5;
    float depth = texture(PPTextures[3], hola).x;
    depth *= 0.999999;
    depth -= 0.000001;
    
    float res = (GPos3D.z/GPos3D.w * 0.5 + 0.5) - depth;
    
//     float res = abs(abs(depth) - abs(Pos3D.z/Pos3D.w * 0.5 + 0.5));

    if(GProjTexCoord.q >= 0 && GPos3D.q >= 0 && res < 0.0008 && dot(normalize(GNormal), normalize(GLightDir)) > 0.0)
    {
        vec4 color = textureProj(PPTextures[0], GProjTexCoord);
        if(color.a == 1.0)
        {
            FragColor = color;
            FragColor2 = vec4(1.0);
        }
        else
            discard;
    }

//     if(GProjTexCoord.q >= 0)
//         FragColor = textureProj(PPTextures[0], GProjTexCoord);
        
/*    vec2 finalCoords = (GProjTexCoord.st / GProjTexCoord.q) * 0.5 + 0.5;

    if(GProjTexCoord.q >= 0)
    {
        vec4 depth = texture(PPTextures[1], (GProjTexCoord.st / GProjTexCoord.q) * 0.5 + 0.5);
        float zeta = (GPos3D.z / GPos3D.q) * 0.5 + 0.5;
        if(finalCoords.x <= 1.0 && finalCoords.x >= 0.0 && finalCoords.y <= 1.0 && finalCoords.y >= 0.0)// && zeta > depth.x)
            FragColor = texture(PPTextures[0], finalCoords);
         else
            //FragColor = texture(ArrayTexture[GTexArrayIndex.x], vec3(vec2(GCoordST.x, GCoordST.y), GTexArrayIndex.y));
            FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);*/

//     vec4 color = texture(PPTextures[0], vec2(GCoordST.x, GCoordST.y));
//     vec4 depth = texture(PPTextures[1], vec2(GCoordST.x, GCoordST.y));
//     float zNear = -5.0;
//     float zFar = 90.0;
//     float zBuffer = GPos3D.z * 0.5 + 0.5;
//  
//     float linearDepth = (2.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
//     FragColor = vec4(vec3(linearDepth), 1.0);
//     //FragColor = color;
        //FragColor = texture(ArrayTexture[GTexArrayIndex.x], vec3(vec2(GCoordST.x, GCoordST.y), GTexArrayIndex.y));
}