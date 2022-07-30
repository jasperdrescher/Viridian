#version 460 core

in vec2 uv;
in vec4 color;

uniform sampler2D tex;

void main()
{
    gl_FragColor = texelFetch(tex, ivec2(uv), 0) * color;
}