//
// Phong specular: light reflected in the direction of the viewer varies based
// on the angle between the view direction and the direction of perfect reflection. 
// Phong term = (View dot Reflect) ^ s
// 		where s = the roughness of the surface
//		smooth surf has large s
//		rough surf has small s
//
float PhongSpecular(vec3 lightDir, vec3 normalDir, vec3 vertexPos, float cosAngIncidence, float shininessFactor)
{
	vec3 viewDir = normalize(- vertexPos); // camera in cam-space is at origin
	vec3 reflectDir = reflect(-lightDir, normalDir);
	float phongTerm = clamp(dot(viewDir, reflectDir), 0.0, 1.0);
	// prevent from having a specular term when the surface normal is oriented away from the light
	phongTerm = cosAngIncidence != 0.0 ? phongTerm : 0.0; 
	phongTerm = pow(phongTerm, shininessFactor);
	
	return phongTerm;
}

//
// Blinn specular: light reflected in the direction of the viewer varies based
// on the difference between the half-angle view direction and the normal. 
// The half-angle vector is the direction halfway between the view direction and the light position.
// Blinn term = (Half-Angle dot Normal) ^ s
// 		where s = the roughness of the surface
//		smooth surf has large s
//		rough surf has small s
//
float BlinnSpecular(vec3 lightDir, vec3 normalDir, vec3 vertexPos, float cosAngIncidence, float shininessFactor)
{
	vec3 viewDir = normalize(- vertexPos); // camera in cam-space is at origin
	vec3 halfAngle = normalize(lightDir + viewDir);
	float blinnTerm = clamp(dot(halfAngle, normalDir), 0.0, 1.0);
	// prevent from having a specular term when the surface normal is oriented away from the light
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor);
	
	return blinnTerm;
}

//
// Gaussian specular: light reflected in the direction of the viewer varies based
// Gaussian term = exponential( -(alpha/m)^2 )
// 		where m = ranges (0, 1] representing an increasingly rougher surface
//		alpha = angle between the surface normal and half-angle vector
//
float GaussianSpecular(vec3 lightDir, vec3 normalDir, vec3 vertexPos, float cosAngIncidence, float shininessFactor)
{
	vec3 viewDir = normalize(- vertexPos); // camera in cam-space is at origin
	vec3 halfAngle = normalize(lightDir + viewDir);
	float angleNormalHalf = acos(dot(halfAngle, normalDir));
	float d = angleNormalHalf / shininessFactor;
	float gaussianTerm = exp( - d * d );
	// prevent from having a specular term when the surface normal is oriented away from the light	
	gaussianTerm = cosAngIncidence != 0.0 ? gaussianTerm : 0.0;
	
	return gaussianTerm;
}

//
// Specular function - choose a method
//
float ApplySpecular(vec3 lightDir, vec3 normalDir, vec3 vertexPos, float cosAngIncidence, float shininessFactor)
{
	return PhongSpecular(lightDir, normalDir, vertexPos, cosAngIncidence, shininessFactor);
}
