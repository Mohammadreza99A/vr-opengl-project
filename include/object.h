#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
/*Principe :
 * On donne le path du fichier -> on lit le fichier
 * 2 étape
 * 1)load le model -> lit le fichier ligne par ligne
 * liste de position de normal de texture
 * suivant la premi�re lettre : lit les valeur suivant et les met dans un vec puis push dans la bonne liste
 * en gros sotck les data dans une frome de tableau
 */

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 Texture;
    glm::vec3 Normal;
};

class Object
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textures;
    std::vector<glm::vec3> normals;
    std::vector<Vertex> vertices;

    int numVertices;

    GLuint VBO, VAO;

    glm::mat4 model = glm::mat4(1.0);

    Object(const char *path);

    void makeObject(Shader shader, bool texture = true);

    void draw();
};
#endif