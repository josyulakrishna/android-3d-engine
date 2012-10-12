
varying vec3 inout_VertexPosition;
varying vec3 inout_VertexNormal;
varying vec4 inout_VertexColor;
//varying vec2 inout_TexCoord;

// Light 0: Ambient
const vec4 AmbientIntensity = vec4(0.1, 0.1, 0.1, 1);

// Light 1: directional light
uniform vec3 in_LightDir; // in camera-space
const vec4 LightIntensity = vec4(0.7, 0.7, 0.7, 1);
const vec4 SpecularColor = vec4(0.25, 0.25, 0.25, 1);
const float ShininessFactor = 50.0;

vec4 ComputeLight(in vec3 lightDir, vec4 lightIntensity)
{
	vec3 normalDir = normalize(inout_VertexNormal);
	
	// compute the angle of incidence between the light and the surface;
	// the surface is represented by the (interpolated and normalized) normal at the point
	float cosAngIncidence = dot(normalDir, lightDir);
	cosAngIncidence = cosAngIncidence < 0.0001 ? 0.0 : cosAngIncidence;

	// optinal specular calculations
	float specularTerm = ApplySpecular(lightDir, normalDir, inout_VertexPosition,
		cosAngIncidence, ShininessFactor);
	
	return inout_VertexColor * lightIntensity * cosAngIncidence
		+ (SpecularColor * lightIntensity * specularTerm)
	;
}

void main()
{
	vec4 finalColor;
	
	// Light 0: Ambient
	finalColor = inout_VertexColor * AmbientIntensity;
	
	// Light 1
	finalColor += ComputeLight(in_LightDir, LightIntensity);//ApplyAttenuation(LightIntensity, in_LightDir, inout_VertexPosition));

	gl_FragColor = ApplyGamma(finalColor, 2.2);
}
