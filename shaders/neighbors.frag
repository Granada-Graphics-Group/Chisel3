#version 430

smooth in vec2 CoordST;
smooth in vec3 PosST;
flat in vec4 Line;
flat in ivec2 TexArrayIndex;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    uvec2 brushMaskIndices;
    uvec2 areaIndices;
    uvec2 lockIndices;
    uvec2 neighborhoodIndices;
};

layout(location = 0) out vec2 FragCoords;

uniform sampler2DArray ArrayTexture[20];

void main()
{    
    float seamMask = texture(ArrayTexture[seamMaskIndices.x], vec3(PosST.xy, seamMaskIndices.y)).x;
    ivec3 layerSize = textureSize(ArrayTexture[brushMaskIndices.x], 0);

// // Lines    
// //     FragCoords = vec4(uint(CoordST.x * layerSize.x), uint(CoordST.y * layerSize.y), Line.x, Line.y);
//     
//     dvec2 ba = Line.zw - Line.xy;
//     dvec2 ca = PosST.xy - Line.xy;
//     
//     double t = (ca.x * ba.x + ca.y * ba.y) / (ba.x * ba.x + ba.y * ba.y);
//     
//     dvec2 d = Line.xy + t * ba;
//     
//     float lineMask = texture(ArrayTexture[seamMaskIndices.x], vec3(d.xy, seamMaskIndices.y)).x;
//     
//     bool result = abs(int(d.x * layerSize.x) + 0.5 - gl_FragCoord.x) <= 1 && abs(int(d.y * layerSize.y) + 0.5 - gl_FragCoord.y) <= 1;
//      
// //     if(lineMask != 0.0)
// //     {
// //         if(seamMask != 0.0 && int(d.x * layerSize.x) == int(PosST.x * layerSize.x) && int(d.y * layerSize.y) == int(PosST.y * layerSize.y))
// //             FragCoords = vec2(int(d.x * layerSize.x), int(PosST.x * layerSize.x));//CoordST;//uvec2(uint(CoordST.x * layerSize.x), uint(CoordST.y * layerSize.y));
// //         else
// //             discard;
// //     }
// //     else if(lineMask == 0.0)
// //     {
//         if(seamMask != 0.0 && result)
//             FragCoords = vec2(int(d.x * layerSize.x), int(PosST.x * layerSize.x));//CoordST;//uvec2(uint(CoordST.x * layerSize.x), uint(CoordST.y * layerSize.y));
//         else
//             discard;        
// //     }
// //     else
// //         discard;
    
// Triangles - distance    
    double distance = ((Line.w - Line.y) * PosST.x - (Line.z - Line.x) * PosST.y + Line.z * Line.y - Line.w * Line.x) / length(Line.zw - Line.xy);
    
    bool result = abs(distance) <= double(1.33) / layerSize.x;
     
    if(result && PosST.z >= -0.3 && seamMask != 0.0)
        FragCoords = uvec2(uint(CoordST.x * layerSize.x), uint(CoordST.y * layerSize.y));//vec2(abs(distance), double(1) / layerSize.x);//CoordST;
    else
        discard;
        
//     if(PosST.z >= -0.3)
//     {
//     for(int x = -1; x <= 1; x++)
//         for(int y = -1; y <= 1; y++)
//         {
//             vec2 offset = vec2(float(x)/layerSize.x, float(y)/layerSize.y);
//             float seamMask = texture(ArrayTexture[seamMaskIndices.x], vec3(PosST.xy + offset, seamMaskIndices.y)).x;
//                 
//             if(seamMask != 0.0)
//                 FragCoords = CoordST;
//         }        
//     }
}
