#version 330 core
in vec3 position; 
in vec2 tex_coord; 
in vec3 normal; 



out vec3 v_frag_coord; 
out vec3 v_normal; 
out vec2 tex_vcoord;
out vec3 v_frag_coord; 

uniform vec3 u_light_pos;
uniform vec3 u_view_pos;

uniform mat4 M; 
uniform mat4 itM; 
uniform mat4 V; 
uniform mat4 P; 


void main(){ 
  vec4 frag_coord = M * vec4(position, 1.0);
  surface_normal = frag_coord.xyz;
  to_light = u_light_pos - frag_coord.xyz;
  to_camera = (inverse(V) * vec4(0.0,0.0,0.0,1.0)).xyz - frag_coord.xyz;
  tex_vcoord = tex_coord;
  gl_Position = P*V*frag_coord; 
  v_frag_coord = frag_coord.xyz; 
};