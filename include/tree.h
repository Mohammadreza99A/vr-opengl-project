#ifndef TREE_H
#define TREE_H

#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "trunk.h"
#include "individual_leaves.h"
#include "noise_generator.h"
#include "texture_float.h"
#include "shader.h"
/**
 * @brief Procedural generation of trees using L-Systems.
 * The trunks are made of cylinder with some sort of simplified brown perlin noise as texture.
 * Leaves are added on the tips of the branches, they are a bunch of green triangles whose corners has been smoothed by the fragment shader.
 * The leaves are added as small spheres of these triangle shapes and the ones that are on the bottom of the sphere are darker in colour to give a sense of shadow and depth.
 * 
 */
class Tree
{
public:
    Tree();
    ~Tree();
    void init();
    void load();
    void set_enabled(bool enabled);
    void draw_trunks();
    void draw_ileaves();
    void move_leaves(float time_delta);
    void draw();
    void setMVP(glm::mat4 model, glm::mat4x4 view, glm::mat4 perspective);
    void setLightPos(GLfloat *light_position);

private:
    GLuint _pid;

    glm::mat4x4 model_matrix;
    glm::mat4x4 view_matrix;
    glm::mat4x4 projection_matrix;

    GLfloat light_position[3];

    bool enabled;

    GLuint _tex_wind_noise;
    NoiseGenerator noise_gen_wind;
    TextureFloat tex_wind_noise;

    Shader *trunk_shader;
    GLuint _pid_trunk;
    GLuint _vao_trunk;

    GLuint _vbo_leaves_transf;
    GLuint _vbo_trunk_transf;
    GLuint _vbo_leaves_base_pos;

    Shader *ileaves_shader;
    GLuint _pid_ileaves;
    GLuint _vao_ileaves;

    uint nb_ileaves_to_draw;
    uint nb_trunks_to_draw;

    uint trunk_nb_indices_to_draw;

    float time_delta;
    float wind_offset[2];

    std::vector<glm::mat4x4> mat_vector_leaves;
    std::vector<glm::mat4x4> mat_vector_trunks;
    std::vector<glm::mat4x4> mat_vector_ileaves;
    std::vector<glm::vec3> pos_vector_ileaves;
};

#endif
