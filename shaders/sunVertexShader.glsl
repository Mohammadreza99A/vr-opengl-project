#version 330 core

in vec2 tex_coord;
in vec3 position;

uniform mat4 M;
uniform mat4 itM;
uniform mat4 V;
uniform mat4 P;

out vec2 tex_vcoord;

void main()
{	
	tex_vcoord = tex_coord;
	vec4 frag_coord = M * vec4(position, 1.0);
  	gl_Position = P * M * V * frag_coord;
}