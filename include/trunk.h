#ifndef TRUNK_H
#define TRUNK_H

#include <glad/glad.h>
#include <vector>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "transform.h"

class Trunk
{

public:
    Trunk();
    ~Trunk();
    void init(GLuint pid, GLuint _vao);
    void create_positions();
    void add_sub_trunks_lsystem(Transform t, uint level, uint max_level, bool is_end_point);
    float lerp(float val, float min, float max);
    void add_sub_trunks_lsystem_random(Transform t, uint level, uint max_level, bool is_end_point);
    // simple trunk model, fractal-like but unrealistic
    // this is not really a l-system
    void add_sub_trunks_fractal(Transform t, uint level, uint max_level);
    // to be added to a drawing list by the caller
    std::vector<glm::mat4> get_transf();
    uint indices_to_draw();
    std::vector<glm::mat4> get_end_point_matrices();
    void draw();

private:
    GLuint _pid;

    glm::mat4x4 model_matrix;
    glm::mat4x4 view_matrix;
    glm::mat4x4 projection_matrix;

    GLfloat light_position[3];

    bool enabled;

    GLuint _vao;
    GLuint _vbo_pos;
    GLuint _vbo_sur_norm;
    GLuint _vbo_idx;
    GLuint _vbo_tex;

    std::vector<Transform> transf;
    std::vector<Transform> end_point_matrices; // points where the twigs/trunks stops, to add leaves
    std::vector<float> rand_vals;              // list of random values

    GLfloat *positions;
    uint nb_positions;

    GLuint *indices;
    uint nb_indices;

    GLfloat *text_coord;
    uint nb_text_coord;

    GLfloat *normals;
    uint nb_normals;
};

#endif