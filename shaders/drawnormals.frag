#version 450

smooth in vec3 Normal;

layout(location = 0) out float FragValueX;
layout(location = 1) out float FragMaskX;
layout(location = 2) out float FragValueY;
layout(location = 3) out float FragMaskY;
layout(location = 4) out float FragValueZ;
layout(location = 5) out float FragMaskZ;

void main()
{
    vec3 normNormal = normalize(Normal);
    
    FragValueX = normNormal.x;
    FragValueY = normNormal.y;
    FragValueZ = normNormal.z;
    
    FragMaskX = 1.0;
    FragMaskY = 1.0;
    FragMaskZ = 1.0;
}
