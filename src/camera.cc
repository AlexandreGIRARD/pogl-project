#include <camera.hh>

glm::mat4 Camera::look_at()
{
    return glm::lookAt(_position, _position + _forward, _up);
}

float Camera::get_speed()
{
    return _speed;
}

float Camera::get_mouse_speed()
{
    return _mouse_speed;
}

glm::vec3 Camera::get_position()
{
    return _position;
}

glm::vec3 Camera::get_target()
{
    return _target;
}

glm::vec3 Camera::get_up()
{
    return _up;
}

glm::vec3 Camera::get_forward()
{
    return _forward;
}

glm::vec3 Camera::get_right()
{
    return _right;
}

void Camera::set_speed(float speed)
{
    _speed = speed;
}

void Camera::set_mouse_speed(float mouse_speed)
{
    _mouse_speed = mouse_speed;
}

void Camera::set_position(glm::vec3 position)
{
    _position = position;
}

void Camera::set_target(glm::vec3 target)
{
    _target = target;
}

void Camera::set_up(glm::vec3 up)
{
    _up = up;
}

void Camera::set_forward(glm::vec3 forward)
{
    _forward = forward;
}

void Camera::set_right(glm::vec3 right)
{
    _right = right;
}

void Camera::update(GLFWwindow *window, float delta, float mouse_x, float mouse_y)
{
    // Keyboard events
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // W : forwards
    {
        set_position(get_position() + get_forward() * get_speed() * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // S : backwards
    {
        set_position(get_position() - get_forward() * get_speed() * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  // A : left
    {
        set_position(get_position() - get_right() * get_speed() * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  // D : right
    {
        set_position(get_position() + get_right() * get_speed() * delta);
    }

    // Mouse events
    set_forward(get_forward() + get_right() * mouse_x * delta * get_mouse_speed());
    set_forward(get_forward() - get_up()    * mouse_y * delta * get_mouse_speed());
    set_forward(glm::normalize(get_forward()));
    set_right(- cross(get_up(), get_forward()));
}
