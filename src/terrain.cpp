#include "terrain.h"

Terrain::Terrain()
{
    shader = new Shader("shaders/terrainV.glsl", "shaders/terrainF.glsl");
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

    // fills the buffers
    generate_terrain();

    glGenBuffers(1, &_vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

    GLuint norm_id = glGetAttribLocation(_pid, "vertex_normal");
    glEnableVertexAttribArray(norm_id);
    glVertexAttribPointer(norm_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    GLuint vpoint_id = glGetAttribLocation(_pid, "position");
    glEnableVertexAttribArray(vpoint_id);
    glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_vbo_idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nb_indices * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Terrain::draw(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection, glm::vec3 light_position, glm::vec3 camera_position)
{
    shader->use();
    glBindVertexArray(_vao);

    shader->setVector3f("light_position", light_position);
    shader->setVector3f("camera_position", camera_position);

    shader->setMatrix4("model", model);
    shader->setMatrix4("view", view);
    shader->setMatrix4("projection", projection);

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

    if (shader != NULL)
    {
        delete shader;
    }

    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
    glDeleteProgram(_pid);
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

            normals[cur_pos + 0] = heightmap_normals[i][j][0]; // cross_product_vec[0];
            normals[cur_pos + 1] = heightmap_normals[i][j][1];
            normals[cur_pos + 2] = heightmap_normals[i][j][2];
        }
    }
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