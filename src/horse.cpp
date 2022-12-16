#include "horse.h"

Horse::Horse(GLuint skyboxID)
{

    // compile the shaders.
    shader_horse = new Shader(PATH_TO_SHADERS "/statueHorseV.glsl", PATH_TO_SHADERS "/statueHorseF.glsl");

    char path1[] = PATH_TO_OBJECTS "/horse.obj";
    horse = new Object(path1);
    horse->makeObject(*shader_horse);
    //horse->model = glm::translate(horse->model, glm::vec3(15.0, 0.0, 0.0));
    horse->model = glm::scale(horse->model, glm::vec3(1.5, 1.5, 1.5));

    shader_norm = new Shader(PATH_TO_SHADERS "/vertexShader.glsl", PATH_TO_SHADERS "/fragShader.glsl");


    char path2[] = PATH_TO_OBJECTS "/base.obj";
    base = new Object(path2);
    base->makeObject(*shader_norm);
    //base->model = glm::translate(base->model, glm::vec3(15.0, 0.0, 0.0));
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
    shader_horse->cleanup();
    shader_norm->cleanup();
    glDeleteTextures(1, &horse_texture_id);
}

void Horse::render(GLuint skyboxID){

	glm::vec3 materialColour = glm::vec3(1.0,1.0,1.0);

    shader_horse->use();
    shader_horse->setInteger("cubemapSampler", 0);

	shader_horse->setVector3f("materialColour", materialColour);

    GLuint cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    shader_horse->setFloat("refractionIndice", 1.36);
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


void Horse::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos)
{

    bindAllTexture();

    shader_horse->use();
    // 1. send the relevant uniform to the shader
    shader_horse->setMatrix4("M", horse->model);
    shader_horse->setMatrix4("itM", glm::transpose(glm::inverse(horse->model)));
    shader_horse->setMatrix4("V", view);
    shader_horse->setMatrix4("P", projection);

    bool color = (rand() % 50)==2 ? true: false;
    
    if(color){
        float red = (rand() % 256)/256.0 ;
        float green = (rand() % 256)/256.0 ;
        float blue = (rand() % 256)/256.0 ;
        shader_horse->setVector3f("materialColour", glm::vec3(red,green,blue));
    }

    //shader_horse->setInteger("f_texture", 0);

    shader_horse->setVector3f("u_view_pos", camera_position);

    //std::cout << delta.z <<std::endl;
    shader_horse->setVector3f("light.light_pos", light_pos);
    horse->draw();

    shader_norm->use();
    shader_norm->setMatrix4("M", base->model);
    shader_norm->setMatrix4("itM", glm::transpose(glm::inverse(base->model)));
    shader_norm->setMatrix4("V", view);
    shader_norm->setMatrix4("P", projection);
    shader_norm->setVector3f("u_view_pos", camera_position);
    shader_norm->setVector3f("u_light_pos", light_pos);
    shader_norm->setInteger("f_texture", 0);

    
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
