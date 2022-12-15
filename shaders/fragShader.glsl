#version 330 core
out vec4 FragColor;
precision mediump float;

in vec2 tex_vcoord;
uniform sampler2D f_texture;
in vec3 v_specular;

void main() { FragColor = texture(f_texture, tex_vcoord); }