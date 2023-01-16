#include "sky_box.h"

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};

SkyBox::SkyBox()
{
    shader = new Shader(PATH_TO_SHADERS "/skyBoxV.glsl", PATH_TO_SHADERS "/skyBoxF.glsl",NULL);

    paths.push_back(PATH_TO_TEXTURE "/skybox/posx.jpg");
    paths.push_back(PATH_TO_TEXTURE "/skybox/negx.jpg");
    paths.push_back(PATH_TO_TEXTURE "/skybox/posy.jpg");
    paths.push_back(PATH_TO_TEXTURE "/skybox/negy.jpg");
    paths.push_back(PATH_TO_TEXTURE "/skybox/posz.jpg");
    paths.push_back(PATH_TO_TEXTURE "/skybox/negz.jpg");

    // skybox VAO
    glGenVertexArrays(1, &this->skyboxVAO);
    glGenBuffers(1, &this->skyboxVBO);
    glBindVertexArray(this->skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    load();

    shader->use();
    shader->setInteger("skybox", 0);
}

void SkyBox::load()
{
    glGenTextures(1, &this->textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);

    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < this->paths.size(); i++)
    {
        unsigned char *data = stbi_load(this->paths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << this->paths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    stbi_set_flip_vertically_on_load(true);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SkyBox::draw(const glm::mat4 &view, const glm::mat4 &projection,
                  const glm::vec3 &camera_position, const glm::vec3 &light_pos)
{
    shader->use();

    const glm::mat4 view_wo_t = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
    shader->setMatrix4("V", view_wo_t);
    shader->setMatrix4("P", projection);

    glBindVertexArray(this->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

SkyBox::~SkyBox()
{
    glDeleteBuffers(1, &this->skyboxVAO);
    glDeleteBuffers(1, &this->skyboxVBO);
}