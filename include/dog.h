#ifndef DOG_H
#define DOG_H

#include <cstdlib>
#include "shader.h"
#include "object.h"
#include "stb_image.h"
#include "vector"

class Dog
{

private:
    Object *dog;
    Shader *shader;
    GLuint dog_texture_id;
    int current_frame;
    int num_frames;
    std::vector<const char *> obj_paths;


public:
    Dog();
    Shader *getShader();
    void cleanup();
    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos,double current_frame);
    void initTexture(std::string path);
    void walk(double current_frame);


    void bindAllTexture();
};

#endif