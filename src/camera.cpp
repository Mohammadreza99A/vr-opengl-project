#include "camera.h"

Camera::Camera()
{
    this->speed = 2.0f;
    this->angle_change_per_second = 1.8f;
}

void Camera::init(float height, Terrain *terrain)
{
    setHeight(height);
    setTerrain(terrain);

    this->updatePos();
}

void Camera::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
    const float EPSILON = 0.000001f;

    this->eye = eye;
    this->center = center;
    this->up = up;

    glm::vec3 direction = center - eye;
    direction = glm::normalize(direction);
    if (direction[0] < EPSILON && direction[0] > -EPSILON)
    {
        direction[0] = EPSILON;
    }
    angle_up = atan(sqrt(direction[0] * direction[0] + direction[2] * direction[2]) / direction[1]);
    if (angle_up < 0.0f)
    {
        angle_up = 3.1415f + angle_up;
    }
    angle_side = atan(direction[2] / direction[0]);

    if (direction[0] < 0.0f)
    {
        angle_side += 3.1415f;
    }

    // normalize the center vector
    normalize(direction);
    center = eye + direction;

    this->updatePos();
}

void Camera::getPosition(glm::vec3 position)
{
    position.x = eye.x;
    position.y = eye.y;
    position.z = eye.z;
}

void Camera::getDirection(glm::vec3 direction)
{
    direction.x = center.x - eye.x;
    direction.y = center.y - eye.y;
    direction.z = center.z - eye.z;
}

void Camera::getCenter(float center[3])
{
    center[0] = this->center.x;
    center[1] = this->center.y;
    center[2] = this->center.z;
}

glm::mat4x4 Camera::getMatrix()
{
    return glm::lookAt(eye, center, up);
}

void Camera::setSpeed(float speed)
{
    this->speed = speed;
}

void Camera::setViewSpeed(float speed)
{
    this->angle_change_per_second = speed;
}

void Camera::setHeight(float height)
{
    this->height = height;
}

void Camera::setTerrain(Terrain *terrain)
{
    this->terrain = terrain;
}

glm::mat4x4 Camera::getReflectionMatrix(float height)
{
    // just return another look at on the other side of the plane (only plane along y)
    return glm::lookAt(glm::vec3(eye.x, -eye.y + height * 2, eye.z), glm::vec3(center.x, -center.y + height * 2, center.z), up);
}

glm::mat4x4 Camera::getPerspectiveMat()
{
    return glm::perspective(3.1415f / 2.0f, (float)win_width / (float)win_height, 0.1f, 1000.0f);
}

void Camera::inputHandling(char key, float time_delta)
{

    glm::vec3 direction;
    glm::vec3 speed_factor = glm::vec3(this->speed) * time_delta;

    direction = center - eye;
    direction = glm::vec3(direction[0], 0.0f, direction[2]);
    direction = normalize(direction);

    glm::vec3 left_dir = normalize(cross(up, direction));
    glm::vec3 right_dir = normalize(cross(direction, up));

    float height_before = eye[1];

    switch (key)
    {
    case 'W':
        eye = eye + speed_factor * direction;
        center = center + speed_factor * direction;
        break;
    case 'S':
        eye = eye - speed_factor * direction;
        center = center - speed_factor * direction;
        break;
    case 'A':
        eye = eye + speed_factor * left_dir;
        center = center + speed_factor * left_dir;
        break;
    case 'D':
        eye = eye + speed_factor * right_dir;
        center = center + speed_factor * right_dir;
    }

    float height_diff = terrain->get_height(eye[0], eye[2]) - height_before;

    eye[1] += height_diff + height;
    center[1] += height_diff + height;

    float angle_change = this->angle_change_per_second * time_delta;

    if (key == 'J')
    {
        angle_side -= angle_change;
        changeCamOrientation();
    }
    if (key == 'L')
    {
        angle_side += angle_change;
        changeCamOrientation();
    }
    if (key == 'I')
    {
        if (angle_up - angle_change > 0.0f)
            angle_up -= angle_change;
        changeCamOrientation();
    }
    if (key == 'K')
    {
        if (angle_up + angle_change < 3.14f)
            angle_up += angle_change;

        changeCamOrientation();
    }
}

void Camera::updatePos()
{
    changeCamOrientation();
}

void Camera::setWindowSize(unsigned int win_width, unsigned int win_height)
{
    this->win_width = win_width;
    this->win_height = win_height;
}

void Camera::changeCamOrientation()
{

    float x = sin(angle_up) * cos(angle_side);
    float y = cos(angle_up);
    float z = sin(angle_up) * sin(angle_side);

    glm::vec3 direction(x, y, z);

    center = eye + direction;
}