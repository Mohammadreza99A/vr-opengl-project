#ifndef STATUE_HORSE_H
#define STATUE_HORSE_H

#include "shader.h"
#include "object.h"
#include "stb_image.h"
#include <vector>

class Horse
{

private:
    Object *horse;
    Object *base;
    Shader *shaderHorse;
    Shader *shaderBase;
    GLuint horse_texture_id;
    GLuint base_id;
    std::vector<Shader *>shaders;
    glm::vec3 rgb = glm::vec3(1.0);
    glm::vec3 position;

public:
    Horse(GLuint skyboxID);
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, const glm::vec3 &light_color);
    
    glm::vec3 getColor();

    void render();
    void initTexture(std::string path);
    // std::vector<Shader *>getShader();

    void bindAllTexture();
    glm::vec3 get_position();

    Shader *getShader();
};

#endif
