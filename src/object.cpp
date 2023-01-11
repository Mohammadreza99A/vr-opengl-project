#include "object.h"

Object::Object(const char *path)
{

    std::ifstream infile(path);
    // TODO Error management
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string indice;
        iss >> indice;
        // std::cout << "indice : " << indice << std::endl;
        if (indice == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back(glm::vec3(x, y, z));
        }
        else if (indice == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back(glm::vec3(x, y, z));
        }
        else if (indice == "vt")
        {
            float u, v;
            iss >> u >> v;
            textures.push_back(glm::vec2(u, v));
        }
        else if (indice == "f")
        {
            std::string f1, f2, f3;
            iss >> f1 >> f2 >> f3;

            std::string p, t, n;

            // for face 1
            Vertex v1;

            p = f1.substr(0, f1.find("/"));
            f1.erase(0, f1.find("/") + 1);

            t = f1.substr(0, f1.find("/"));
            f1.erase(0, f1.find("/") + 1);

            n = f1.substr(0, f1.find("/"));

            v1.Position = positions.at(std::stof(p) - 1);
            v1.Normal = normals.at(std::stof(n) - 1);
            v1.Texture = textures.at(std::stof(t) - 1);
            vertices.push_back(v1);
            indices.push_back(std::stof(p) - 1);

            // for face 12
            Vertex v2;

            p = f2.substr(0, f2.find("/"));
            f2.erase(0, f2.find("/") + 1);

            t = f2.substr(0, f2.find("/"));
            f2.erase(0, f2.find("/") + 1);

            n = f2.substr(0, f2.find("/"));

            v2.Position = positions.at(std::stof(p) - 1);
            v2.Normal = normals.at(std::stof(n) - 1);
            v2.Texture = textures.at(std::stof(t) - 1);
            vertices.push_back(v2);
            indices.push_back(std::stof(p) - 1);

            // for face 3
            Vertex v3;

            p = f3.substr(0, f3.find("/"));
            f3.erase(0, f3.find("/") + 1);

            t = f3.substr(0, f3.find("/"));
            f3.erase(0, f3.find("/") + 1);

            n = f3.substr(0, f3.find("/"));

            v3.Position = positions.at(std::stof(p) - 1);
            v3.Normal = normals.at(std::stof(n) - 1);
            v3.Texture = textures.at(std::stof(t) - 1);
            vertices.push_back(v3);
            indices.push_back(std::stof(p) - 1);
            calculateTangents(v1, v2, v3);
        }
    }
    // std::cout << positions.size() << std::endl;
    // std::cout << normals.size() << std::endl;
    // std::cout << textures.size() << std::endl;
    std::cout << "Load model with " << vertices.size() << " vertices" << std::endl;

    infile.close();

    numVertices = vertices.size();
}

void Object::makeObject(Shader &shader, bool texture, bool bump, bool has_shadow_buffer)
{
    /* This is a working but not perfect solution, you can improve it if you need/want
     * What happens if you call this function twice on an Model ?
     * What happens when a shader doesn't have a position, tex_coord or normal attribute ?
     */

    float *data = new float[11 * numVertices];
    for (int i = 0; i < numVertices; i++)
    {
        Vertex v = vertices.at(i);
        data[i * 11] = v.Position.x;
        data[i * 11 + 1] = v.Position.y;
        data[i * 11 + 2] = v.Position.z;

        data[i * 11 + 3] = v.Texture.x;
        data[i * 11 + 4] = v.Texture.y;

        data[i * 11 + 5] = v.Normal.x;
        data[i * 11 + 6] = v.Normal.y;
        data[i * 11 + 7] = v.Normal.z;

        data[i * 11 + 8] = v.Tangent.x;
        data[i * 11 + 9] = v.Tangent.y;
        data[i * 11 + 10] = v.Tangent.z;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // define VBO and VAO as active buffer and active vertex array
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, data, GL_STATIC_DRAW);

    auto att_pos = glGetAttribLocation(shader.ID, "position");
    glEnableVertexAttribArray(att_pos);
    glVertexAttribPointer(att_pos, 3, GL_FLOAT, false, 11 * sizeof(float), (void *)0);

    if (texture)
    {
        auto att_tex = glGetAttribLocation(shader.ID, "tex_coord");
        glEnableVertexAttribArray(att_tex);
        glVertexAttribPointer(att_tex, 2, GL_FLOAT, false, 11 * sizeof(float), (void *)(3 * sizeof(float)));
    }

    auto att_col = glGetAttribLocation(shader.ID, "normal");
    glEnableVertexAttribArray(att_col);
    glVertexAttribPointer(att_col, 3, GL_FLOAT, false, 11 * sizeof(float), (void *)(5 * sizeof(float)));

    if (bump)
    {

        auto att_tan = glGetAttribLocation(shader.ID, "tangent");
        glEnableVertexAttribArray(att_tan);
        glVertexAttribPointer(att_tan, 3, GL_FLOAT, false, 11 * sizeof(float), (void *)(8 * sizeof(float)));
    }

    glUniform1ui(glGetUniformLocation(shader.ID, "shadow_buffer_tex_size"), this->shadow_buffer_texture_width);

    if (has_shadow_buffer)
    {
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "shadow_matrix"), 1, GL_FALSE, glm::value_ptr(this->shadow_matrix));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _shadow_texture_id);
        GLuint tex_id = glGetUniformLocation(shader.ID, "shadow_buffer_tex");
        glUniform1i(tex_id, 1 /*GL_TEXTURE0*/);
    }

    if (has_shadow_buffer)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // desactive the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete[] data;
}

void Object::calculateTangents(Vertex &v1, Vertex &v2, Vertex &v3)
{
    glm::vec3 delatPos1 = v2.Position - v1.Position;
    glm::vec3 delatPos2 = v3.Position - v1.Position;
    glm::vec2 uv1 = v1.Texture;
    glm::vec2 uv2 = v2.Texture;
    glm::vec2 uv3 = v3.Texture;
    glm::vec2 deltaUv2 = uv2 - uv1;
    glm::vec2 deltaUv3 = uv3 - uv1;

    float r = 1.0f / (deltaUv2.x * deltaUv3.y - deltaUv2.y * deltaUv3.x);
    delatPos1 *= deltaUv3.y;
    delatPos2 *= deltaUv2.y;
    glm::vec3 tangent = delatPos1 - delatPos2;
    tangent *= r;
    v1.Tangent = tangent;
    v2.Tangent = tangent;
    v3.Tangent = tangent;
}

void Object::draw()
{

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}
