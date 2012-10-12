precision mediump float;

varying vec2 inout_TexCoord;
varying highp vec4 inout_VertexPosProjInLight;

uniform sampler2D in_TexUnit2D;
uniform highp sampler2D in_ShadowTexUnit;

void main()
{
	vec4 texel = texture2D(in_TexUnit2D, inout_TexCoord);
	
	//if( IsPointInShadow(inout_VertexPosProjInLight) )
	//	texel *= CastShadow(in_ShadowTexUnit, inout_VertexPosProjInLight);
	
	gl_FragColor = texel;
}