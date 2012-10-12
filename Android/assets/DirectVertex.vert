precision mediump float;

attribute vec4 in_Vertex;

uniform mat4 in_ModelViewProjectionMatrix;

void main()
{
	gl_Position = in_ModelViewProjectionMatrix * in_Vertex;
}