#version 330 core
out vec4 FragColor;
precision mediump float;

in vec2 tex_vcoord;
uniform sampler2D f_texture;

void main() { FragColor = texture(f_texture, tex_vcoord); }