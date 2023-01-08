#include "matrix_squares.h"

MatrixSquares::~MatrixSquares()
{
    if (vertices != NULL)
    {
        delete vertices;
        vertices = NULL;
    }

    if (indices != NULL)
    {
        delete indices;
        indices = NULL;
    }

    if (textures != NULL)
    {
        delete textures;
        textures = NULL;
    }

    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteProgram(_pid);
}

void MatrixSquares::normalize_vec3(float vec[3])
{
    float length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);

    vec[0] = vec[0] / length;
    vec[1] = vec[1] / length;
    vec[2] = vec[2] / length;
}

void MatrixSquares::cross_product(float vec_a[3], float vec_b[3], float vec_out[3])
{
    vec_out[0] = vec_a[1] * vec_b[2] - vec_a[2] * vec_b[1];
    vec_out[1] = vec_a[2] * vec_b[0] - vec_a[0] * vec_b[2];
    vec_out[2] = vec_a[0] * vec_b[1] - vec_a[1] * vec_b[0];
}

void MatrixSquares::init()
{
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    unsigned int nb_quads = (sub_x - 1) * (sub_y - 1);
    unsigned int nb_tris = nb_quads * 2;
    nb_indices = nb_tris * 3;

    vertices = new GLfloat[nb_vertices * 3];
    indices = new GLuint[nb_indices];
    normals = new GLfloat[nb_vertices * 3];
    textures = new GLfloat[nb_vertices * 2];

    // fill the indices array
    // set_indices();
    // fill_buffers();
    // glBindVertexArray(0);
}

GLuint MatrixSquares::initTexture(char const *path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void MatrixSquares::fill_buffers()
{

    glGenBuffers(1, &_vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

    GLuint norm_id = glGetAttribLocation(_pid, "normal");
    glEnableVertexAttribArray(norm_id);
    glVertexAttribPointer(norm_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    GLuint vpoint_id = glGetAttribLocation(_pid, "position");
    glEnableVertexAttribArray(vpoint_id);
    glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_vbo_textures);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_textures);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * 2 * sizeof(GLfloat), textures, GL_STATIC_DRAW);

    GLuint texpoint_id = glGetAttribLocation(_pid, "tex_coord");
    glEnableVertexAttribArray(texpoint_id);
    glVertexAttribPointer(texpoint_id, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_vbo_idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void MatrixSquares::set_indices()
{
    for (uint j = 0; j < sub_y - 1; j++)
    {
        for (uint i = 0; i < sub_x - 1; i++)
        {
            unsigned int v0 = j * sub_x + i;
            unsigned int v1 = j * sub_x + i + 1;
            unsigned int v2 = (j + 1) * sub_x + i;
            unsigned int v3 = (j + 1) * sub_x + i + 1;

            unsigned int index_ptr = (j * (sub_x - 1) + i) * 6;
            // printf("%d\n", index_ptr);

            indices[index_ptr] = v0;
            indices[index_ptr + 1] = v3;
            indices[index_ptr + 2] = v1;

            indices[index_ptr + 3] = v0;
            indices[index_ptr + 4] = v2;
            indices[index_ptr + 5] = v3;

            // printf("index: %d, %d, %d || %d, %d, %d\n", v0, v3, v1, v0, v2, v3);
        }
    }
}