#include "light.h"

Light::Light(float shininess, float ambient, float diffuse, float specular, glm::vec3 materialColour)
{

    this->shininess = shininess;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;

    this->materialColour = materialColour; // glm::vec3(1.0f, 1.0f, 0.9f)
}

void Light::on(Shader *shader)
{

    shader->use();
    shader->setFloat("shininess", this->shininess);
    shader->setVector3f("materialColour", this->materialColour);
    shader->setFloat("light.ambient_strength", this->ambient);
    shader->setFloat("light.diffuse_strength", this->diffuse);
    shader->setFloat("light.specular_strength", this->specular);
    shader->setFloat("light.constant", 1.0);
    shader->setFloat("light.linear", 0.14);
    shader->setFloat("light.quadratic", 0.07);
    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

void Light::cleanup()
{
    shader_light->cleanup();
}