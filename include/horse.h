#ifndef STATUE_HORSE_H
#define STATUE_HORSE_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"

class Horse
{

private:
    Object *horse;
    Object *base;
    Shader *shaderHorse;
    Shader *shaderBase;
    GLuint horse_texture_id;

public:
    Horse(GLuint skyboxID);
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, const glm::vec3 &light_color);
    unsigned int loadCubemap(std::vector<std::string> faces);
    void render(GLuint skyboxID);
    void initTexture(std::string path);

    void bindAllTexture();
};

#endif