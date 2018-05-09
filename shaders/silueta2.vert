#version 330

in vec3 in_Posicion;
in vec3 in_Normal;

out vec4 Vertice;
out vec4 NormalProyectada;

uniform mat4 ModeloVista;
uniform mat4 Proyeccion;
//in vec3 Normal;

void main()
{	
	NormalProyectada = Proyeccion * ModeloVista * vec4(in_Posicion.xy + in_Normal.xy, 0.0, 1.0);
	//Transform the vertex (ModelViewProj matrix)
	//gl_Position = gl_ModelViewProjectionMatrix * vec4(Posicion, 1.0);
	Vertice = ModeloVista * vec4(in_Posicion, 1.0);
	gl_Position = Proyeccion * ModeloVista * vec4(in_Posicion, 1.0);
	//gl_Position = ftransform();
}




/*# version 150 compatibility

void main(void)
{
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
}*/