#version 330

in vec3 inPosition;
in vec2 inTexCoord;

smooth out vec2 outCoordTex;
smooth out vec2 Blur3x3[9];

uniform float DeltaX;
uniform float DeltaY;

uniform mat4 Projection;
uniform mat4 ModelView;

void main()
{
	Blur3x3[0] = inTexCoord.st - vec2(DeltaX, DeltaY);
	Blur3x3[1] = inTexCoord.st - vec2(0, DeltaY);
	Blur3x3[2] = inTexCoord.st - vec2(-DeltaX, DeltaY);
	Blur3x3[3] = inTexCoord.st - vec2(DeltaX, 0);
	Blur3x3[4] = inTexCoord.st;
	Blur3x3[5] = inTexCoord.st - vec2(-DeltaX, 0);
	Blur3x3[6] = inTexCoord.st - vec2(DeltaX, -DeltaY);
	Blur3x3[7] = inTexCoord.st - vec2(0, -DeltaY);
	Blur3x3[8] = inTexCoord.st - vec2(-DeltaX, -DeltaY);

	outCoordTex = inTexCoord;

	gl_Position = Projection * ModelView * vec4(inPosition, 1.0);
}