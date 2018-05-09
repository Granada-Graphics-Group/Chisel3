#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;

/*
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
};*/

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    uvec2 brushMaskIndices;
};

layout(std140, binding = 11) uniform TransientData
{
    uvec2 copyIndices;
    int Offset;
};

layout(std140, binding = 12) uniform PerMaterialData
{
    float value;
};

layout(location = 0) out float FragValue;
layout(location = 1) out float FragMask;

uniform sampler2DArray ArrayTexture[20];

void main()
{
    float seamMask = texture(ArrayTexture[seamMaskIndices.x], vec3(CoordST, seamMaskIndices.y)).x;
    ivec3 layerSize = textureSize(ArrayTexture[brushMaskIndices.x], 0);
//     float brushMask = texture(ArrayTexture[brushMaskIndices.x], vec3(CoordST, brushMaskIndices.y)).x;
//     float value = texture(ArrayTexture[copyIndices.x], vec3(CoordST, copyIndices.y)).x;
//     
//     float finalValue = value;
// 
//     int lastValidDistance = 200;

    int count = 0;

    for(int y = Offset; y >= -Offset; y--)
        for(int x = -Offset; x <= Offset; x++)
        {
            vec2 offsetCoord = CoordST + vec2(float(x)/layerSize.x, float(y)/layerSize.y);// * vec2(Offset);// * vec2(2.0);// * vec2(1/2048.0);
//             float offsetValue = texture(ArrayTexture[copyIndices.x], vec3(offsetCoord, copyIndices.y)).x;
//             bool isValid = texture(ArrayTexture[brushMaskIndices.x], vec3(offsetCoord, brushMaskIndices.y)).x != 0.0 && seamMask != 0.0;
//             int offsetDistance = x * x + y * y;
                                    
            if(texture(ArrayTexture[brushMaskIndices.x], vec3(offsetCoord, brushMaskIndices.y)).x != 0.0)
                count = count + 1;
                        
//             if(isValid && offsetDistance <= lastValidDistance)
//             {
//                 finalValue = value;
//                 lastValidDistance = offsetDistance;
//             }
        }

    if(seamMask != 0.0 && count > 0)
    {
        FragValue = value;
        FragMask = 1.0;
    }
    else
        discard;    
    
}

// layout(location = 2) out float FragColor3;
// 
// uniform int Offset;
// 
// uniform sampler2DArray ArrayTexture[13];
// uniform sampler2D PPTextures[6];
// 
// void main()
// {
//     // PPTextures 2 layer texture
//     // PPTextures 5 texture seam Mask
//     // PPTexture 3 brush mask
//     // PPTextures 4 layer value texture
// 
//     vec4 texColor = texture(PPTextures[2], CoordST);
//     vec4 seamMask = texture(PPTextures[5], CoordST);
//     vec4 finalColor = texColor;
// 
//     int lastValidDistance = 200;
// 
//     int count = 0;
// 
//     for(int y = Offset; y >= -Offset; y--)
//         for(int x = -Offset; x <= Offset; x++)
//         {
//             vec2 offsetCoord = CoordST + vec2(x/2048.0,y/2048.0);// * vec2(Offset);// * vec2(2.0);// * vec2(1/2048.0);
//             vec4 offsetValue = texture(PPTextures[2], offsetCoord);
//             bool isValid = texture(PPTextures[3], offsetCoord).rgb != vec3(0.0) && seamMask.rgb != vec3(0.0);
//             int offsetDistance = x * x + y * y;
//                         
//             if(isValid)
//                 count = count + 1;
//                         
//                 if(isValid && offsetDistance < lastValidDistance)
//                 {
//                     finalColor = vec4(0.0, 1.0, 0.0, 1.0); //offsetValue;
//                     lastValidDistance = offsetDistance;
//                 }
//                 else if(isValid && offsetDistance == lastValidDistance)
//                 {
//                     finalColor = (offsetValue + finalColor)/ 2.0;
//                 }
//         }
// 
//     if(count >= pow(Offset, 2))
//     {
//         FragColor = finalColor;
//         FragColor2 = value;
//         FragColor3 = 1.0;
//     }
//     else
//         discard;
//         //FragColor = texColor;
//         
//     //FragColor2 = 0.1;//texture(PPTextures[4], CoordST).r;
//     //FragColor = texture(PPTextures[3], CoordST);
// //     if(seamMask.rgb != vec3(0.0))
// //         FragColor = finalColor;
// }
