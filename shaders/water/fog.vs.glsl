#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out VS_OUT {
    vec2 uv;
} vs_out;

void main()
{
    vs_out.uv = uv;
    gl_Position = vec4(pos, 1.0);
}
