#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //perspective

// interface for a drawable element such as a cube or a sphere
// inspired by https://github.com/damdoy/opengl_examples/blob/master/common/drawable.h
class Drawable
{

public:
    Drawable();
    virtual void set_model_matrix(glm::mat4x4 model);
    virtual void set_view_matrix(glm::mat4x4 view);
    virtual void set_projection_matrix(glm::mat4x4 projection);
    virtual void set_light_pos(glm::vec3  light_position);
    virtual void set_camera_pos(glm::vec3  camera_position);
    virtual void set_camera_direction(GLfloat camera_direction[3]);
    virtual void set_MVP_matrices(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection);
    virtual void set_shadow_matrix(glm::mat4x4 shadow_mat);
    virtual void set_shadow_buffer_texture(GLuint sh_tex);
    virtual void set_shadow_buffer_texture_size(unsigned int shadow_buffer_tex_size);
    virtual void set_shadow_buffer_texture_size(unsigned int shadow_buffer_tex_width, unsigned int shadow_buffer_tex_height);
    virtual void set_shadow_mapping_effect(unsigned int shadow_mapping_effect);
    virtual void set_window_dim(unsigned int win_width, unsigned int win_height);
    virtual void set_shader(GLuint shader_pid);
    virtual GLuint get_shader();
    virtual void set_clip_coord(float x, float y, float z, float d);
    virtual void set_enabled(bool enabled);
    virtual void set_sun_dir(float sun_dir[3]);
    virtual void set_sun_col(float sun_col[3]);
    virtual void draw() = 0;

protected:
    GLuint _pid;

    glm::mat4x4 model_matrix;
    glm::mat4x4 view_matrix;
    glm::mat4x4 projection_matrix;

    glm::mat4x4 shadow_matrix;

    GLfloat light_position[3];
    GLfloat camera_position[3];
    GLfloat camera_direction[3];
    GLfloat clip_coord[4];

    GLfloat sun_dir[3];
    GLfloat sun_col[3];

    bool has_shadow_buffer;
    GLuint _shadow_texture_id;

    bool enabled;

    unsigned shadow_mapping_effect;
    unsigned window_width;
    unsigned window_height;
    unsigned int shadow_buffer_texture_width;
    unsigned int shadow_buffer_texture_height;
};

#endif
