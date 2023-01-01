#include "terrain.h"

Terrain::Terrain(unsigned int sub_x, unsigned int sub_y): MatrixSquares(sub_x, sub_y)
{
    shader = new Shader(PATH_TO_SHADERS "/terrainV.glsl", PATH_TO_SHADERS "/terrainF.glsl");
    _pid = shader->ID;
    if (_pid == 0)
        exit(-1);
}

void Terrain::init()
{
    MatrixSquares::init();
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
    generate_heightmap(&Terrain::function_flat, sub_x, sub_y);

    // fills the buffers
    generate_terrain();

    fill_buffers();

    texture_id=initTexture(PATH_TO_TEXTURE "/terrain/terrain_snow.jpg");
    

    glBindVertexArray(0);
}

void Terrain::draw(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection,
                   glm::vec3 light_position, glm::vec3 camera_position)
{

    
    shader->use();
    bindAllTexture();
    glBindVertexArray(_vao);

    shader->setVector3f("u_view_pos", camera_position);
    shader->setVector3f("u_light_pos", light_position);

    shader->setMatrix4("M", model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setInteger("texture1", 0);

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

void Terrain::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Terrain::generate_terrain()
{
    for (uint j = 0; j < sub_y; j++)
    {
        for (uint i = 0; i < sub_x; i++)
        {

            // makes that the relatives go from -1 to 1
            GLfloat relative_x = (float(i) / (sub_x - 1)) * SIZE -1 ;
            // rel_z not y as y is up
            GLfloat relative_z = (float(j) / (sub_y - 1)) * SIZE -1;

            unsigned int cur_pos = (j * sub_x + i) * 3;

            vertices[cur_pos] = relative_x;
            vertices[cur_pos + 1] = 0; //(*this.*func)(relative_x, relative_z);
            vertices[cur_pos + 2] = relative_z;

            textures[((j * sub_x + i) * 2)] = (float(i) / (sub_x - 1))*70;
            textures[((j * sub_x + i) * 2) + 1] = (float(j) / (sub_y - 1))*70;

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
            GLfloat relative_x = (float(i) / (grid_x - 1)) * SIZE -1;
            // y dir is up
            GLfloat relative_z = (float(j) / (grid_y - 1)) * SIZE -1;

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