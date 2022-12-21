#version 330 core
out vec4 FragColor;
precision mediump float;

in vec2 tex_vcoord;
uniform sampler2D texture1;
in vec3 v_specular;
uniform vec3 materialColour; 

void main() { 
    vec4 frag_light = vec4(materialColour , 1.0);
    FragColor = frag_light;
     } 

