#version 330 core
in vec3 position;
in vec2 tex_coord;
in vec3 normal;

out vec2 tex_vcoord;

uniform mat4 M;
uniform mat4 itM;
uniform mat4 V;
uniform mat4 P;
// 2. which vector do you need and where do you use them ?
uniform vec3 u_light_pos;
uniform vec3 u_view_pos;
float spec_strength = 0.8;

void main() {
  tex_vcoord = tex_coord;
  vec4 frag_coord = M * vec4(position, 1.0);
  gl_Position = P * V * frag_coord;
}