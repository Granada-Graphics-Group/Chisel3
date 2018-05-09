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

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
};

// layout(std140, binding = 11) uniform TransientData
// {
//     uvec4 inputIndices[10];
// };

layout(std140, binding = 12) uniform PerMaterialData
{
    float value;
};

layout(location = 0) out float FragValue;
layout(location = 1) out float FragMask;
layout(location = 2) out float FragBrushMask;

uniform sampler2DArray ArrayTexture[20];

void main()
{    
    vec3 projPos3D = Pos3D.xyz/Pos3D.w * 0.5 + 0.5;

    float prePassDepth = texture(ArrayTexture[depthIndices.x], vec3(projPos3D.xy, depthIndices.y)).x; //texture(ArrayTexture[inputIndices[1].x], vec3(projPos3D.xy, inputIndices[1].y)).x;  
    //prePassDepth *= 0.999999;
    //prePassDepth -= 0.000001;    
    //float depthDiff = prePassDepth + LightDir.z;
    float depthDiff = projPos3D.z - prePassDepth;
    
    vec2 projCoord = ProjTexCoord.xy/ProjTexCoord.w;    
    
    float delta = (0.0015 - 0.0000005) * (projPos3D.z - 1.0) / -1.0  + 0.0000005;

    if(depthDiff < delta && dot(normalize(Normal), normalize(LightDir)) >= 0.0 && 
        ProjTexCoord.q >= 0 && Pos3D.q >= 0 &&
        projCoord.x >= 0.0 && projCoord.y >= 0.0 && projCoord.x <= 1.0 && projCoord.y <= 1.0)
    {        
        if(texture(ArrayTexture[brushIndices.x], vec3(projCoord.xy, brushIndices.y)).r != 0.0)
        {

            FragValue = value;
            FragMask = 1.0;            
            FragBrushMask = 1.0;
        }
        else
            discard;
    }
    else discard;
}

//     float prePassDepth = texture(ArrayTexture[depthIndices.x], vec3(projPos3D.xy, depthIndices.y)).z; //texture(ArrayTexture[inputIndices[1].x], vec3(projPos3D.xy, inputIndices[1].y)).x;  
//     prePassDepth *= 0.999999;
//     prePassDepth -= 0.1;    
//     float depthDiff = -LightDir.z - prePassDepth;
//     
//     vec2 projCoord = ProjTexCoord.xy/ProjTexCoord.w;    
// 
//     if(-LightDir.z > prePassDepth && dot(normalize(Normal), normalize(LightDir)) >= 0.0 && 
//         ProjTexCoord.q >= 0 && Pos3D.q >= 0 &&
//         projCoord.x >= 0.0 && projCoord.y >= 0.0 && projCoord.x <= 1.0 && projCoord.y <= 1.0)
//     {        
//         if(texture(ArrayTexture[brushIndices.x], vec3(projCoord.xy, brushIndices.y)).a == 1.0)
//         {
//             FragValue = value;
//             FragMask = 1.0;            
//             FragBrushMask = 1.0;
//         }
//         else
//             discard;
//     }
//     else discard;
// }
