#version 330
 
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 36) out;

in vec4 NormalProyectada[6];
in vec4 Vertice[6];

out vec3 CoordTextura;
out vec2 Centro;

uniform float GrosorSilueta;
uniform float Repeticion;
uniform float Ajuste;
uniform vec4 CentroCL;
uniform float AnguloInicial;
uniform float AnguloFinal;
uniform int TipoSilueta;
uniform mat4 ModeloVista;
uniform mat4 Proyeccion;

const float PI = 3.14159265;

float calculaCoordenadaTextura(vec2 vertice, vec2 centro)
{
	const float Pi = 3.14159265;
	
	float x = vertice.x;// - centro.x;
	float y = vertice.y;// - centro.y;
	float arctan = 0;
	float coordalpha;

	if(x > 0)
		arctan = atan(y, x);
	else if (x < 0)
		arctan = atan(y,x) - Pi;
	else if (x == 0)
		if (y > 0)
			arctan = Pi/2;
		else if (y < 0)
			arctan = -Pi/2;
		
/*	if ( x < 0 || y < 0)
		coordalpha = 2 * Pi + atan(y, x)/2*Pi;
	else*/
		coordalpha = arctan/2*Pi;

	return coordalpha + Ajuste * distance(vertice, centro);
}


vec3 obtenerNormal(vec3 A, vec3 B, vec3 C)
{
	vec3 AB = B - A;
	vec3 AC = C - A;
	return normalize( cross(AB,AC) );
}

bool IsFront(vec3 A, vec3 B, vec3 C)
{
   float area = (A.x * B.y - B.x * A.y) + (B.x * C.y - C.x * B.y) + (C.x * A.y - A.x * C.y);
   return area > 0.0;
}


void emitirArista(vec3 p0, vec3 normal0, vec3 p1, vec3 normal1)
{
   vec2 direccionArista = normalize(p1.xy - p0.xy);
	vec3 N0 = vec3(normalize(normal0.xy - p0.xy), 0);
	vec3 N1 = vec3(normalize(normal1.xy - p1.xy), 0);
   vec3 N = vec3(normalize(vec2(direccionArista.y, -direccionArista.x)), 0);

	float escalar0 = dot(N.xy, N0.xy);
	float escalar1 = dot(N.xy, N1.xy);

	N = N * GrosorSilueta;
	N0 = N0 * GrosorSilueta;
	N1 = N1 * GrosorSilueta;

	Centro = (Proyeccion * ModeloVista * CentroCL).xy;

	vec4 vInfIzqCuad = vec4(p0, 1.0);
	vec4 vSupIzqCuad = vec4(p0 + N, 1.0);
	vec4 vInfDerCuad = vec4(p1, 1.0);
	vec4 vSupDerCuad = vec4(p1 + N, 1.0);


	vec4 vSupTriIzq = vec4((escalar0 < 0.0) ? p0 + (N0 * -1.0f) : p0 + N0, 1.0);
	vec4 vSupTriDer = vec4((escalar1 < 0.0) ? p1 + (N1 * -1.0f) : p1 + N1, 1.0);


	float longA = distance(vSupTriIzq, vSupIzqCuad);
	float longB = distance(vSupIzqCuad, vSupDerCuad);
	float longC = distance(vSupDerCuad, vSupTriDer);
	float longD = longA + longB + longC;

	CoordTextura.xy = p0.xy;
	CoordTextura.z = 0.0;
   gl_Position = vSupTriIzq; 
	EmitVertex();

	CoordTextura.xy = p0.xy + (direccionArista.xy * ((longB * longA) / longD));
	CoordTextura.z = 0.0;   
	gl_Position = vSupIzqCuad; 
	EmitVertex();

	CoordTextura.xy = p0.xy;
	CoordTextura.z = 1.0;
   gl_Position = vInfIzqCuad; 
	EmitVertex();

	CoordTextura.xy = p1.xy - (direccionArista.xy * ((longB * longC) / longD));
	CoordTextura.z = 0.0;      
	gl_Position = vSupDerCuad; 
	EmitVertex();

	CoordTextura.xy = p1.xy;
	CoordTextura.z = 1.0;      
	gl_Position = vInfDerCuad; 
	EmitVertex();

	CoordTextura.xy = p1.xy;
	CoordTextura.z = 0.0;      
	gl_Position = vSupTriDer; 
	EmitVertex();
   EndPrimitive();
}

void main() {
   vec3 v0 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
   vec3 v1 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
   vec3 v2 = gl_in[2].gl_Position.xyz / gl_in[2].gl_Position.w;
   vec3 v3 = gl_in[3].gl_Position.xyz / gl_in[3].gl_Position.w;
   vec3 v4 = gl_in[4].gl_Position.xyz / gl_in[4].gl_Position.w;
   vec3 v5 = gl_in[5].gl_Position.xyz / gl_in[5].gl_Position.w;

	vec3 n0 = NormalProyectada[0].xyz / NormalProyectada[0].w;
	vec3 n1 = NormalProyectada[1].xyz / NormalProyectada[1].w;
	vec3 n2 = NormalProyectada[2].xyz / NormalProyectada[2].w;
	vec3 n3 = NormalProyectada[3].xyz / NormalProyectada[3].w;
	vec3 n4 = NormalProyectada[4].xyz / NormalProyectada[4].w;
	vec3 n5 = NormalProyectada[5].xyz / NormalProyectada[5].w;

/*	vec3 normal0 = obtenerNormal(v0, v2, v4);
	vec3 normal1 = obtenerNormal(v0, v1, v2);
	vec3 normal2 = obtenerNormal(v2, v3, v4);
	vec3 normal3 = obtenerNormal(v0, v4, v5);*/

	vec3 normal0 = obtenerNormal(Vertice[0].xyz, Vertice[2].xyz, Vertice[4].xyz);
	vec3 normal1 = obtenerNormal(Vertice[0].xyz, Vertice[1].xyz, Vertice[2].xyz);
	vec3 normal2 = obtenerNormal(Vertice[2].xyz, Vertice[3].xyz, Vertice[4].xyz);
	vec3 normal3 = obtenerNormal(Vertice[0].xyz, Vertice[4].xyz, Vertice[5].xyz);


	float cos1 = dot (normal0, normal1);
	float cos2 = dot (normal0, normal2);
	float cos3 = dot (normal0, normal3);

	float longPV1 =  dot(normal1, normalize(cross(normalize(cross(normal1, normal0)), normal0)));//sign(cross(normal0, normal1).y) *  length(cross(normal0, normal1));
	float longPV2 =  dot(normal2, normalize(cross(normalize(cross(normal2, normal0)), normal0)));//sign(cross(normal0, normal2).y) *  length(cross(normal0, normal2));
	float longPV3 =  dot(normal3, normalize(cross(normalize(cross(normal3, normal0)), normal0)));//sign(cross(normal0, normal3).y) *  length(cross(normal0, normal3));

	float angulo1 = atan(longPV1, cos1);
	float angulo2 = atan(longPV2, cos2);
	float angulo3 = atan(longPV3, cos3);


	angulo1 = (cos1 < 0.0) ? angulo1 + PI : (longPV1 < 0.0) ? angulo1 + (2 * PI) : angulo1;
	angulo2 = (cos2 < 0.0) ? angulo2 + PI : (longPV2 < 0.0) ? angulo2 + (2 * PI) : angulo2;
	angulo3 = (cos3 < 0.0) ? angulo3 + PI : (longPV3 < 0.0) ? angulo3 + (2 * PI) : angulo3;

	/*float angulo1 = atan(dot(normal1, normalize(cross(cross(normal1, normal0), normal0))), dot(normal1, normal0));
	float angulo2 = atan(dot(normal2, normalize(cross(cross(normal2, normal0), normal0))), dot(normal2, normal0));
	float angulo3 = atan(dot(normal3, normalize(cross(cross(normal3, normal0), normal0))), dot(normal3, normal0));*/

	//atan2( normalsAtCurrentRes[neighbourIndex] * ((normalsAtCurrentRes[neighbourIndex].CrossProduct(normalsAtCurrentRes[i])).CrossProduct(normalsAtCurrentRes[i])).normalize(), normalsAtCurrentRes[neighbourIndex] * normalsAtCurrentRes[i] );

	/*angulo1 = (angulo1 < 0.0) ? angulo1 + (2 * PI) : angulo1;
	angulo2 = (angulo2 < 0.0) ? angulo2 + (2 * PI) : angulo2;
	angulo3 = (angulo3 < 0.0) ? angulo3 + (2 * PI) : angulo3;*/

   if(IsFront(v0, v2, v4))
   {
      if((TipoSilueta == 0 || TipoSilueta == 2) && !IsFront(v0, v1, v2)) emitirArista(v0, n0, v2, n2);
	  else if (TipoSilueta == 2 && angulo1 <  AnguloFinal  && angulo1 > AnguloInicial) emitirArista(v0, n0, v2, n2);
      if((TipoSilueta == 0 || TipoSilueta == 2) && !IsFront(v2, v3, v4)) emitirArista(v2, n2, v4, n4);
	  else if (TipoSilueta == 2 && angulo2 <  AnguloFinal  && angulo2 > AnguloInicial) emitirArista(v2, n2, v4, n4);
      if((TipoSilueta == 0 || TipoSilueta == 2) && !IsFront(v0, v4, v5)) emitirArista(v4, n4, v0, n0);
      else if (TipoSilueta == 2 && angulo3 <  AnguloFinal  && angulo3 > AnguloInicial) emitirArista(v4, n4, v0, n0);
	}

   if(TipoSilueta == 1 && !IsFront(v0, v2, v4))
   {
      if(!IsFront(v0, v1, v2) && angulo1 < AnguloFinal && angulo1 > AnguloInicial) emitirArista(v0, n0, v2, n2);
      if(!IsFront(v2, v3, v4) && angulo2 < AnguloFinal && angulo2 > AnguloInicial) emitirArista(v2, n2, v4, n4);
      if(!IsFront(v0, v4, v5) && angulo3 < AnguloFinal && angulo3 > AnguloInicial) emitirArista(v4, n4, v0, n0);
	}
}
