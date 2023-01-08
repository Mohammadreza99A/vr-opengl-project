#ifndef SUN_H
#define SUN_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"

class Sun
{

private:
    Shader *shader_sun;
    Object *sun;
    GLuint sun_texture_id;

public:
    Sun();

    void cleanup();

    void draw(const glm::mat4 &view, const glm::mat4 &projection,
              const glm::vec3 &camera_position, const glm::vec3 &light_pos,
              glm::vec3 &delta, const glm::vec3 &color);

    void initTexture(std::string path);
    void turn(glm::vec3 &delta);

    void bindAllTexture();
};
#endif