#version 330 core
in vec3 position;
in vec2 tex_coord;
in vec3 normal;

out vec3 v_specular;
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
  gl_Position = P * M * V * frag_coord;

  vec3 N = vec3(itM * vec4(normal, 1.0));
  vec3 L = normalize(u_light_pos - frag_coord.xyz);
  vec3 V = normalize(u_view_pos - frag_coord.xyz);
  vec3 R = reflect(-L, N); // max (2 * dot(N,L) * N - L , 0.0) ;
  float cosTheta = dot(R, V);
  float spec = pow(max(cosTheta, 0.0), 32.0);
  float specular = spec_strength * spec;
  v_specular = vec3(specular);
}