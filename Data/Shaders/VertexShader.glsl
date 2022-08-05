#version 460

in vec3 aPosition;
in vec2 aTextureCoordinates;

uniform mat4 uModelViewProjection;

out vec2 vTextureCoordinates;

void main()
{
	gl_Position = uModelViewProjection * vec4(aPosition, 1.0);
	vTextureCoordinates = aTextureCoordinates;
}