#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

smooth in vec2 CoordST[];
flat in ivec2 TexArrayIndex[];
smooth in vec3 Normal[];
smooth in vec4 ProjTexCoord[];
smooth in vec4 Pos3D[];
smooth in vec3 LightDir[];

smooth out vec2 GCoordST;
flat out ivec2 GTexArrayIndex;
smooth out vec3 GNormal;
smooth out vec4 GProjTexCoord;
smooth out vec4 GPos3D;
smooth out vec3 GLightDir;
 
void main() 
{
    vec2 Centroid = (gl_in[0].gl_Position.xy + gl_in[1].gl_Position.xy + gl_in[2].gl_Position.xy)/3.0;
    vec2 x1x0 = normalize(gl_in[0].gl_Position.xy - gl_in[1].gl_Position.xy);
    vec2 x2x0 = normalize(gl_in[0].gl_Position.xy - gl_in[2].gl_Position.xy);
    
    vec2 addX0 = 10.0/480.0 * normalize(x1x0 + x2x0);
//     addX0.x = 10/1280.0 * addX0.x;
//     addX0.y = 10/720.0 * addX0.y;

    GCoordST = CoordST[0];
    GTexArrayIndex = TexArrayIndex[0];
    GNormal = Normal[0];
    GProjTexCoord = ProjTexCoord[0];
    GPos3D = Pos3D[0];
    GLightDir = LightDir[0];
    //gl_Position = gl_in[0].gl_Position + vec4(addX0, 0.0, 0.0);
    gl_Position = vec4(Centroid + 1.004 * (gl_in[0].gl_Position.xy - Centroid), 0.0, 1.0);
    EmitVertex();
    
    vec2 x2x1 = normalize(gl_in[1].gl_Position.xy - gl_in[2].gl_Position.xy);
    vec2 x0x1 = normalize(gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy);
    
    vec2 addX1 = 10.0/480.0 * normalize(x0x1 + x2x1);
//     addX1.x = 10/1280.0 * addX1.x;
//     addX1.y = 10/720.0 * addX1.y;
    
    
    GCoordST = CoordST[1];
    GTexArrayIndex = TexArrayIndex[1];
    GNormal = Normal[1];
    GProjTexCoord = ProjTexCoord[1];
    GPos3D = Pos3D[1];
    GLightDir = LightDir[1];
    //gl_Position = gl_in[1].gl_Position + vec4(addX1, 0.0, 0.0);
    gl_Position = vec4(Centroid + 1.004 * (gl_in[1].gl_Position.xy - Centroid), 0.0, 1.0);
    EmitVertex();

    vec2 x0x2 = normalize(gl_in[2].gl_Position.xy - gl_in[0].gl_Position.xy);
    vec2 x1x2 = normalize(gl_in[2].gl_Position.xy - gl_in[1].gl_Position.xy);
    
    vec2 addX2 = 10.0/480.0 * normalize(x0x2 + x1x2);
//     addX2.x = 10/1280.0 * addX2.x;
//     addX2.y = 10/720.0 * addX2.y;

    
    GCoordST = CoordST[2];
    GTexArrayIndex = TexArrayIndex[2];
    GNormal = Normal[2];
    GProjTexCoord = ProjTexCoord[2];
    GPos3D = Pos3D[2];
    GLightDir = LightDir[2];
    //gl_Position = gl_in[2].gl_Position + vec4(addX2, 0.0, 0.0);
    gl_Position = vec4(Centroid + 1.004 * (gl_in[2].gl_Position.xy - Centroid), 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
    
}