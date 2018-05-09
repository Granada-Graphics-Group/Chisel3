#version 330

in vec3 CoordTextura;
in vec2 Centro;

//uniform vec4 VecLuz;
out vec4 FragColor;

uniform float Repeticion;
uniform float Ajuste;

uniform vec3 ColorSilueta;
uniform sampler2D Textura;

void main()
{
		const float Pi = 3.14159265;

		vec2 coordTex = vec2(0.0, CoordTextura.z);

		vec2 vectorCentro = CoordTextura.xy - Centro.xy;

		float angulo = atan(vectorCentro.y/vectorCentro.x);

		angulo = (vectorCentro.x < 0.0) ? angulo + Pi : (vectorCentro.y < 0.0) ? angulo + (2 * Pi) : angulo;

/*	if(vectorCentro.x > 0)
		angulo = atan(vectorCentro.y, vectorCentro.x);
	else if (vectorCentro.x < 0)
		angulo = atan(vectorCentro.y, vectorCentro.x) - Pi;
	else if (vectorCentro.x == 0)
		if (vectorCentro.y > 0)
			angulo = Pi/2;
		else if (vectorCentro.y < 0)
			angulo = -Pi/2;*/

		coordTex.x = Repeticion * ((angulo/(2 * Pi)) + length(vectorCentro) * Ajuste);

		FragColor = texture(Textura, coordTex);

		if (FragColor.a < 0.15)
			discard;
		else
			FragColor += vec4(ColorSilueta, FragColor.a);
			

		
		//FragColor = texture(Textura,CoordTextura);
}
