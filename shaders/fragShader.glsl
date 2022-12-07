#version 330 core
out vec4 FragColor;
precision mediump float;

in vec2 TexCoord;
uniform sampler2D ourTexture;
in vec3 v_specular;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}