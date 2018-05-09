#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 WorldVertex;
smooth in vec4 Pos3D;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    uvec2 brushMaskIndices;
    uvec2 areaIndices;
    uvec2 lockIndices;
};

layout(location = 0) out vec4 FragColor;

uniform sampler2DArray ArrayTexture[13];

void main()
{
//     vec4 co = texture(ArrayTexture[brushIndices.x], vec3(CoordST, brushIndices.y));
//     FragColor = vec4(co.rgb, 1.0);

    float area = texture(ArrayTexture[areaIndices.x], vec3(CoordST, areaIndices.y)).r;
//     float area = texture(uArrayTexture[lockIndices.x], vec3(CoordST, lockIndices.y)).r;
// 
//     if(area < 1)
//         FragColor = vec4(0.0, 0.0, 0.0, 1.0);
//     else if(area < 3)
//         FragColor = vec4(1.0, 1.0, 0.0, 1.0);
//     else if(area < 5)
//         FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//     else if(area < 7)
//         FragColor = vec4(0.0, 0.0, 1.0, 1.0);
//     else
//         FragColor = vec4(0.0, 1.0, 1.0, 1.0);

    
    if(area < 0.00000001)
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else if(area < 0.2)
        FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    else if(area < 0.3)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if(area < 0.4)
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    else
        FragColor = vec4(0.0, 1.0, 1.0, 1.0);
}
