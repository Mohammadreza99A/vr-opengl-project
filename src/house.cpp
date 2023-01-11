#include "house.h"

House::House()
{
    shader = new Shader(PATH_TO_SHADERS "/shadowVertexShader.glsl", PATH_TO_SHADERS "/shadowFragShader.glsl");

    char path[] = PATH_TO_OBJECTS "/farm_house.obj";
    house = new Object(path);
    house->makeObject(*shader, true, false, true);
    house->model = glm::translate(house->model, glm::vec3(5.0, 5.0, -30.0));
    house->model = glm::rotate(house->model, glm::radians(25.f), glm::vec3(0.0, 1.0, 0.0));
    house->model = glm::scale(house->model, glm::vec3(0.5, 0.5, 0.5));

    // load terrain textures
    initTexture(PATH_TO_TEXTURE "/house/house_texture.jpg");

    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

Shader *House::getShader()
{
    return this->shader;
}

void House::cleanup()
{
    shader->cleanup();
    glDeleteTextures(1, &house_texture_id);
}

void House::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos)
{

    shader->use();

    bindAllTexture();

    // 1. send the relevant uniform to the shader
    shader->setMatrix4("M", house->model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(house->model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setVector3f("u_view_pos", camera_position);
    shader->setVector3f("light.light_pos", light_pos);
    shader->setInteger("f_texture", 0);

    house->draw();

    glBindVertexArray(0);
    glUseProgram(0);
}

void House::drawShadow(const glm::mat4 &view, const glm::mat4 &projection,
                       const glm::vec3 &camera_position, const glm::vec3 &light_pos, GLuint depth_fbo)
{

    shader->use();
    glClearDepth(1.0f);

    // Bind the "depth only" FBO and set the viewport to the size
    // of the depth texture
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    glViewport(0, 0, 1024, 720);

    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable polygon offset to resolve depth-fighting isuses
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    // 1. send the relevant uniform to the shader
    shader->setMatrix4("M", house->model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(house->model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setVector3f("u_view_pos", camera_position);
    shader->setVector3f("light.light_pos", light_pos);
    shader->setInteger("f_texture", 0);

    house->draw();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void House::initTexture(std::string path)
{

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &house_texture_id);
    glBindTexture(GL_TEXTURE_2D, house_texture_id);

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

void House::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, house_texture_id);
}
