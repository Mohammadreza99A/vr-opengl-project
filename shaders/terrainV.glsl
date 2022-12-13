#version 330 core

in vec3 position;
// in vec2 textureCoordinates;
in vec3 normal;

// out vec2 pass_textureCoordinates;
out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 light_position;
uniform vec3 camera_position;

void main(void){

	vec4 worldPosition = model * vec4(position,1.0);
	gl_Position = projection * view * worldPosition;
	// pass_textureCoordinates = textureCoordinates * 40.0;
	
	surfaceNormal = (model * vec4(normal,0.0)).xyz;
	toLightVector = light_position - worldPosition.xyz;
	toCameraVector = (inverse(view) * vec4(0.0,0.0,0.0,1.0)).xyz - worldPosition.xyz;
}