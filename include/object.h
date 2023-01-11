#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 Texture;
    glm::vec3 Normal;
    glm::vec3 Tangent;
};

class Object
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;
    std::vector<Vertex> vertices;
    glm::mat4x4 shadow_matrix;
    GLuint _shadow_texture_id;
    GLuint shadow_buffer_texture_width;

    void setShadowBufferTexture(GLuint shadow_texture_id)
    {
        this->_shadow_texture_id = shadow_texture_id;
    }

    void set_shadow_matrix(glm::mat4x4 shadow_mat)
    {
        this->shadow_matrix = shadow_mat;
    }

    void set_shadow_buffer_texture_width(GLuint shadow_buffer_texture_width)
    {
        this->shadow_buffer_texture_width = shadow_buffer_texture_width;
    }

    void set_shadow_buffer_texture(GLuint shadow_text_id)
    {
        this->_shadow_texture_id = shadow_text_id;
    }

    std::vector<int> indices;

    int numVertices;

    GLuint VBO, VAO;

    glm::mat4 model = glm::mat4(1.0);

    Object(const char *path);
    void makeObject(Shader &shader, bool texture = true, bool bump = false, bool has_shadow_buffer = false);
    void calculateTangents(Vertex &v1, Vertex &v2, Vertex &v3);
    void draw();
};
#endif