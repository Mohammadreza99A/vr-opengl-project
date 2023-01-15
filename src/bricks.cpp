#include "bricks.h"

Bricks::Bricks(unsigned int sub_x, unsigned int sub_y) : MatrixSquares(sub_x, sub_y)
{
    shader = new Shader(PATH_TO_SHADERS "/normalV.glsl", PATH_TO_SHADERS "/dispF.glsl", NULL);
    _pid = shader->ID;
    if (_pid == 0)
        exit(-1);

    // load textures
    // -------------

    diffuseMap=initTexture(PATH_TO_TEXTURE "/brick/bricks2.jpg");
    normalMap=initTexture(PATH_TO_TEXTURE "/brick/bricks2_normal.jpg");
    heightMap=initTexture(PATH_TO_TEXTURE "/brick/bricks2_disp.jpg");

    init();

    float ambient = 0.5;
	float diffuse = 0.1;
	float specular = 0.2;

    shader->use();
    shader->setInteger("diffuseMap", 0);
    shader->setInteger("normalMap", 1);
    shader->setInteger("depthMap", 2);

    // shader->setFloat("shininess", 68.0f);

    // shader->setFloat("light.ambient_strength", ambient);
	// shader->setFloat("light.diffuse_strength", diffuse);
	// shader->setFloat("light.specular_strength", specular);
	// shader->setFloat("light.constant", 1.0);
	// shader->setFloat("light.linear", 0.3);
	// shader->setFloat("light.quadratic", 0.07);
    // to avoid the current object being polluted
    glBindVertexArray(0);
    glUseProgram(0);
}

Shader *Bricks::getShader()
{
    return this->shader;
}

void Bricks::transform(glm::vec3 translate,float radians, glm::vec3 rotate, glm::vec3 scale){
    model = glm::translate(model, translate);
    model = glm::rotate(model, radians, rotate);
    model = glm::scale(model, scale);
}

Bricks::~Bricks()
{
    MatrixSquares::~MatrixSquares();

    if (tangent != NULL)
    {
        delete tangent;
        tangent = NULL;
    }

}

void Bricks::init()
{
    MatrixSquares::init();
    set_indices();

    generate_bricks();

    compute_tangents();

    fill_buffers();

    glBindVertexArray(0);
}

void Bricks::compute_tangents()
{
    tangent = new GLfloat[nb_vertices * 3];

    for (unsigned int i = 0; i < nb_indices; i += 3)
    {

        unsigned int cur_pos = indices[i];
        glm::vec3 pos1(vertices[cur_pos], vertices[cur_pos + 1], vertices[cur_pos + 2]);

        cur_pos = indices[i + 1] * 3;
        glm::vec3 pos2(vertices[cur_pos], vertices[cur_pos + 1], vertices[cur_pos + 2]);

        cur_pos = indices[i + 2] * 3;
        glm::vec3 pos3(vertices[cur_pos], vertices[cur_pos + 1], vertices[cur_pos + 2]);

        cur_pos = indices[i] * 2;
        glm::vec2 uv1(textures[cur_pos], textures[cur_pos + 1]);

        cur_pos = indices[i + 1];
        glm::vec2 uv2(textures[cur_pos], textures[cur_pos + 1]);

        cur_pos = indices[i + 2] ;
        glm::vec2 uv3(textures[cur_pos], textures[cur_pos + 1]);

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent_v(f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                            f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                            f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z));

        tangent_v = glm::normalize(tangent_v);
        for (unsigned int k = 0; k < 3; k++)
        {
            cur_pos = indices[i + k] * 3;
            tangent[cur_pos] = tangent_v.x;
            tangent[cur_pos + 1] = tangent_v.y;
            tangent[cur_pos + 2] = tangent_v.z;
        }
    }
}
void Bricks::fill_buffers()
{
    MatrixSquares::fill_buffers();

    glGenBuffers(1, &_vbo_tg);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tg);
    glBufferData(GL_ARRAY_BUFFER, nb_vertices * 3 * sizeof(GLfloat), tangent, GL_STATIC_DRAW);

    GLuint tangent_id = glGetAttribLocation(_pid, "tangent");
    glEnableVertexAttribArray(tangent_id);
    glVertexAttribPointer(tangent_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Bricks::draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &camera_position, float heightScale, const glm::vec3 &light_pos)
{
    glUseProgram(_pid);
    glBindVertexArray(_vao);
    shader->use();
    shader->setMatrix4("projection", projection);
    shader->setMatrix4("view", view);
    shader->setMatrix4("model", model);
    shader->setVector3f("viewPos", camera_position);
    shader->setVector3f("light.light_pos", light_pos);
    shader->setFloat("heightScale", heightScale); // adjust with Q and E keys
    bindAllTexture();
    glDrawElements(GL_TRIANGLES, nb_indices, GL_UNSIGNED_INT, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

GLuint Bricks::initTexture(char const *path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


void Bricks::bindAllTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, heightMap);
}


void Bricks::cleanup()
{
    shader->cleanup();
    glDeleteTextures(1, &diffuseMap);
    glDeleteTextures(1, &normalMap);
    glDeleteTextures(1, &heightMap);
}

void Bricks::generate_bricks()
{
    for (uint j = 0; j < sub_y; j++)
    {
        for (uint i = 0; i < sub_x; i++)
        {

            // makes that the relatives go from -1 to 1
            GLfloat relative_x = (float(i) / (sub_x - 1))*2  - 1;
            // rel_z not y as y is up
            GLfloat relative_z = (float(j) / (sub_y - 1)) *10 - 1;

            unsigned int cur_pos = (j * sub_x + i) * 3;

            vertices[cur_pos] = relative_x;
            vertices[cur_pos + 1] = 0; //(*this.*func)(relative_x, relative_z);
            vertices[cur_pos + 2] = relative_z;

            textures[((j * sub_x + i) * 2)] = (float(i) / (sub_x - 1));
            textures[((j * sub_x + i) * 2) + 1] = (float(j) / (sub_y - 1))*5;

            normals[cur_pos + 0] = 0;
            normals[cur_pos + 1] = 1;
            normals[cur_pos + 2] = 0;
        }
    }
}
