#include "windmill.h"

Windmill::Windmill()
{

    // compile the shaders.
    shader = new Shader(PATH_TO_SHADERS "/vertexShader.glsl", PATH_TO_SHADERS "/fragShader.glsl",NULL);

    char path1[] = PATH_TO_OBJECTS "/mill.obj";
    mill = new Object(path1);
    mill->makeObject(*shader);
    mill->model = glm::translate(mill->model, glm::vec3(15.0, 5.0, -25.0));
    mill->model = glm::rotate(mill->model, glm::radians(-75.f), glm::vec3(0.0, 1.0, 0.0));
    mill->model = glm::scale(mill->model, glm::vec3(0.5, 0.5, 0.5));

    char path2[] = PATH_TO_OBJECTS "/turbines.obj";
    turbines = new Object(path2);
    turbines->makeObject(*shader);
    turbines->model = glm::translate(turbines->model, glm::vec3(15.0, 5.0, -25.0));
    turbines->model = glm::rotate(turbines->model, glm::radians(-75.f), glm::vec3(0.0, 1.0, 0.0));
    turbines->model = glm::scale(turbines->model, glm::vec3(0.5, 0.5, 0.5));

    // load terrain textures
    initTexture(PATH_TO_TEXTURE "/windmill/windmill_diffuse.jpg");

    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

Shader *Windmill::getShader()
{
    return this->shader;
}

void Windmill::cleanup()
{
    shader->cleanup();
    glDeleteTextures(1, &windmill_texture_id);
}

void Windmill::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, float degree)
{

    shader->use();

    bindAllTexture();

    // 1. send the relevant uniform to the shader
    shader->setMatrix4("M", mill->model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(mill->model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setVector3f("u_view_pos", camera_position);
    shader->setVector3f("light.light_pos", light_pos);
    shader->setInteger("f_texture", 0);

    mill->draw();

    shader->setMatrix4("M", turbines->model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(turbines->model)));
    turn(degree);
    turbines->draw();

    glBindVertexArray(0);
    glUseProgram(0);
}

void Windmill::initTexture(std::string path)
{

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &windmill_texture_id);
    glBindTexture(GL_TEXTURE_2D, windmill_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *dataBuffer = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (dataBuffer)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataBuffer);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(dataBuffer);
}

void Windmill::turn(float degree)
{
    turbines->model = glm::rotate(turbines->model, glm::radians(degree), glm::vec3(0.0, 0.0, 1.0));
}

void Windmill::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, windmill_texture_id);
}
