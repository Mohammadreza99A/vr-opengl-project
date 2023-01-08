#include "horse.h"

Horse::Horse(GLuint skyboxID)
{

    // compile the shaders.
    shaderHorse = new Shader(PATH_TO_SHADERS "/statueHorseV.glsl", PATH_TO_SHADERS "/statueHorseF.glsl");

    char path1[] = PATH_TO_OBJECTS "/horse.obj";
    horse = new Object(path1);
    horse->makeObject(*shaderHorse);
    // horse->model = glm::translate(horse->model, glm::vec3(15.0, 0.0, 0.0));
    horse->model = glm::scale(horse->model, glm::vec3(1.5, 1.5, 1.5));

    shaderBase = new Shader(PATH_TO_SHADERS "/vertexShader.glsl", PATH_TO_SHADERS "/fragShader.glsl");

    char path2[] = PATH_TO_OBJECTS "/base.obj";
    base = new Object(path2);
    base->makeObject(*shaderBase);
    // base->model = glm::translate(base->model, glm::vec3(15.0, 0.0, 0.0));
    base->model = glm::scale(base->model, glm::vec3(1.5, 1.5, 1.5));

    // load terrain textures
    initTexture(PATH_TO_TEXTURE "/statue/statue_horse.png");

    render(skyboxID);

    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

void Horse::cleanup()
{
    shaderHorse->cleanup();
    shaderBase->cleanup();
    glDeleteTextures(1, &horse_texture_id);
}

void Horse::render(GLuint skyboxID)
{

    glm::vec3 materialColour = glm::vec3(1.0, 1.0, 1.0);
    float ambient = 0.1;
    float diffuse = 0.5;
    float specular = 0.3;

    shaderHorse->use();
    shaderHorse->setInteger("cubemap_sampler", 0);

    shaderHorse->setFloat("shininess", 40.0f);
    shaderHorse->setVector3f("materialColour", materialColour);
    shaderHorse->setFloat("light.ambient_strength", ambient);
    shaderHorse->setFloat("light.diffuse_strength", diffuse);
    shaderHorse->setFloat("light.specular_strength", specular);
    shaderHorse->setFloat("light.constant", 1.0);
    shaderHorse->setFloat("light.linear", 0.14);
    shaderHorse->setFloat("light.quadratic", 0.07);

    shaderHorse->setVector3f("materialColour", materialColour);

    GLuint cubeMapTexture;
    glGenTextures(1, &cubeMapTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    shaderHorse->setFloat("refractionIndice", 1.63);
}

unsigned int Horse::loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Horse::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos, const glm::vec3 &light_color)
{

    bindAllTexture();

    shaderHorse->use();
    // 1. send the relevant uniform to the shader
    shaderHorse->setMatrix4("M", horse->model);
    shaderHorse->setMatrix4("itM", glm::transpose(glm::inverse(horse->model)));
    shaderHorse->setMatrix4("V", view);
    shaderHorse->setMatrix4("P", projection);
    shaderHorse->setVector3f("light_color", light_color);

    bool color = (rand() % 50) == 2 ? true : false;

    if (color)
    {
        float red = (rand() % 256) / 256.0;
        float green = (rand() % 256) / 256.0;
        float blue = (rand() % 256) / 256.0;
        shaderHorse->setVector3f("material_colour", glm::vec3(red, green, blue));
    }

    // shader_horse->setInteger("f_texture", 0);

    shaderHorse->setVector3f("u_view_pos", camera_position);

    // std::cout << delta.z <<std::endl;
    shaderHorse->setVector3f("light.light_pos", light_pos);
    horse->draw();

    shaderBase->use();
    shaderBase->setMatrix4("M", base->model);
    shaderBase->setMatrix4("itM", glm::transpose(glm::inverse(base->model)));
    shaderBase->setMatrix4("V", view);
    shaderBase->setMatrix4("P", projection);
    shaderBase->setVector3f("u_view_pos", camera_position);
    shaderBase->setVector3f("u_light_pos", light_pos);
    shaderBase->setInteger("f_texture", 0);

    base->draw();

    glBindVertexArray(0);
    glUseProgram(0);
}

void Horse::initTexture(std::string path)
{

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &horse_texture_id);
    glBindTexture(GL_TEXTURE_2D, horse_texture_id);

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

void Horse::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, horse_texture_id);
}