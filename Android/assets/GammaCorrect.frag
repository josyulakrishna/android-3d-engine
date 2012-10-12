//
// perform gamma correction
//
vec4 ApplyGamma(vec4 color, float gammaFactor)
{
// method A
// proper gamma correction
	//vec4 gamma = vec4(1.0 / gammaFactor);
	//gamma.w = 1.0;
	//return pow(color, gamma);

// method B
// cheap gamma correction for gamma=2.0
	//return sqrt(color);
	
// method C
// no gamma correction
	return color;	
}
