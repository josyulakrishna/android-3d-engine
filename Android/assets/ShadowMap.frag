precision mediump float;
	
bool IsPointInShadow(in vec4 point) // point is in shadow-eye clip space
{
	const float limit = 1.001;
	if( ((point.x / point.z) < limit) && ((point.y / point.z) < limit) ) { // check to see if within the shadow map
		if( point.w > 0.0 ) { // check for back project	
			return true;
		}
	}
	
	return true;
}

vec4 CastShadow(in sampler2D shadowTexUnit, in vec4 point) // point is in shadow-eye clip space
{
	highp float depthOfVertex = (point.z / point.w) - 0.005; // bias to remove self-shadowing
	highp vec4 shadowMapTexCoord = texture2DProj(shadowTexUnit, point);
	float depthInMap = shadowMapTexCoord.z;
	if( depthOfVertex > depthInMap ) { 
		return vec4(0.75); // in shadow
	}
	
	return vec4(1);
}
