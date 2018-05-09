#version 330

in vec3 inPosition;
in vec2 inTexCoord;

smooth out vec2 Blur5x5[9];

uniform float DeltaX;
uniform float DeltaY;

void main()
{
	Blur5x5[0] = inTexCoord.st -  vec2( DeltaX * 2, DeltaY * 2);
	Blur5x5[1] = inTexCoord.st -  vec2( DeltaX, 	DeltaY * 2);
	Blur5x5[2] = inTexCoord.st -  vec2( 0,	 	DeltaY * 2);
	Blur5x5[3] = inTexCoord.st -  vec2(-DeltaX, 	DeltaY * 2);
	Blur5x5[4] = inTexCoord.st -  vec2(-DeltaX * 2, DeltaY * 2);
	Blur5x5[5] = inTexCoord.st -  vec2( DeltaX * 2, DeltaY);
	Blur5x5[6] = inTexCoord.st -  vec2( DeltaX, 	DeltaY);
	Blur5x5[7] = inTexCoord.st -  vec2( 0, 		DeltaY);
	Blur5x5[8] = inTexCoord.st -  vec2(-DeltaX, 	DeltaY);
	Blur5x5[9] = inTexCoord.st -  vec2(-DeltaX * 2, DeltaY);
	Blur5x5[10] = inTexCoord.st - vec2( DeltaX * 2, 0);
	Blur5x5[11] = inTexCoord.st - vec2( DeltaX, 	0);
	Blur5x5[12] = inTexCoord.st;
	Blur5x5[13] = inTexCoord.st - vec2(-DeltaX, 	0);
	Blur5x5[14] = inTexCoord.st - vec2(-DeltaX * 2, 0);
	Blur5x5[15] = inTexCoord.st - vec2( DeltaX * 2, -DeltaY);
	Blur5x5[16] = inTexCoord.st - vec2( DeltaX, 	-DeltaY);
	Blur5x5[17] = inTexCoord.st - vec2( 0, 		-DeltaY);
	Blur5x5[18] = inTexCoord.st - vec2(-DeltaX, 	-DeltaY);
	Blur5x5[19] = inTexCoord.st - vec2(-DeltaX * 2, -DeltaY);
	Blur5x5[20] = inTexCoord.st - vec2( DeltaX * 2, -DeltaY * 2);
	Blur5x5[21] = inTexCoord.st - vec2( DeltaX, 	-DeltaY * 2);
	Blur5x5[22] = inTexCoord.st - vec2( 0, 		-DeltaY * 2);
	Blur5x5[23] = inTexCoord.st - vec2(-DeltaX, 	-DeltaY * 2);
	Blur5x5[24] = inTexCoord.st - vec2(-DeltaX * 2,	-DeltaY * 2);
	

	gl_Position = vec4(inPosition, 1.0);
}
