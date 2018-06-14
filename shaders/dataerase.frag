#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;
smooth in vec3 LightDir;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
};

layout(std140, binding = 12) uniform PerMaterialData
{
    float value;
};

layout(location = 0) out float FragMask;
layout(location = 1) out float FragBrushMask;

uniform sampler2DArray ArrayTexture[20];

void main()
{    
    vec3 projPos3D = Pos3D.xyz/Pos3D.w;
    vec2 projCoord = ProjTexCoord.xy/ProjTexCoord.w; 
    
    float prePassDepth = texture(ArrayTexture[depthIndices.x], vec3(projPos3D.xy * 0.5 + 0.5, depthIndices.y)).x * 0.999;
    float depthDiff = projPos3D.z - prePassDepth;
           
    if(depthDiff >= 0 && dot(normalize(Normal), normalize(LightDir)) >= 0.0 &&  
        ProjTexCoord.q >= 0 && Pos3D.q >= 0 &&
        projCoord.x >= 0.0 && projCoord.y >= 0.0 && projCoord.x <= 1.0 && projCoord.y <= 1.0)
    {        
        if(texture(ArrayTexture[brushIndices.x], vec3(projCoord.xy, brushIndices.y)).a == 1.0)
        {
            FragMask = 0.0;
            FragBrushMask = 1.0;
        }
        else
            discard;
    }
    else discard;
}

//     vec3 projPos3D = Pos3D.xyz/Pos3D.w * 0.5 + 0.5;
// 
//     float prePassDepth = texture(ArrayTexture[depthIndices.x], vec3(projPos3D.xy, depthIndices.y)).x;
//     prePassDepth *= 0.999998;
//     //prePassDepth -= 0.000001;    
// 
//     float depthDiff = projPos3D.z - prePassDepth;
//     
//     vec2 projCoord = ProjTexCoord.xy/ProjTexCoord.w;    
// 
//     if(depthDiff < mix(0.001, 0.000005, prePassDepth) /*&& dot(normalize(Normal), normalize(LightDir)) >= 0.0*/ && 
//         ProjTexCoord.q >= 0 && Pos3D.q >= 0 &&
//         projCoord.x >= 0.0 && projCoord.y >= 0.0 && projCoord.x <= 1.0 && projCoord.y <= 1.0)
//     {        
//         if(texture(ArrayTexture[brushIndices.x], vec3(projCoord.xy, brushIndices.y)).a == 1.0)
//         {
//             FragMask = 0.0;
//             FragBrushMask = 1.0;
//         }
//         else
//             discard;
//     }
//     else discard;

//     float prePassDepth = texture(ArrayTexture[depthIndices.x], vec3(projPos3D.xy, depthIndices.y)).x;
//     prePassDepth *= 0.999999;
//     prePassDepth -= 0.000001;    
//     float depthDiff = projPos3D.z - prePassDepth;
//     
//     vec2 projCoord = ProjTexCoord.xy/ProjTexCoord.w;    
// 
//     if(depthDiff < 0.00008 && dot(normalize(Normal), normalize(LightDir)) > 0.0 && 
//         ProjTexCoord.q >= 0 && Pos3D.q >= 0 &&
//         projCoord.x > 0.0 && projCoord.y > 0.0 && projCoord.x < 1.0 && projCoord.y < 1.0)
//     {        
//         if(texture(ArrayTexture[brushIndices.x], vec3(projCoord.xy, brushIndices.y)).a == 1.0)
//         {
//             FragMask = 0.0;
//             FragBrushMask = 1.0;
//         }
//         else
//             discard;
//     }
//     else discard;
// }
