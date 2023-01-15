#ifndef INDIVIDUAL_LEAVES_H
#define INDIVIDUAL_LEAVES_H

#include <glad/glad.h>
#include <vector>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "transform.h"

class IndividualLeaves
{
public:
    IndividualLeaves();
    ~IndividualLeaves();
    void init(GLuint pid, GLuint _vao);
    void generate();
    void generate(std::vector<glm::mat4> end_point_matrices);
    void build_matrices();
    uint get_nb_leaves_to_draw();
    std::vector<glm::mat4x4> get_mat_vector();
    std::vector<glm::vec3> get_pos_vector();
    void draw();

private:
    GLuint _pid;

    glm::mat4x4 model_matrix;
    glm::mat4x4 view_matrix;
    glm::mat4x4 projection_matrix;

    GLfloat light_position[3];

    bool enabled;

    GLuint _vao;
    GLuint _vbo;
    GLuint _vbo_sur_norm;
    GLuint _vbo_idx;
    GLuint _vbo_tex;

    uint nb_leaves_to_draw;

    std::vector<glm::mat4x4> mat_vector;
    std::vector<glm::vec3> pos_vector;
};

#endif