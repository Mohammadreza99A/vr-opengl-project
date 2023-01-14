

#version 330 core
in vec3 position;
in vec3 normal;
in vec2 tex_coord;

out vec2 TexCoords;

void main()
{
    TexCoords = tex_coord;
    gl_Position = vec4(position, 1.0);
}

