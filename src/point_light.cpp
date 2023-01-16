#include "point_light.h"

PointLight::PointLight(glm::vec3 color, float ambient, float diffuse, float specular, glm::vec3 position,
                       float constant, float linear, float quadratic) : color(color), ambient(ambient), diffuse(diffuse), specular(specular), position(position), constant(constant),
                                                                        linear(linear), quadratic(quadratic)
{
}

void PointLight::setLight(Shader *shader_light)
{
    shader_light->use();
    shader_light->setVector3f("point_light.color", this->color);
    shader_light->setVector3f("point_light.position", this->position);
    shader_light->setFloat("point_light.ambient", this->ambient);
    shader_light->setFloat("point_light.diffuse", this->diffuse);
    shader_light->setFloat("point_light.specular", this->specular);
    shader_light->setFloat("point_light.constant", this->constant);
    shader_light->setFloat("point_light.linear", this->linear);
    shader_light->setFloat("point_light.quadratic", this->quadratic);
    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

void PointLight::setColor(Shader *shader_light, glm::vec3 color)
{
    this->color = color;
    shader_light->use();
    shader_light->setVector3f("point_light.color", this->color);
    glBindVertexArray(0);
    glUseProgram(0);
}
