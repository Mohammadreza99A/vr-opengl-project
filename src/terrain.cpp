#include "terrain.h"

Terrain::Terrain()
{
    shader = new Shader("shaders/terrainV.glsl", "shaders/terrainF.glsl");
    _pid = shader->ID;
    if (_pid == 0)
        exit(-1);
}

void Terrain::init(unsigned int gridX, unsigned int gridZ)
{

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    int count = this->VERTEX_COUNT * this->VERTEX_COUNT;
    this->x = gridX * this->SIZE;
    this->z = gridZ * this->SIZE;
    this->nb_vertices = count * 3;
    this->nb_indices = 6 * (this->VERTEX_COUNT - 1) * (this->VERTEX_COUNT - 1);

    vertices = new GLfloat[this->nb_vertices];
    normals = new GLfloat[this->nb_vertices];
    indices = new GLuint[this->nb_indices];
    textureCoords = new GLfloat[count * 2];

    // fills the buffers
    generate_terrain();
    // fill the indices array
    set_indices();

    glGenBuffers(1, &_vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

    GLuint norm_id = glGetAttribLocation(_pid, "normal");
    glEnableVertexAttribArray(norm_id);
    glVertexAttribPointer(norm_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

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
    shader->setVector3f("light_color", glm::vec3(0.5, 0.5, 0.5));
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

    if (textureCoords != NULL)
    {
        delete textureCoords;
        textureCoords = NULL;
    }

    if (normals != NULL)
    {
        delete normals;
        normals = NULL;
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

    int vertexPointer = 0;

    for (size_t i = 0; i < this->VERTEX_COUNT; i++)
    {
        for (size_t j = 0; j < this->VERTEX_COUNT; j++)
        {
            GLfloat relX = (float)j / ((float)this->VERTEX_COUNT - 1);
            GLfloat relZ = (float)i / ((float)this->VERTEX_COUNT - 1);

            this->vertices[vertexPointer * 3] = relX * this->SIZE;
            this->vertices[vertexPointer * 3 + 1] = 0;
            this->vertices[vertexPointer * 3 + 2] = relZ * this->SIZE;
            this->normals[vertexPointer * 3] = 0;
            this->normals[vertexPointer * 3 + 1] = 1;
            this->normals[vertexPointer * 3 + 2] = 0;
            this->textureCoords[vertexPointer * 2] = relX;
            this->textureCoords[vertexPointer * 2 + 1] = relZ;
            vertexPointer++;
        }
    }
}

void Terrain::set_indices()
{
    int pointer = 0;
    for (size_t gz = 0; gz < this->VERTEX_COUNT - 1; gz++)
    {
        for (size_t gx = 0; gx < this->VERTEX_COUNT - 1; gx++)
        {
            int topLeft = (gz * this->VERTEX_COUNT) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * this->VERTEX_COUNT) + gx;
            int bottomRight = bottomLeft + 1;
            this->indices[pointer++] = topLeft;
            this->indices[pointer++] = bottomLeft;
            this->indices[pointer++] = topRight;
            this->indices[pointer++] = topRight;
            this->indices[pointer++] = bottomLeft;
            this->indices[pointer++] = bottomRight;
        }
    }
}