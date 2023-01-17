#include "dog.h"

Dog::Dog()
{
    shader = new Shader(PATH_TO_SHADERS "/vertexShader.glsl", PATH_TO_SHADERS "/fragShader.glsl", NULL);
    shaderNormal=  new Shader(PATH_TO_SHADERS "/dogNormalV.glsl", PATH_TO_SHADERS "/dogNormalF.glsl", PATH_TO_SHADERS "/dogNormalG.glsl");
    previousTime = 0;

    initKeyframe();
    initTexture(PATH_TO_TEXTURE "/dog/dog.png");

    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}
void Dog::initKeyframe()
{
    objectPaths = {PATH_TO_OBJECTS "/dog_inter_left.obj", PATH_TO_OBJECTS "/dog_inter_right.obj", PATH_TO_OBJECTS "/dog_right.obj", PATH_TO_OBJECTS "/dog_left.obj"};

    currentFrame = 0;
    numOfFrames = objectPaths.size();

    for (int i = 0; i < numOfFrames; i++)
    {

        obj.push_back(new Object(objectPaths[i]));
    }

    int numOfObjects = obj.size();
    for (int i = 0; i < numOfObjects; i++)
    {

        obj[i]->makeObject(*shader);
    }
    dog = obj[currentFrame];
}

Shader *Dog::getShader()
{
    return this->shader;
}

void Dog::cleanup()
{
    shader->cleanup();
    glDeleteTextures(1, &dogTextureID);
}

void Dog::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &cameraPosition, const glm::vec3 &lightPosition, double currentTime)
{

    walk(currentTime);
    shader->use();

    bindAllTexture();

    // 1. send the relevant uniform to the shader
    shader->setMatrix4("M", dog->model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(dog->model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setVector3f("u_view_pos", cameraPosition);
    shader->setVector3f("light.light_pos", lightPosition);
    shader->setInteger("f_texture", 0);
    dog->draw();
    
    shaderNormal->use();
    shaderNormal->setMatrix4("projection", projection);
    shaderNormal->setMatrix4("view", view);
    shaderNormal->setMatrix4("model", dog->model);
    dog->draw();

    glBindVertexArray(0);
    glUseProgram(0);
}
void Dog::walk(double currentTime)
{
    bool move = (rand() % 10) == 2 ? true : false;
    currentFrame = (currentFrame + 1) % numOfFrames;
    double deltaTime = sin(currentTime / 2);

    if (move)
    {
        dog = obj[currentFrame];
    }

    dog->model = glm::mat4(1.0f);
    dog->model = glm::scale(dog->model, glm::vec3(0.5f));
    dog->model = glm::translate(dog->model, glm::vec3(-10.0, 5.0, -50.0 + deltaTime * 5));
    dog->model = glm::rotate(dog->model, glm::radians(-75.f), glm::vec3(0.0, 1.0, 0.0));

    if ((deltaTime - previousTime) > 0.0)
    {
        dog->model = glm::rotate(dog->model, glm::radians(-175.f), glm::vec3(0.0, 1.0, 0.0));
    }
    previousTime = deltaTime;
}

void Dog::initTexture(std::string path)
{

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &dogTextureID);
    glBindTexture(GL_TEXTURE_2D, dogTextureID);

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

void Dog::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dogTextureID);
}
