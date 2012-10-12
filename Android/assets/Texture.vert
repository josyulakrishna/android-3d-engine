precision mediump float;

attribute vec3 in_Position; // in world-space
attribute vec2 in_TexCoord;

uniform mat4 in_ModelViewMatrix;
uniform mat4 in_ProjectionMatrix;
uniform mat4 in_LightMVPMatrix; // matrix that transforms(projects) eye-pos into light clip-space

varying vec2 inout_TexCoord;
varying highp vec4 inout_VertexPosProjInLight; // (eye-space) vertex position projected onto the light's view plane

void main()
{
	vec4 vertexEyePos = in_ModelViewMatrix * vec4(in_Position, 1);
	gl_Position = in_ProjectionMatrix * vertexEyePos;

	inout_VertexPosProjInLight = in_LightMVPMatrix * vertexEyePos;
	inout_TexCoord = in_TexCoord;
}