#include "terrain.h"

Terrain::Terrain()
{
    shader = new Shader(PATH_TO_SHADERS "/terrainV.glsl", PATH_TO_SHADERS "/terrainF.glsl");
    _pid = shader->ID;
    if (_pid == 0)
        exit(-1);
}

void Terrain::init(unsigned int sub_x, unsigned int sub_y)
{

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    this->sub_x = sub_x;
    this->sub_y = sub_y;
    nb_vertices = sub_x * sub_y;
    unsigned int nb_quads = (sub_x - 1) * (sub_y - 1);
    unsigned int nb_tris = nb_quads * 2;
    nb_indices = nb_tris * 3;

    vertices = new GLfloat[nb_vertices * 3];
    indices = new GLuint[nb_indices];
    normals = new GLfloat[nb_vertices * 3];
    textures = new GLfloat[nb_vertices * 2];

    // fill the indices array
    set_indices();

    heightmap.resize(sub_x);
    heightmap_normals.resize(sub_x);
    for (unsigned int i = 0; i < sub_x; i++)
    {
        heightmap[i].resize(sub_y);
        heightmap_normals[i].resize(sub_y);
        for (unsigned int j = 0; j < sub_y; j++)
        {
            heightmap_normals[i][j].resize(3);
        }
    }

    // generates heightmap as well as normals
    generate_heightmap(&Terrain::function_sin, sub_x, sub_y);

    // fills the buffers
    generate_terrain();

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

    initTexture(PATH_TO_TEXTURE "/terrain/terrain_snow.jpg");

    glBindVertexArray(0);
}

void Terrain::draw(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection,
                   glm::vec3 light_position, glm::vec3 camera_position)
{

    bindAllTexture();
    shader->use();
    glBindVertexArray(_vao);

    shader->setVector3f("u_view_pos", camera_position);
    shader->setVector3f("u_light_pos", light_position);

    shader->setMatrix4("M", model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setInteger("f_texture", 0);

    glDrawElements(GL_TRIANGLES, nb_indices, GL_UNSIGNED_INT, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

void Terrain::cleanup()
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

float Terrain::get_height(float pos_x, float pos_y)
{
    // transform back the point to (1,1)
    glm::vec4 vec(pos_x, 1.0f, pos_y, 1.0f);
    glm::mat4 transf_matrix = glm::scale(glm::mat4(1.0), glm::vec3(256.0, 0.0, 256.0));

    glm::vec4 vec_transf = glm::inverse(transf_matrix) * vec;

    // get height for the point
    float height = function_flat(vec_transf[0], vec_transf[2]);
    height = (transf_matrix * glm::vec4(0.0f, height, 0.0f, 1.0f))[1];
    return height;
}

void Terrain::initTexture(char const *path)
{

    glGenTextures(1, &terrain_texture_id);

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

        glBindTexture(GL_TEXTURE_2D, terrain_texture_id);
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
}

void Terrain::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrain_texture_id);
}

void Terrain::generate_terrain()
{
    for (uint j = 0; j < sub_y; j++)
    {
        for (uint i = 0; i < sub_x; i++)
        {

            // makes that the relatives go from -1 to 1
            GLfloat relative_x = (float(i) / (sub_x - 1)) * 2 - 1;
            // rel_z not y as y is up
            GLfloat relative_z = (float(j) / (sub_y - 1)) * 2 - 1;

            unsigned int cur_pos = (j * sub_x + i) * 3;

            vertices[cur_pos] = relative_x;
            vertices[cur_pos + 1] = heightmap[i][j]; //(*this.*func)(relative_x, relative_z);
            vertices[cur_pos + 2] = relative_z;

            textures[((j * sub_x + i) * 2)] = (float(i) / (sub_x - 1)) * 70;
            textures[((j * sub_x + i) * 2) + 1] = (float(j) / (sub_y - 1)) * 70;

            normals[cur_pos + 0] = heightmap_normals[i][j][0]; // cross_product_vec[0];
            normals[cur_pos + 1] = heightmap_normals[i][j][1];
            normals[cur_pos + 2] = heightmap_normals[i][j][2];
        }
    }
}

void Terrain::generate_heightmap(float (Terrain::*func)(float, float), unsigned int grid_x, unsigned int grid_y)
{
    const float EPSILON = 0.0001;

    for (uint j = 0; j < grid_x; j++)
    {
        for (uint i = 0; i < grid_y; i++)
        {

            // makes that the relatives go from -1 to 1
            GLfloat relative_x = (float(i) / (grid_x - 1)) * 2 - 1;
            // y dir is up
            GLfloat relative_z = (float(j) / (grid_y - 1)) * 2 - 1;

            // fill height map for each vertex
            heightmap[i][j] = (*this.*func)(relative_x, relative_z);

            // save normals
            // derivative_x[1] = (*this.*func)(relative_x+EPSILON, relative_z)-(*this.*func)(relative_x, relative_z);
            float derivative_x[3];
            float derivative_z[3];

            // calculate derivatives for each point of the grid using epsilon (far from optimized)
            derivative_x[0] = 2 * EPSILON;
            derivative_x[1] = (*this.*func)(relative_x + EPSILON, relative_z) - (*this.*func)(relative_x - EPSILON, relative_z);
            derivative_x[2] = 0.0f;

            normalize_vec3(derivative_x);

            derivative_z[0] = 0.0f;
            derivative_z[1] = (*this.*func)(relative_x, relative_z + EPSILON) - (*this.*func)(relative_x, relative_z - EPSILON);
            derivative_z[2] = 2 * EPSILON;

            normalize_vec3(derivative_z);

            float cross_product_vec[3];

            cross_product(derivative_z, derivative_x, cross_product_vec);

            heightmap_normals[i][j][0] = cross_product_vec[0];
            heightmap_normals[i][j][1] = cross_product_vec[1];
            heightmap_normals[i][j][2] = cross_product_vec[2];
        }
    }
}

void Terrain::normalize_vec3(float vec[3])
{
    float length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);

    vec[0] = vec[0] / length;
    vec[1] = vec[1] / length;
    vec[2] = vec[2] / length;
}

void Terrain::cross_product(float vec_a[3], float vec_b[3], float vec_out[3])
{
    vec_out[0] = vec_a[1] * vec_b[2] - vec_a[2] * vec_b[1];
    vec_out[1] = vec_a[2] * vec_b[0] - vec_a[0] * vec_b[2];
    vec_out[2] = vec_a[0] * vec_b[1] - vec_a[1] * vec_b[0];
}

void Terrain::set_indices()
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

            indices[index_ptr] = v0;
            indices[index_ptr + 1] = v3;
            indices[index_ptr + 2] = v1;

            indices[index_ptr + 3] = v0;
            indices[index_ptr + 4] = v2;
            indices[index_ptr + 5] = v3;
        }
    }
}