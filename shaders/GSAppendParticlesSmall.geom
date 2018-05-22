#version 400
layout(lines) in;
layout(points, max_vertices = 1) out;

layout(stream  = 0) out vec4 Color;
layout(stream  = 0) out vec4 Info;

uniform float AlphaSmallThreshold;
uniform float ContrastThreshold;
uniform float DeltaX;
uniform float DeltaY;
uniform float CurTime;
uniform float ProbabilitySmallThreshold;
uniform float SizeSmallThreshold;
uniform int StrokeLength;
uniform int StrokeWidth;

uniform sampler2D ColorBuffer1;
uniform sampler2D ColorBuffer2;
uniform sampler2D ColorBuffer3;
uniform sampler2D Probability;
uniform sampler2D RenderInfo;
uniform sampler2D TempImage;

void main()
{
	vec2 texcoord = vec2(gl_in[0].gl_Position.r, 1 - gl_in[0].gl_Position.g);
	vec4 renderInformation = texture(RenderInfo, texcoord);
	float size = renderInformation.r;
	float strokeAngle = renderInformation.a;
	//float alpha = renderInformation.g;
	
	if (abs(size - SizeSmallThreshold) < 0.1)
	//if (abs(alpha - AlphaMediumThreshold) < 0.01)
	{
		float prob = texture(Probability, texcoord + vec2(CurTime * 0.051, CurTime * 0.031)).r;
		float prob1 = texture(Probability, texcoord + vec2(CurTime * 0.082, CurTime *0.03)).r;


		gl_Position.x = gl_in[0].gl_Position.x + prob * DeltaX * 3; 
		gl_Position.y = gl_in[0].gl_Position.y + prob1 * DeltaY * 3; 

		vec3 c1 = texture(ColorBuffer1, texcoord).rgb;
		vec3 c2 = texture(ColorBuffer2, texcoord).rgb;
		vec3 c3 = texture(ColorBuffer3, texcoord).rgb;		

		//average the color a little bit over color values of the previous frames
		Color.rgb = c2 * ContrastThreshold - (ContrastThreshold - 1);
		Color.a = AlphaSmallThreshold * 0.7;
		Info = vec4( size * StrokeLength, size * StrokeWidth, strokeAngle, 0.5);  //1 denotes existing stroke, 0 denotes the stroke to be deleted, 0.5 denotes newly constructed particle
		
		if(prob < ProbabilitySmallThreshold * 0.3)
		{
			//output.color.rgb = float3(1,0,0);
			EmitStreamVertex(0);
		}
		else{
			float emptyness = texture(TempImage, texcoord).r;
			if (emptyness < ProbabilitySmallThreshold * 0.12) //0.07
			{
				//output.color.rgb = float3(1,0,0);
				EmitStreamVertex(0);
			}			
			if (emptyness < ProbabilitySmallThreshold * 0.2) //0.13
			{		
				//output.color.rgb = float3(0,0,1);
				if(prob < ProbabilitySmallThreshold * 0.2)
				  EmitStreamVertex(0);
			}
		}
	}
  EndStreamPrimitive(0);
}