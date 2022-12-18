#include "sun.h"

Sun::Sun()
{

    // compile the shaders.
    shader_light = new Shader(PATH_TO_SHADERS "/lightVertexShader.glsl", PATH_TO_SHADERS "/lightFragShader.glsl");
    char pathToHouse[] = PATH_TO_OBJECTS "/house.obj";
    house = new Object(pathToHouse);
    house->makeObject(*shader_light);
    house->model = glm::translate(house->model, glm::vec3(5.3, 0.0, -30.0));
    house->model = glm::rotate(house->model, glm::radians(25.f), glm::vec3(0.0, 1.0, 0.0));
    house->model = glm::scale(house->model, glm::vec3(0.4, 0.4, 0.4));
    
    shader_sun = new Shader(PATH_TO_SHADERS "/sunVertexShader.glsl", PATH_TO_SHADERS "/sunFragShader.glsl");

    char pathToSun[] = PATH_TO_OBJECTS "/sun.obj";
    sun = new Object(pathToSun);
    sun->makeObject(*shader_sun, false);
    sun->model = glm::scale(sun->model, glm::vec3(2.0f, 2.0f, 1.0f));


    // load terrain textures
    // initTexture(PATH_TO_TEXTURE "/sun/sun_texture.png");
    initTexture(PATH_TO_TEXTURE "/house/house_texture.jpg");

    float ambient = 0.5;
    float diffuse = 1.0;
    float specular = 0.8;

    glm::vec3 materialColour = glm::vec3(1.0f, 1.0f, 0.9f);

    // Rendering

    shader_light->use();
    shader_light->setFloat("shininess", 32.0f);
    shader_light->setVector3f("materialColour", materialColour);
    shader_light->setFloat("light.ambient_strength", ambient);
    shader_light->setFloat("light.diffuse_strength", diffuse);
    shader_light->setFloat("light.specular_strength", specular);
    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

void Sun::cleanup()
{
    shader_light->cleanup();
    shader_sun->cleanup();
    glDeleteTextures(1, &sun_texture_id);
}

void Sun::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, glm::vec3 delta, const glm::vec3 &color)
{

    // shader_light->use();

    bindAllTexture();

    shader_light->use();
    // 1. send the relevant uniform to the shader
    shader_light->setMatrix4("M", house->model);
    shader_light->setMatrix4("itM", glm::transpose(glm::inverse(house->model)));
    shader_light->setMatrix4("V", view);
    shader_light->setMatrix4("P", projection);
    shader_light->setVector3f("u_view_pos", camera_position);
    shader_light->setVector3f("light.light_pos", delta);

    house->draw();

    shader_sun->use();
    shader_sun->setMatrix4("M", sun->model);
    shader_sun->setMatrix4("itM", glm::transpose(glm::inverse(sun->model)));
    shader_sun->setMatrix4("V", view);
    shader_sun->setMatrix4("P", projection);
    shader_sun->setVector3f("u_view_pos", camera_position);
    shader_sun->setVector3f("materialColour", color);
    turn(delta);
    sun->draw();
    
    glBindVertexArray(0);
    glUseProgram(0);
}

void Sun::initTexture(std::string path)
{

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &sun_texture_id);
    glBindTexture(GL_TEXTURE_2D, sun_texture_id);

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

void Sun::turn(glm::vec3 delta)
{
    sun->model = glm::translate(sun->model, delta);
}

void Sun::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sun_texture_id);
}
