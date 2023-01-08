#version 330 core
out vec4 FragColor;
precision mediump float;

in vec2 tex_vcoord;
in vec3 v_specular;

void main() { FragColor = vec4(1.0,1.0,1.0,1.0); }