#version 450

#define pi 3.1415926535897932384626433832795

smooth in vec3 Normal;

layout(std140, binding = 11) uniform RenderTargetData
{
    vec3 reference;
};

layout(location = 0) out float FragValue;
layout(location = 1) out float FragMask;

void main()
{
    vec3 normNormal = normalize(Normal);
    vec3 normReference = normalize(reference);
    
    //FragValue = atan(dot(normNormal, normalize(cross(cross(normNormal, normReference), normReference))), dot(normNormal, normReference)) * 180 / pi;
    
    FragValue = reference.z;
    FragMask = 1.0;
}
