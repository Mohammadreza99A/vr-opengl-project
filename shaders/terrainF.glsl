#version 330 core
precision mediump float;
out vec4 FragColor;

in vec2 tex_vcoord;
uniform sampler2D texture1;
in vec3 v_specular;

void main() { FragColor = texture(texture1, tex_vcoord); }
