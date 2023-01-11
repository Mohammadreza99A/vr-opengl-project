#ifndef HOUSE_H
#define HOUSE_H

#include <cstdlib>
#include "shader.h"
#include "object.h"
#include "stb_image.h"

class House
{

private:
    Object *house;
    Shader *shader;
    GLuint house_texture_id;

public:
    House();
    Shader *getShader();
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos);
    void drawShadow(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, GLuint depth_fbo);

    void initTexture(std::string path);

    void bindAllTexture();

    void set_shadow_matrix(glm::mat4x4 shadow_mat)
    {
        house->set_shadow_matrix(shadow_mat);
    }

    void set_shadow_buffer_texture(GLuint shadow_buffer_texture)
    {
        house->set_shadow_buffer_texture(shadow_buffer_texture);
    }

    void set_shadow_buffer_texture_width(GLuint width)
    {
        house->set_shadow_buffer_texture_width(width);
    }
};

#endif