#include "snow.h"

SnowManager::SnowManager(uint nb_particles)
{
    shader = new Shader(PATH_TO_SHADERS "/snowV.glsl", PATH_TO_SHADERS "/snowF.glsl");

    _pid = shader->ID;

    this->nb_particles = nb_particles;
    life_sec_min = 1;
    life_sec_max = 2;

    vao_particles = 0;

    // wind_func = SnowManager::default_wind;

    glGenVertexArrays(1, &vao_particles);
    glBindVertexArray(vao_particles);

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

    GLuint vpoint_id = glGetAttribLocation(_pid, "position");
    glEnableVertexAttribArray(vpoint_id);
    glVertexAttribPointer(vpoint_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(vpoint_id, 0);

    // texture positions will allow the fragemnt shader to draw the particle
    const GLfloat vtexcoord[] = {
        0.0f, 1.0f, // 1
        1.0f, 1.0f, // 2
        0.0f, 0.0f, // 0
        1.0f, 0.0f, // 3
    };

    glGenBuffers(1, &_vbo_tex);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

    GLuint vtexcoord_id = glGetAttribLocation(_pid, "uv");
    glEnableVertexAttribArray(vtexcoord_id);
    glVertexAttribPointer(vtexcoord_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(vtexcoord_id, 0);

    glGenBuffers(1, &_vbo_points_pos);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_points_pos);

    particles_positions = new GLfloat[3 * nb_particles];

    for (size_t i = 0; i < nb_particles; i++)
    {
        Particle part;
        part.position = &(particles_positions[i * 3]);
        part.life_remaining = 0;
        part.age = 0;
        part.speed = glm::vec3(0.0f);
        lst_particles.push_back(part);
    }

    glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(GLfloat), particles_positions, GL_DYNAMIC_DRAW);

    // contains 3d position of the particle
    GLuint pos_id = glGetAttribLocation(_pid, "pos_point");

    glEnableVertexAttribArray(pos_id);
    glVertexAttribPointer(pos_id, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glVertexAttribDivisor(pos_id, 1);

    // particle life buffer
    glGenBuffers(1, &_vbo_points_life);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_points_life);

    particles_life = new GLfloat[nb_particles * 2];

    for (size_t i = 0; i < nb_particles; i++)
    {
        particles_life[i * 2] = 0.0f;     // life remaining of the particle
        particles_life[i * 2 + 1] = 0.0f; // age of the particle
        lst_particles[i].life_remaining = &(particles_life[i * 2]);
        lst_particles[i].age = &(particles_life[i * 2 + 1]);
    }

    glBufferData(GL_ARRAY_BUFFER, nb_particles * 2 * sizeof(GLfloat), particles_life, GL_DYNAMIC_DRAW);

    GLuint age_id = glGetAttribLocation(_pid, "life_age");

    glEnableVertexAttribArray(age_id);
    glVertexAttribPointer(age_id, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glVertexAttribDivisor(age_id, 1);

    glBindVertexArray(0);
}

void SnowManager::clean()
{
    delete[] particles_positions;
}

void SnowManager::draw(glm::mat4x4 view_matrix, glm::mat4x4 projection_matrix, glm::vec3 &camera_position, glm::vec3 &light_position)
{

    handle_particles();

    // sends the buffer to the gpu again
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_points_pos);
    glBufferData(GL_ARRAY_BUFFER, nb_particles * 3 * sizeof(GLfloat), particles_positions, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo_points_life);
    glBufferData(GL_ARRAY_BUFFER, nb_particles * 2 * sizeof(GLfloat), particles_life, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    shader->use();
    glBindVertexArray(vao_particles);

    shader->setVector3f("camera_position", camera_position);
    shader->setVector3f("light_position", light_position);

    shader->setMatrix4("view", view_matrix);
    shader->setMatrix4("projection", projection_matrix);

    glDisable(GL_CULL_FACE);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nb_particles);

    glEnable(GL_CULL_FACE);
    glBindVertexArray(0);
    glUseProgram(0);
}

void SnowManager::set_initial_velocity(float init_vel_x, float init_vel_y, float init_vel_z, float variation_x, float variation_y, float variation_z)
{
    this->velocity.x = init_vel_x;
    this->velocity.y = init_vel_y;
    this->velocity.z = init_vel_z;

    for (size_t i = 0; i < nb_particles; i++)
    {
        float rand_val = (rand() % 1000) / 1000.0f;
        lst_particles[i].speed.x = mix(init_vel_x - variation_x, init_vel_x + variation_x, rand_val);
        rand_val = (rand() % 1000) / 1000.0f;
        lst_particles[i].speed.y = mix(init_vel_y - variation_y, init_vel_y + variation_y, rand_val);
        rand_val = (rand() % 1000) / 1000.0f;
        lst_particles[i].speed.z = mix(init_vel_z - variation_z, init_vel_z + variation_z, rand_val);
    }
}

void SnowManager::set_emiter_boundary(GLfloat minx, GLfloat maxx, GLfloat miny, GLfloat maxy, GLfloat minz, GLfloat maxz)
{
    emiter_boudary_min_x = minx;
    emiter_boudary_max_x = maxx;
    emiter_boudary_min_y = miny;
    emiter_boudary_max_y = maxy;
    emiter_boudary_min_z = minz;
    emiter_boudary_max_z = maxz;
}

void SnowManager::set_time(float time)
{
    this->current_time = time;
}

void SnowManager::set_life_duration_sec(float life_sec_min, float life_sec_max)
{
    this->life_sec_min = life_sec_min;
    this->life_sec_max = life_sec_max;
}

void SnowManager::handle_particles()
{
    float time_diff = current_time - prev_time;
    for (size_t i = 0; i < nb_particles; i++)
    {

        if (*lst_particles[i].life_remaining <= 0)
        { // create new particle
            float rand_val = (rand() % 1000) / 1000.0f;
            lst_particles[i].position[0] = mix(emiter_boudary_min_x, emiter_boudary_max_x, rand_val);
            rand_val = (rand() % 1000) / 1000.0f;
            lst_particles[i].position[1] = mix(emiter_boudary_min_y, emiter_boudary_max_y, rand_val);
            rand_val = (rand() % 1000) / 1000.0f;
            lst_particles[i].position[2] = mix(emiter_boudary_min_z, emiter_boudary_max_z, rand_val);
            rand_val = (rand() % 1000) / 1000.0f;
            *lst_particles[i].life_remaining = mix(life_sec_min, life_sec_max, rand_val);
            *lst_particles[i].age = 0.0f;
        }
        else
        { // update particle
            float wind[3];
            wind_func(lst_particles[i].position, wind, this->current_time);
            lst_particles[i].position[0] += time_diff * (lst_particles[i].speed.x + wind[0]);
            lst_particles[i].position[1] += time_diff * (lst_particles[i].speed.y + wind[1]);
            lst_particles[i].position[2] += time_diff * (lst_particles[i].speed.z + wind[2]);
            *lst_particles[i].life_remaining -= time_diff;
            *lst_particles[i].age += time_diff;
        }
    }
    prev_time = current_time;
}
float SnowManager::mix(float min, float max, float ratio)
{
    return min * (1.0f - ratio) + max * ratio;
}

void SnowManager::default_wind(float[3], float ret[3], float)
{
    ret[0] = 0;
    ret[1] = 0;
    ret[2] = 0;
}

void SnowManager::wind_func(float pos[3], float ret[3], float time)
{
    ret[0] = (sin((pos[1] + pos[2] + time) / 2)) * 2;
    ret[1] = (cos((pos[0] + pos[2] + time) / 2)) * 2;
    ret[2] = (sin((pos[1] + pos[2] + time) / 2)) * 2;
}