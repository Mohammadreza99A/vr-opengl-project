#include "drawable.h"

Drawable::Drawable()
{
    has_shadow_buffer = false;
    shadow_mapping_effect = 0;
    this->window_width = 0;
    this->window_height = 0;
    this->enabled = true;
}

void Drawable::set_model_matrix(glm::mat4x4 model)
{
    this->model_matrix = model;
}

void Drawable::set_view_matrix(glm::mat4x4 view)
{
    this->view_matrix = view;
}

void Drawable::set_projection_matrix(glm::mat4x4 projection)
{
    this->projection_matrix = projection;
}

void Drawable::set_light_pos(GLfloat light_position[3])
{
    this->light_position[0] = light_position[0];
    this->light_position[1] = light_position[1];
    this->light_position[2] = light_position[2];
}

void Drawable::set_camera_pos(GLfloat camera_position[3])
{
    this->camera_position[0] = camera_position[0];
    this->camera_position[1] = camera_position[1];
    this->camera_position[2] = camera_position[2];
}

void Drawable::set_camera_direction(GLfloat camera_direction[3])
{
    this->camera_direction[0] = camera_direction[0];
    this->camera_direction[1] = camera_direction[1];
    this->camera_direction[2] = camera_direction[2];
}

void Drawable::set_MVP_matrices(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection)
{
    this->set_model_matrix(model);
    this->set_view_matrix(view);
    this->set_projection_matrix(projection);
}

void Drawable::set_shadow_matrix(glm::mat4x4 shadow_mat)
{
    this->shadow_matrix = shadow_mat;
}

void Drawable::set_shadow_buffer_texture(GLuint sh_tex)
{
    has_shadow_buffer = true;
    _shadow_texture_id = sh_tex;
}

void Drawable::set_shadow_buffer_texture_size(unsigned int shadow_buffer_tex_size)
{
    shadow_buffer_texture_width = shadow_buffer_tex_size;
    shadow_buffer_texture_height = shadow_buffer_tex_size;
}

void Drawable::set_shadow_buffer_texture_size(unsigned int shadow_buffer_tex_width, unsigned int shadow_buffer_tex_height)
{
    shadow_buffer_texture_width = shadow_buffer_tex_width;
    shadow_buffer_texture_height = shadow_buffer_tex_height;
}

void Drawable::set_shadow_mapping_effect(unsigned int shadow_mapping_effect)
{
    this->shadow_mapping_effect = shadow_mapping_effect;
}

void Drawable::set_window_dim(unsigned int win_width, unsigned int win_height)
{
    this->window_width = win_width;
    this->window_height = win_height;
}

void Drawable::set_shader(GLuint shader_pid)
{
    this->_pid = shader_pid;
}

GLuint Drawable::get_shader()
{
    return this->_pid;
}

void Drawable::set_clip_coord(float x, float y, float z, float d)
{
    this->clip_coord[0] = x;
    this->clip_coord[1] = y;
    this->clip_coord[2] = z;
    this->clip_coord[3] = d;
}

void Drawable::set_enabled(bool enabled)
{
    this->enabled = enabled;
}

void Drawable::set_sun_dir(float sun_dir[3])
{
    this->sun_dir[0] = sun_dir[0];
    this->sun_dir[1] = sun_dir[1];
    this->sun_dir[2] = sun_dir[2];
}

void Drawable::set_sun_col(float sun_col[3])
{
    this->sun_col[0] = sun_col[0];
    this->sun_col[1] = sun_col[1];
    this->sun_col[2] = sun_col[2];
}