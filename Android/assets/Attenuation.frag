//
// just a simple linear attenuation, should have a more realistic scheme
//
vec4 ApplyAttenuation(in vec4 intensity, in vec3 lightPos, in vec3 vertexPos) // positions are in camera space
{
// method A
// stupid linear method invented by me
	//const float cutOffDist = 10.0;
	//return intensity * (cutOffDist - clamp(distance(lightPos, vertexPos), 0.0, cutOffDist)) / cutOffDist;
	
// method B
// http://msdn.microsoft.com/en-us/library/windows/desktop/bb172279%28v=vs.85%29.aspx
// Atten = 1/( att0i + att1i * d + att2i * d^2)	
// 		where att0-2 are factors
//		and d is the distance between the light and vertex
	float d = distance(lightPos, vertexPos);
	return intensity * (1.0 / (1.0 + 0.5 * d));
}
