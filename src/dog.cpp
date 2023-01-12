#include "dog.h"

Dog::Dog()
{
    shader = new Shader(PATH_TO_SHADERS "/vertexShader.glsl", PATH_TO_SHADERS "/fragShader.glsl");

    obj_paths = { PATH_TO_OBJECTS "/dog.obj", PATH_TO_OBJECTS "/dog1.obj"};

    current_frame = 0;
    num_frames = obj_paths.size();
    dog = new Object(obj_paths[current_frame]);
    dog->makeObject(*shader);
    dog->model = glm::translate(dog->model, glm::vec3(15.0, 5.0, -30.0));
    // dog->model = glm::rotate(dog->model, glm::radians(-75.f), glm::vec3(1.0, 0.0, 0.0));


    initTexture(PATH_TO_TEXTURE "/dog/dog.png");

    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

Shader *Dog::getShader()
{
    return this->shader;
}

void Dog::cleanup()
{
    shader->cleanup();
    glDeleteTextures(1, &dog_texture_id);
}

void Dog::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, const glm::vec3 &light_pos,double current_frame)
{

    walk(current_frame);
    shader->use();

    bindAllTexture();

    // 1. send the relevant uniform to the shader
    shader->setMatrix4("M", dog->model);
    shader->setMatrix4("itM", glm::transpose(glm::inverse(dog->model)));
    shader->setMatrix4("V", view);
    shader->setMatrix4("P", projection);
    shader->setVector3f("u_view_pos", camera_position);
    shader->setVector3f("light.light_pos", light_pos);
    shader->setInteger("f_texture", 0);

    dog->draw();

    glBindVertexArray(0);
    glUseProgram(0);
}
void Dog::walk(double current_frame){
    bool move = (rand() % 50) == 2 ? true : false;
    this->current_frame = (this->current_frame + 1) % num_frames;

    
    if (move){

    dog= new Object(obj_paths[this->current_frame]);
    dog->model = glm::translate(dog->model, glm::vec3(15.0, 0.0, -30.0) + glm::vec3(sin(current_frame)*10, 5.0 , sin(current_frame)*10));
    }
    dog->makeObject(*shader);
}

void Dog::initTexture(std::string path)
{

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &dog_texture_id);
    glBindTexture(GL_TEXTURE_2D, dog_texture_id);

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
    glBindTexture(GL_TEXTURE_2D, dog_texture_id);
}
