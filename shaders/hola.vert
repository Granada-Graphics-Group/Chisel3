#version 440

in vec3 in_Posicion;
in vec2 in_CoordTextura;

smooth out vec2 CoordST;

uniform MatrixBlock
{
  uniform mat4 ModelView;
  uniform mat4 Proj;
} Matrices;

uniform mat4 ModeloVista;
uniform mat4 Proyeccion;

void main(void)
{
	CoordST = in_CoordTextura;

	gl_Position = Proyeccion * ModeloVista * vec4(in_Posicion, 1.0);
} 
