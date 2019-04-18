#version 450

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 EyePosition;

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

layout(location = 0) out float FragColor;
//layout(location = 1) out vec4 FragColor2;
//layout(location = 2) out vec4 FragColor3;

//uniform sampler2DArray ArrayTexture[20];
//uniform sampler2D PPTextures[4];

void main()
{
    FragColor = EyePosition.z;
//     vec4 baseColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));
//     vec4 layerColor = texture(PPTextures[1], CoordST);
//     
//     FragColor = mix(baseColor, layerColor, layerColor.a);

// ----------    
    
//    FragColor = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));
//    FragColor += texture(PPTextures[0], CoordST);
    //FragColor *= texture(PPTextures[1], CoordST);
    //FragColor.a = 0.3;
//     float zNear = -5.0;
//     float zFar = 90.0;
//     float zBuffer = gl_FragCoord.z;
// 
//     float linearDepth = (1.0 * zNear)/(zFar + zNear - zBuffer *(zFar - zNear));
//     FragColor = vec4(vec3(linearDepth), 1.0);
}
