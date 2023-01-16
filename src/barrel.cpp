#include "barrel.h"

Barrel::Barrel()
{
    shader = new Shader(PATH_TO_SHADERS "/normalV.glsl", PATH_TO_SHADERS "/normalF.glsl");

    char path[] = PATH_TO_OBJECTS "/barrel.obj";
    barrel = new Object(path);
    barrel->makeObject(*shader, true, true);

    barrel->model = glm::translate(barrel->model, glm::vec3(0.0, 2.2, -35.0));
    // barrel->model = glm::rotate(barrel->model, glm::radians(25.f), glm::vec3(0.0, 1.0, 0.0));
    barrel->model = glm::scale(barrel->model, glm::vec3(0.2, 0.2, 0.2));

    // load terrain textures
    diffuseMap = Tools::initTexture(PATH_TO_TEXTURE "/barrel/barrel.png");
    normalMap = Tools::initTexture(PATH_TO_TEXTURE "/barrel/barrel_normal.png");

    float ambient = 0.2;
    float diffuse = 0.2;
    float specular = 0.2;

    shader->use();
    shader->setInteger("diffuseMap", 0);
    shader->setInteger("normalMap", 1);

    shader->setFloat("shininess", 10.0f);

    shader->setFloat("light.ambient_strength", ambient);
    shader->setFloat("light.diffuse_strength", diffuse);
    shader->setFloat("light.specular_strength", specular);
    shader->setFloat("light.constant", 0.5);
    shader->setFloat("light.linear", 0.3);
    shader->setFloat("light.quadratic", 0.07);
    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

void Barrel::cleanup()
{
    shader->cleanup();
    glDeleteTextures(1, &diffuseMap);
    glDeleteTextures(1, &normalMap);
}

void Barrel::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos)
{

    shader->use();

    bindAllTexture();

    shader->setMatrix4("projection", projection);
    shader->setMatrix4("view", view);
    shader->setMatrix4("model", barrel->model);
    shader->setVector3f("viewPos", camera_position);
    shader->setVector3f("lightPos", light_pos);

    barrel->draw();

    glBindVertexArray(0);
    glUseProgram(0);
}

Shader *Barrel::getShader()
{
    return this->shader;
}

void Barrel::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
}
