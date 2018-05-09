#version 430

smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 vColor;
smooth in vec3 LightPos;
smooth in vec3 View;
smooth in vec4 ProjTexCoord;
smooth in vec4 Pos3D;

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

struct LayerData
{
    uvec2 layer;
    uvec2 layerMask;
    uvec2 palette;
    vec2 paletteRange;
    float opacity;
    uint samplerType;
};

layout(std140, binding = 12) uniform PerMaterialData
{
    float value;
    uint index;
    uint layerCount;
    bool textureVisibility;
    vec4 defaultColor;
    vec4 specularAlbedoAndPower;    
    LayerData layers[10];
};

layout(location = 0) out vec4 FragColor;

uniform sampler2DArray ArrayTexture[20];
uniform isampler2DArray iArrayTexture[6];
uniform usampler2DArray uArrayTexture[6];
uniform sampler1DArray ArrayTexture1D[2];

// Material properties
uniform vec3 diffuseAlbedo = vec3(0.2, 0.2, 0.2);
uniform vec3 specularAlbedo = vec3(0.7);
uniform float specularPower = 128.0;

void main()
{
    if(ProjTexCoord.q >= 0)
    {
//         vec4 proj = ProjTexCoord.xyzw / ProjTexCoord.w * 0.5 + 0.5;
//         float brushScale = 0.60 - 0.40;
//         vec2 relativePaintBrush = (proj.xy - 0.40)/(brushScale);
//         if(relativePaintBrush.x > 0.0 && relativePaintBrush.x < 1.0 && relativePaintBrush.y > 0.0 && relativePaintBrush.y < 1.0)
//             projectedTex = texture(PPTextures[0], relativePaintBrush);
//         else
//             projectedTex = vec4(0.0);


//        vec4 projectedTex = textureProj(PPTextures[0], ProjTexCoord);
        if(textureVisibility == true)
            FragColor = vColor;//texture(ArrayTexture[TexArrayIndex.x], vec3(CoordST, TexArrayIndex.y));
        else
            FragColor = defaultColor;
                
        for(int i = 0; i < layerCount; ++i)
        {
            //float layerTex =  texture(ArrayTexture[layers[i].layer.x], vec3(CoordST, layers[i].layer.y)).r;        
            float maskValue = texture(ArrayTexture[layers[i].layerMask.x], vec3(CoordST, layers[i].layerMask.y)).r;
            
            if(maskValue == 1.0)
            {
                if(layers[i].samplerType == 0)
                {
                    int value = texture(iArrayTexture[layers[i].layer.x], vec3(CoordST, layers[i].layer.y)).r;
                    vec4 color = texture(ArrayTexture1D[layers[i].palette.x], vec2((value - layers[i].paletteRange.x)/(layers[i].paletteRange.y- layers[i].paletteRange.x), layers[i].palette.y));
                    FragColor = mix(FragColor, color, color.a * layers[i].opacity);
                }
                else if(layers[i].samplerType == 1)
                {
                    uint value = texture(uArrayTexture[layers[i].layer.x], vec3(CoordST, layers[i].layer.y)).r;
                    vec4 color = texture(ArrayTexture1D[layers[i].palette.x], vec2((value - layers[i].paletteRange.x)/(layers[i].paletteRange.y- layers[i].paletteRange.x), layers[i].palette.y));
                    FragColor = mix(FragColor, color, color.a * layers[i].opacity);
                }
                else if(layers[i].samplerType == 2)
                {
                    float value = texture(ArrayTexture[layers[i].layer.x], vec3(CoordST, layers[i].layer.y)).r;
                    vec4 color = texture(ArrayTexture1D[layers[i].palette.x], vec2((value - layers[i].paletteRange.x)/(layers[i].paletteRange.y- layers[i].paletteRange.x), layers[i].palette.y));
                    FragColor = mix(FragColor, color, color.a * layers[i].opacity);
                }
            }
        }
        
        vec3 normal = normalize(Normal);
        vec3 light = normalize(LightPos);
        vec3 view = normalize(View);
        
        vec3 halfVector = normalize(light + view);
        
        vec3 diffuse = max(dot(normal, light), 0.3) * FragColor.xyz;
        vec3 specular = pow(max(dot(normal, halfVector), 0.0), specularAlbedoAndPower.w) * specularAlbedoAndPower.xyz;
        
        FragColor = vec4(diffuse + specular, 1.0);
    }
}
