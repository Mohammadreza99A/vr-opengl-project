#version 330 core
out vec4 FragColor;
precision mediump float;

in vec3 v_specular;

void main()
{
    FragColor = vec4(v_specular, 1.0);
}