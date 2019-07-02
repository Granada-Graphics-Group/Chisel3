#version 450

smooth in vec3 Position;

layout(location = 0) out float FragValueX;
layout(location = 1) out float FragMaskX;
layout(location = 2) out float FragValueY;
layout(location = 3) out float FragMaskY;
layout(location = 4) out float FragValueZ;
layout(location = 5) out float FragMaskZ;

void main()
{        
    FragValueX = Position.x;
    FragValueY = Position.y;
    FragValueZ = Position.z;
    
    FragMaskX = 1.0;
    FragMaskY = 1.0;
    FragMaskZ = 1.0;
}
