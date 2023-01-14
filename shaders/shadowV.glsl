#version 330                                                                        
                                                                                                                                     
in vec3 position;
in vec3 normal;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
}