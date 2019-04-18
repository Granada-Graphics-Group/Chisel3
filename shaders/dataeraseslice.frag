#version 450
#extension GL_ARB_bindless_texture : require

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;
smooth in vec3 LightDir;

smooth in vec4 EyePosition;
smooth in vec3 PlaneNormal;
smooth in vec3 PlanePoint;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    vec4 paddle1;
    vec4 paddle2;
    vec4 planeNormal;    
    vec4 planePoint;    
};

layout(std140, binding = 12) uniform PerMaterialData
{
    float value;
};

layout(location = 0) out float FragMask;
layout(location = 1) out float FragBrushMask;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{
    float distance = dot(normalize(PlaneNormal), EyePosition.xyz - PlanePoint);
    
    if(distance < 0)
    {
        vec3 projPos3D = Pos3D.xyz/Pos3D.w;
        vec2 projCoord = ProjTexCoord.xy/ProjTexCoord.w; 
        
        float prePassDepth = texture(ArrayTexture[depthIndices.x], vec3(projPos3D.xy * 0.5 + 0.5, depthIndices.y)).x * 0.999;
        float depthDiff = projPos3D.z - prePassDepth;
            
        if(depthDiff >= 0 && dot(normalize(Normal), normalize(LightDir)) >= 0.0 && 
            ProjTexCoord.q >= 0 && Pos3D.q >= 0 &&
            projCoord.x >= 0.0 && projCoord.y >= 0.0 && projCoord.x <= 1.0 && projCoord.y <= 1.0)
        {        
            if(texture(ArrayTexture[brushIndices.x], vec3(projCoord.xy, brushIndices.y)).r != 0.0)
            {
                FragMask = 0.0;
                FragBrushMask = 1.0;
            }
            else
                discard;
        }
        else discard;
    }
    else discard;        
}    
