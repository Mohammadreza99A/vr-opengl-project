#include "light.h"

Light::Light(float shininess, float ambient, float diffuse, float specular, glm::vec3 materialColour)
{

    this->shininess = shininess;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;

    this->materialColour = materialColour; // glm::vec3(1.0f, 1.0f, 0.9f)
}

void Light::setLight(Shader *shader_light)
{

    shader_light->use();
    shader_light->setFloat("shininess", this->shininess);
    shader_light->setVector3f("materialColour", this->materialColour);
    shader_light->setFloat("light.ambient_strength", this->ambient);
    shader_light->setFloat("light.diffuse_strength", this->diffuse);
    shader_light->setFloat("light.specular_strength", this->specular);
    shader_light->setFloat("light.constant", 1.0);
    shader_light->setFloat("light.linear", 0.14);
    shader_light->setFloat("light.quadratic", 0.07);
    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

void Light::cleanup()
{
    shader_light->cleanup();
}
