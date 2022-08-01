#version 460

in vec3 a_position;
in vec2 a_texCoord;

uniform mat4 u_projectionMatrix;

out vec2 v_texCoord;

void main()
{
	gl_Position = u_projectionMatrix * vec4(a_position, 1.0);
	v_texCoord = a_texCoord;
}