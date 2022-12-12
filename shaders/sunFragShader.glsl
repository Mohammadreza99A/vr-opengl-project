#version 330 core
out vec4 FragColor;
precision mediump float;

in vec2 tex_vcoord;
uniform sampler2D texture1;
in vec3 v_specular;
uniform vec3 sunColour; 

void main() { 
    vec4 frag_light = vec4(sunColour , 1.0);
    FragColor = texture(texture1, tex_vcoord) * frag_light;
     }

