#version 450
 
layout(triangles) in;
layout (triangle_strip, max_vertices=3) out;
 
smooth in vec2 CoordST[3]; 
flat in vec2 FlatCoordST[3];
flat in vec3 VertexPosition[3];
flat out float AreaPerPixel;

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

uniform sampler2DArray ArrayTexture[20];

smooth out vec2 GCoordST;


void main()
{
    float realLength[3];
    float textureLength[3];        
    vec2 coordST[3];    
    
    ivec3 layerSize = textureSize(ArrayTexture[areaIndices.x], 0);

    coordST[0] = FlatCoordST[0] * layerSize.x;
    coordST[1] = FlatCoordST[1] * layerSize.x;
    coordST[2] = FlatCoordST[2] * layerSize.x;

//     realLength[0] = length(VertexPosition[1] - VertexPosition[0]);
//     realLength[1] = length(VertexPosition[2] - VertexPosition[1]);
//     realLength[2] = length(VertexPosition[0] - VertexPosition[2]);    
//     
//     textureLength[0] = length(coordST[1] - coordST[0]);
//     textureLength[1] = length(coordST[2] - coordST[1]);
//     textureLength[2] = length(coordST[0] - coordST[2]);
//     
//     float realSemiPer = (realLength[0] + realLength[1] + realLength[2])/2.0;
//     float textureSemiPer = (textureLength[0] + textureLength[1] + textureLength[2])/2.0;
//     
//     float realArea = sqrt(realSemiPer * (realSemiPer - realLength[0]) * (realSemiPer - realLength[1]) * (realSemiPer - realLength[2]));
//     float textureArea = sqrt(textureSemiPer * (textureSemiPer - textureLength[0]) * (textureSemiPer - textureLength[1]) * (textureSemiPer - textureLength[2]));

    float realArea = length(cross(VertexPosition[1] - VertexPosition[0], VertexPosition[2] - VertexPosition[0]));
//     vec2 textA = coordST[1] - coordST[0];
//     vec2 textB = coordST[2] - coordST[0];
//     float textureArea = abs(textA.x * textB.y - textB.x * textA.y);
    float textureArea = abs(determinant(mat2(coordST[1] - coordST[0], coordST[2] - coordST[0])));
    
    AreaPerPixel = realArea/textureArea;
    
    for(int i = 0; i < gl_in.length(); i++)
    {
        GCoordST = CoordST[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
