#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;
smooth in vec3 LightDir;

// struct Light
// {
//     vec4 position;
//     vec4 direction; 
//     vec4 color;
// };
// 
// layout(std140, binding = 0) uniform PerFrameData
// {
//     mat4 cameraToClipMatrix;
//     mat4 worldToCameraMatrix;
//     mat4 projectorCameraMatrix;
//     mat4 paintTexCameraMatrix;
//     Light lights[5];
// };

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor2;
layout(location = 2) out vec4 FragColor3;

uniform sampler2DArray ArrayTexture[13];
uniform sampler2D PPTextures[4];

void main()
{
    //float depth = textureProj(PPTextures[3], Pos3D).x;

//         vec4 proj = ProjTexCoord.xyzw / ProjTexCoord.w * 0.5 + 0.5;
//         float brushScale = 0.60 - 0.40;
//         vec2 relativePaintBrush = (proj.xy - 0.40)/(brushScale);
//         if(ProjTexCoord.q >= 0 && relativePaintBrush.x > 0.0 && relativePaintBrush.x < 1.0 && relativePaintBrush.y > 0.0 && relativePaintBrush.y < 1.0)
//         {
//             vec4 projectedTex = texture(PPTextures[0], relativePaintBrush);
//             if(projectedTex.a ==  1.0)
//                 FragColor = projectedTex;
//         }
//         else
//             discard;
    
    vec2 hola = Pos3D.xy/Pos3D.w * 0.5 + 0.5;
    float depth = texture(PPTextures[3], hola).x;
    depth *= 0.999999;
    depth -= 0.000001;
    
    float res = (Pos3D.z/Pos3D.w * 0.5 + 0.5) - depth;
    
//     float res = abs(abs(depth) - abs(Pos3D.z/Pos3D.w * 0.5 + 0.5));

    if(ProjTexCoord.q >= 0 && Pos3D.q >= 0 && res < 0.0008 && dot(normalize(Normal), normalize(LightDir)) > 0.0)
    {
        vec4 color = textureProj(PPTextures[0], ProjTexCoord);
        if(color.a == 1.0)
        {
            FragColor = color;
            FragColor2 = vec4(1.0);
        }
        else
            discard;
    }
    

//     vec4 color = texture(PPTextures[0], vec2(CoordST.x, CoordST.y));
//     vec4 depth = texture(PPTextures[1], vec2(CoordST.x, CoordST.y));
//     float zNear = -5.0;
//     float zFar = 90.0;
//     float zBuffer = Pos3D.z * 0.5 + 0.5;
//  
//     float linearDepth = (2.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
//     FragColor = vec4(vec3(linearDepth), 1.0);
}
