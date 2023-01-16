#include "snow.h"

Snow::Snow(uint nbOfParticles)
{
    shader = new Shader(PATH_TO_SHADERS "/snowV.glsl", PATH_TO_SHADERS "/snowF.glsl",NULL);

    srand(time(0));

    _pid = shader->ID;

    this->nbOfParticles = nbOfParticles;
    lifeSecMin = 1;
    lifeSecMax = 2;

    vaoParticles = 0;

    glGenVertexArrays(1, &vaoParticles);
    glBindVertexArray(vaoParticles);

    // particle is just a square
    GLfloat vpoint[] = {
        -0.5f, 0.0f, 0.5f,  // 1
        0.5f, 0.0f, 0.5f,   // 2
        -0.5f, 0.0f, -0.5f, // 0
        0.5f, 0.0f, -0.5f,  // 3
    };

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

    GLuint vpointID = glGetAttribLocation(_pid, "position");
    glEnableVertexAttribArray(vpointID);
    glVertexAttribPointer(vpointID, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(vpointID, 0);

    // texture positions will allow the fragemnt shader to draw the particle
    const GLfloat vtexcoord[] = {
        0.0f, 1.0f, // 1
        1.0f, 1.0f, // 2
        0.0f, 0.0f, // 0
        1.0f, 0.0f, // 3
    };

    glGenBuffers(1, &_vboTex);
    glBindBuffer(GL_ARRAY_BUFFER, _vboTex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

    GLuint vtexcoordID = glGetAttribLocation(_pid, "uv");
    glEnableVertexAttribArray(vtexcoordID);
    glVertexAttribPointer(vtexcoordID, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(vtexcoordID, 0);

    glGenBuffers(1, &_vboPointsPos);
    glBindBuffer(GL_ARRAY_BUFFER, _vboPointsPos);

    particlesPositions = new GLfloat[3 * nbOfParticles];

    for (size_t i = 0; i < nbOfParticles; i++)
    {
        Particle part;
        part.position = &(particlesPositions[i * 3]);
        part.lifeRemaining = 0;
        part.age = 0;
        part.speed = glm::vec3(0.0f);
        particlesVector.push_back(part);
    }

    glBufferData(GL_ARRAY_BUFFER, nbOfParticles * 3 * sizeof(GLfloat), particlesPositions, GL_DYNAMIC_DRAW);

    // contains 3d position of the particle
    GLuint posID = glGetAttribLocation(_pid, "pos_point");

    glEnableVertexAttribArray(posID);
    glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glVertexAttribDivisor(posID, 1);

    // particle life buffer
    glGenBuffers(1, &_vboPointsLife);
    glBindBuffer(GL_ARRAY_BUFFER, _vboPointsLife);

    particlesLife = new GLfloat[nbOfParticles * 2];

    for (size_t i = 0; i < nbOfParticles; i++)
    {
        particlesLife[i * 2] = 0.0f;     // life remaining of the particle
        particlesLife[i * 2 + 1] = 0.0f; // age of the particle
        particlesVector[i].lifeRemaining = &(particlesLife[i * 2]);
        particlesVector[i].age = &(particlesLife[i * 2 + 1]);
    }

    glBufferData(GL_ARRAY_BUFFER, nbOfParticles * 2 * sizeof(GLfloat), particlesLife, GL_DYNAMIC_DRAW);

    GLuint ageID = glGetAttribLocation(_pid, "life_age");

    glEnableVertexAttribArray(ageID);
    glVertexAttribPointer(ageID, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glVertexAttribDivisor(ageID, 1);

    glBindVertexArray(0);
}

void Snow::clean()
{
    delete[] particlesPositions;
}

void Snow::draw(glm::mat4x4 viewMatrix, glm::mat4x4 projectionMatrix, glm::vec3 &cameraPosition, glm::vec3 &lightPosition)
{

    handleParticles();

    // sends the buffer to the gpu again
    glBindBuffer(GL_ARRAY_BUFFER, _vboPointsPos);
    glBufferData(GL_ARRAY_BUFFER, nbOfParticles * 3 * sizeof(GLfloat), particlesPositions, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vboPointsLife);
    glBufferData(GL_ARRAY_BUFFER, nbOfParticles * 2 * sizeof(GLfloat), particlesLife, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader->use();
    glBindVertexArray(vaoParticles);

    shader->setVector3f("camera_position", cameraPosition);
    shader->setVector3f("light_position", lightPosition);

    shader->setMatrix4("view", viewMatrix);
    shader->setMatrix4("projection", projectionMatrix);

    glDisable(GL_CULL_FACE);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nbOfParticles);

    glEnable(GL_CULL_FACE);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Snow::setInitialVelocity(glm::vec3 initialVelocity , glm::vec3 variation){
    this->velocity.x = initialVelocity.x;
    this->velocity.y = initialVelocity.y;
    this->velocity.z = initialVelocity.z;

    for (size_t i = 0; i < nbOfParticles; i++)
    {
        float randVal = (rand() % 1000) / 1000.0f;
        particlesVector[i].speed.x = mix(initialVelocity.x - variation.x, initialVelocity.x + variation.x, randVal);
        randVal = (rand() % 1000) / 1000.0f;
        particlesVector[i].speed.y = mix(initialVelocity.y - variation.y, initialVelocity.y + variation.y, randVal);
        randVal = (rand() % 1000) / 1000.0f;
        particlesVector[i].speed.z = mix(initialVelocity.z - variation.z, initialVelocity.z + variation.z, randVal);
    }
}

void Snow::setEmiterBoundary(glm::vec3 minCoords, glm::vec3 maxCoords)
{
    emiterBoudaryMin.x = minCoords.x;
    emiterBoudaryMax.x = maxCoords.x;
    emiterBoudaryMin.y = minCoords.y;
    emiterBoudaryMax.y = maxCoords.y;
    emiterBoudaryMin.z = minCoords.z;
    emiterBoudaryMax.z = maxCoords.z;
}

void Snow::setTime(float time)
{
    this->currentTime = time;
}

void Snow::setLifeDurationSec(float lifeSecMin, float lifeSecMax)
{
    this->lifeSecMin = lifeSecMin;
    this->lifeSecMax = lifeSecMax;
}

void Snow::handleParticles()
{
    float timeDiff = currentTime - prevTime;
    for (size_t i = 0; i < nbOfParticles; i++)
    {

        if (*particlesVector[i].lifeRemaining <= 0)
        { // create new particle
            float randVal = (rand() % 1000) / 1000.0f;
            particlesVector[i].position[0] = mix(emiterBoudaryMin.x, emiterBoudaryMax.x, randVal);
            randVal = (rand() % 1000) / 1000.0f;
            particlesVector[i].position[1] = mix(emiterBoudaryMin.y, emiterBoudaryMax.y, randVal);
            randVal = (rand() % 1000) / 1000.0f;
            particlesVector[i].position[2] = mix(emiterBoudaryMin.z, emiterBoudaryMax.z, randVal);
            randVal = (rand() % 1000) / 1000.0f;
            *particlesVector[i].lifeRemaining = mix(lifeSecMin, lifeSecMax, randVal);
            *particlesVector[i].age = 0.0f;
        }
        else
        { // update particle
            float wind[3];
            windFunc(particlesVector[i].position, wind, this->currentTime);
            particlesVector[i].position[0] += timeDiff * (particlesVector[i].speed.x + wind[0]);
            particlesVector[i].position[1] += timeDiff * (particlesVector[i].speed.y + wind[1]);
            particlesVector[i].position[2] += timeDiff * (particlesVector[i].speed.z + wind[2]);
            *particlesVector[i].lifeRemaining -= timeDiff;
            *particlesVector[i].age += timeDiff;
        }
    }
    prevTime = currentTime;
}
float Snow::mix(float min, float max, float ratio)
{
    return min * (1.0f - ratio) + max * ratio;
}

void Snow::defaultWind(float[3], float ret[3], float)
{
    ret[0] = 0;
    ret[1] = 0;
    ret[2] = 0;
}

void Snow::windFunc(float pos[3], float ret[3], float time)
{
    ret[0] = (sin((pos[1] + pos[2] + time) / 2)) * 2;
    ret[1] = (cos((pos[0] + pos[2] + time) / 2)) * 2;
    ret[2] = (sin((pos[1] + pos[2] + time) / 2)) * 2;
}