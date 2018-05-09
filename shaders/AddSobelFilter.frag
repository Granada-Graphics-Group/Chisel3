#version 330

float PI = 3.1416;

smooth in vec2 Blur3x3[9];
smooth in vec2 CoordTex;

out vec4 FragColor;

uniform float AngleThreshold;
uniform float GradientThreshold;
uniform float GradientLargeThreshold;
uniform float GradientMediumThreshold;
uniform float GradientSmallThreshold;
uniform float ProbabilityLargeThreshold;
uniform float ProbabilityMediumThreshold;
uniform float ProbabilitySmallThreshold;
uniform float AlphaLargeThreshold;
uniform float AlphaMediumThreshold;
uniform float AlphaSmallThreshold;
uniform float SizeMediumThreshold;
uniform float SizeSmallThreshold;
uniform bool OnRealGradientAngle;
uniform float ContrastThreshold;
uniform float MouseX;
uniform float MouseY;
uniform float FocusF;
uniform float FocusR1;
uniform float FocusR2;
uniform bool OnFocusPoint;

uniform sampler2D ColorBuffer0;
uniform sampler2D ColorBuffer1;
uniform sampler2D ModelMatte;
uniform sampler2D Probability;
uniform sampler2D StrokeTex;


vec4 RGB_to_HSV (vec4 color)
{
	float r, g, b, delta;
	float colorMax, colorMin;
	float h = 0, s = 0, v = 0;
	vec4 hsv = vec4(0);
	r = color[0];
	g = color[1];
	b = color[2];
	colorMax = max (r, g);
	colorMax = max (colorMax, b);
	colorMin = min (r, g);
	colorMin = min (colorMin, b);
	v = colorMax; // this is value
	if (colorMax != 0)
	{
		s = (colorMax - colorMin) / colorMax;
	}
	if (s != 0) // if not achromatic
	{
		delta = colorMax - colorMin;

		if (r == colorMax)
		{
			h = (g - b)/delta;
		}
		else if (g == colorMax)
		{
			h = 2.0 + (b - r) / delta;
		}
		else // b is max
		{
			h = 4.0 + (r - g)/delta;
		}
		h *= 60;
		if( h < 0)
		{
			h += 360;
		}
		hsv[0] = h / 360.0; // moving h to be between 0 and 1.
		hsv[1] = s;
		hsv[2] = v;
	}

	return hsv;
}

void main()
{
  vec4 color[9];
  
  for (int i = 0; i < 9; i++)
    color[i] = texture(ColorBuffer1, Blur3x3[i]);
  
  vec4 gX = color[0] - color[2]  + 2 * color[3] - 2 * color[5] + color[6] - color[8];
  vec4 gY = color[0] - color[6]  + 2 * color[1] - 2 * color[7] + color[2] - color[8];
  
  float gX1 = 0.299 * gX.r + 0.557 * gX.g + 0.144 * gX.b;
  float gY1 = 0.299 * gY.r + 0.557 * gY.g + 0.144 * gY.b;
  
  vec2 fxy = vec2(MouseX, MouseY);
  float rad = length(fxy - CoordTex);// /sqrt(2);
  float scale;
  
  if (OnFocusPoint)
    scale = FocusF + (1 - FocusF) * smoothstep(1 - FocusR2, 1 - FocusR1, 1 - rad);
  else
    scale = FocusF + texture(ModelMatte, CoordTex).r * (1 - FocusF);
  
  float gradient = sqrt(gX1 * gX1 + gY1 * gY1) * scale;
  
  float gradientAngle = 0;
  //if (!OnRealGradientAngle && Gradient< angleThreshold)	
  if (gradient< AngleThreshold) //in low frequency area
  {
    if (OnRealGradientAngle)
      gradientAngle = texture(ColorBuffer0, CoordTex, 0 ).r;//CHANGE
    else
    {
	vec3 hsv = RGB_to_HSV(texture(ColorBuffer0, CoordTex, 0 )).rgb;
	gradientAngle = mod(hsv.r * smoothstep(0, 0.1, hsv.g * hsv.b) + 0.25, 1.0);
	//jitter the gradient angle a bit
	gradientAngle = mod(gradientAngle + texture(Probability, CoordTex).r * 0.1, 1.0);
    }
  }
  else
  {
    float y = sqrt(gY.r * gY.r);
    float x = sqrt(gX.r * gX.r);
  
    //float Y = gY1;
    //float X = gX1;
  
    //if (X < 0) X = -X;
    //if (Y > 0) Y = -Y;
    if (gX.r < 0) x = -x;
    if (gY.r > 0) y = -y;
  
    float angle;
    if (abs(x) < 0.01)		
      ;
    //angle = PI * 0.5;
    else
      angle= atan(y, x);
    
    gradientAngle = ( angle / PI + 1 ) * 0.5;
  }

  //return vec4(Gradient, Gradient, Gradient, GradientAngle);
  //vec2 opticalFlow = FlowBuffer0.texture(CoordTex, 0 ).rg;
  //return vec4(opticalFlow, Gradient, GradientAngle);

  //calculate the stroke properties
  vec4 finalColor;
  float prob = texture(Probability, CoordTex).r; //probability from 0 - 1
  finalColor = vec4(0, 0, gradient, gradientAngle);

  //for the large stroke
  if (gradient < GradientLargeThreshold)
  {	
    finalColor.r = 1; //size
    if (prob < ProbabilityLargeThreshold && GradientThreshold < GradientLargeThreshold) //prob 0.05
      finalColor.g = AlphaLargeThreshold;
  }	

  //for the medium stroke
  else if (gradient >= GradientLargeThreshold && gradient < GradientMediumThreshold)
  {
    finalColor.r = SizeMediumThreshold;
    if (prob < ProbabilityMediumThreshold && GradientThreshold < GradientMediumThreshold) //prob 0.15
      finalColor.g = AlphaMediumThreshold;
  }

  //for the small stroke
  else if (gradient >= GradientMediumThreshold) // 0.3
  {		
    finalColor.r = SizeSmallThreshold;
    if (prob < ProbabilitySmallThreshold) //prob 0.2
      finalColor.g = AlphaSmallThreshold;
  }
  FragColor = finalColor;
}
