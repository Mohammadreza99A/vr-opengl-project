#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec3 v_frag_coord; 
in vec3 v_normal; 

uniform vec3 u_view_pos; 

//In GLSL you can use structures to better organize your code


uniform vec3 materialColour; 
uniform samplerCube cubemapSampler;
uniform float refractionIndice;



void main() { 
float ratio = 1.00 / refractionIndice;
vec3 N = normalize(v_normal);
vec3 V = normalize(u_view_pos - v_frag_coord); 
vec3 R = refract(-V,N,ratio); 

FragColor = mix(texture(cubemapSampler,R),vec4(materialColour, 1.0),0.3); 
} 