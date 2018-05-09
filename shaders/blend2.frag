#version 430
smooth in vec2 CoordST;
flat in ivec2 TexArrayIndex;
smooth in vec3 Normal;
smooth in vec4 WorldVertex;

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
    Light lights[5];
};

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor2;
layout(location = 2) out vec4 FragColor3;

uniform sampler2DArray ArrayTexture[13];

void main()
{
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);


    vec3 lightVec = normalize(lights[0].position.xyz - WorldVertex.xyz);
    vec3 normal = normalize(Normal);
    float l = dot(normal, lightVec);
    if (l > 0.0)
    {
        color = texture(ArrayTexture[TexArrayIndex.x], vec3(vec2(CoordST.x, CoordST.y), TexArrayIndex.y));    
        float PI = 3.14159265358979323846264;
        float outerCutoff = cos(PI / 3.0);
        float innerCutoff = cos(PI / 18.0);
        float exponent = 0.5;
        float spotlight = max(-dot(lightVec, lights[0].direction.xyz), 0.0);
        float spotlightFade = clamp((outerCutoff - spotlight) / (outerCutoff - innerCutoff), 0.0, 1.0);
        spotlight = pow(spotlight * spotlightFade, exponent);
        
        float d = distance(WorldVertex.xyz, lights[0].position.xyz);
        vec3 attenuation = vec3(0.5, 0.0, 0.0);
        float a = 1.0 / (attenuation.x + (attenuation.y * d) + (attenuation.z * d * d));
        
        //color = vec4(vec3(color) * a * spotlight, 1.0) ;
    }
    
    if(color.a > 0.5)
        FragColor = lights[0].color;
    else
        discard;
    
    FragColor2 = vec4(0.0, color.y, 0.0, 1.0);
    FragColor3 = vec4(color.x, 0.0, color.z , 1.0);
}