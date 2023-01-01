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
        }
    }
    // std::cout << positions.size() << std::endl;
    // std::cout << normals.size() << std::endl;
    // std::cout << textures.size() << std::endl;
    std::cout << "Load model with " << vertices.size() << " vertices" << std::endl;

    infile.close();

    numVertices = vertices.size();
}


void Object::makeObject(Shader &shader, bool texture, bool bump)
{
    /* This is a working but not perfect solution, you can improve it if you need/want
     * What happens if you call this function twice on an Model ?
     * What happens when a shader doesn't have a position, tex_coord or normal attribute ?
     */

 float *data = new float[8 * numVertices];
 for (int i = 0; i < numVertices; i++)
 {
     Vertex v = vertices.at(i);
     data[i * 8] = v.Position.x;
     data[i * 8 + 1] = v.Position.y;
     data[i * 8 + 2] = v.Position.z;

     data[i * 8 + 3] = v.Texture.x;
     data[i * 8 + 4] = v.Texture.y;

     data[i * 8 + 5] = v.Normal.x;
     data[i * 8 + 6] = v.Normal.y;
     data[i * 8 + 7] = v.Normal.z;
 }

 glGenVertexArrays(1, &VAO);
 glGenBuffers(1, &VBO);

 // define VBO and VAO as active buffer and active vertex array
 glBindVertexArray(VAO);
 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, data, GL_STATIC_DRAW);

 auto att_pos = glGetAttribLocation(shader.ID, "position");
 glEnableVertexAttribArray(att_pos);
 glVertexAttribPointer(att_pos, 3, GL_FLOAT, false, 8 * sizeof(float), (void *)0);

 if (texture)
 {
     auto att_tex = glGetAttribLocation(shader.ID, "tex_coord");
     glEnableVertexAttribArray(att_tex);
     glVertexAttribPointer(att_tex, 2, GL_FLOAT, false, 8 * sizeof(float), (void *)(3 * sizeof(float)));
 }

 auto att_col = glGetAttribLocation(shader.ID, "normal");
 glEnableVertexAttribArray(att_col);
 glVertexAttribPointer(att_col, 3, GL_FLOAT, false, 8 * sizeof(float), (void *)(5 * sizeof(float)));

 if (bump)
 {
     GLfloat *tangent = new GLfloat[numVertices * 3];
     GLfloat *bitangent = new GLfloat[numVertices * 3];

     compute_tangent(tangent, bitangent);

     glGenBuffers(1, &VBO_TG);
     glBindBuffer(GL_ARRAY_BUFFER, VBO_TG);
     glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), tangent, GL_STATIC_DRAW);

     auto att_tan = glGetAttribLocation(shader.ID, "tangent");
     glEnableVertexAttribArray(att_tan);
     glVertexAttribPointer(att_tan, 3, GL_FLOAT, GL_FALSE, 0, NULL);

     glGenBuffers(1, &VBO_BTG);
     glBindBuffer(GL_ARRAY_BUFFER, VBO_BTG);
     glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), bitangent, GL_STATIC_DRAW);

     auto att_bitan = glGetAttribLocation(shader.ID, "bitangent");
     glEnableVertexAttribArray(att_bitan);
     glVertexAttribPointer(att_bitan, 3, GL_FLOAT, GL_FALSE, 0, NULL);
     delete tangent;
     delete bitangent;
 }

 // desactive the buffer
 glBindBuffer(GL_ARRAY_BUFFER, 0);
 glBindVertexArray(0);

 delete[] data;
}

void Object::compute_tangent(GLfloat *tangent, GLfloat *bitangent)
{

    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        unsigned int cur_pos = indices.at(i) * 3;
        Vertex v1 = vertices.at(cur_pos);
        glm::vec3 pos1=v1.Position;
        glm::vec2 uv1 = v1.Texture;

        cur_pos = indices.at(i + 1) * 3;
        Vertex v2 = vertices.at(cur_pos);
        glm::vec3 pos2= v2.Position;
        glm::vec2 uv2 = v2.Texture;

        cur_pos = indices.at(i + 2) * 3;
        Vertex v3 = vertices.at(cur_pos);
        glm::vec3 pos3=v3.Position;
        glm::vec2 uv3 = v3.Texture;

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent_v(f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                            f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                            f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z));

        glm::vec3 bitangent_v(f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                              f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                              f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z));

        for (unsigned int k = 0; k < 3; k++)
        {
            cur_pos = indices.at(i + k) * 3;
            tangent[cur_pos] = tangent_v.x;
            tangent[cur_pos + 1] = tangent_v.y;
            tangent[cur_pos + 2] = tangent_v.z;
            bitangent[cur_pos] = bitangent_v.x;
            bitangent[cur_pos + 1] = bitangent_v.y;
            bitangent[cur_pos + 2] = bitangent_v.y;
        }
    }
}

void Object::draw()
{

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}
