precision mediump float;

attribute vec3 in_Position; // in world-space
attribute vec3 in_Normal; // in world-space
//attribute vec2 in_TexCoord; // in world-space
const vec4 in_Color = vec4(1, 1, 1, 1); // should be an attribute, for tests just const

uniform mat4 in_ModelViewMatrix;
uniform mat4 in_ProjectionMatrix;
uniform mat3 in_NormalsToViewMatrix;
uniform mat4 in_LightMVPMatrix; // matrix that transforms(projects) eye-pos into light clip-space

varying vec3 inout_VertexPosition;
varying vec3 inout_VertexNormal;
varying vec4 inout_VertexColor;
//varying vec2 inout_TexCoord;
varying highp vec4 inout_VertexPosProjInLight; // (eye-space) vertex position projected onto the light's view plane

void main()
{
	vec4 vertexEyePos = in_ModelViewMatrix * vec4(in_Position, 1);
	gl_Position = in_ProjectionMatrix * vertexEyePos;

	inout_VertexPosition = vec3(vertexEyePos); // in camera-space
	inout_VertexNormal = in_NormalsToViewMatrix * in_Normal; // in camera-space
	inout_VertexColor = in_Color;
//	inout_TexCoord = in_TexCoord;
	inout_VertexPosProjInLight = in_LightMVPMatrix * vertexEyePos;
}