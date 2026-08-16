#include "PlayerControlComponent.h"

#include <algorithm>

#include "core/Core.h"

namespace entity
{
void PlayerControlComponent::init()
{}

void PlayerControlComponent::update(float delta_time)
{
    const auto &input_manager = mymc::Core::GetInstance().getInputManager();
    auto        rotation      = m_owner->getRotation();


    if (input_manager.isMousePositionChanged() && input_manager.isMousePositionChangedEventEnable())
    {
        const auto &old_pos     = input_manager.getMousePositionOld();
        const auto &current_pos = input_manager.getMousePositionCurrent();

        const float delta_x = current_pos.x - old_pos.x;
        const float delta_y = current_pos.y - old_pos.y;

        //绕Y轴旋转
        const float y_delta_angle = -delta_x * m_sensitivity;
        m_yRot                    += y_delta_angle;
        const glm::quat y_rot     = glm::angleAxis(glm::radians(m_yRot), glm::vec3(0.0f, 1.0f, 0.0f));

        //绕X轴旋转
        const float x_delta_angle = -delta_y * m_sensitivity;
        m_xRot                    += x_delta_angle;
        m_xRot                    = std::clamp(m_xRot, -89.99f, 89.99f);
        const glm::quat x_rot     = glm::angleAxis(glm::radians(m_xRot), glm::vec3(1.0f, 0.0f, 0.0f));

        rotation = glm::normalize(y_rot * x_rot);

        m_owner->setRotation(rotation);
    }

    // 前/右方向（水平投影）
    glm::vec3 front = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    front.y         = 0.0f;
    front           = glm::normalize(front);

    glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    right.y         = 0.0f;
    right           = glm::normalize(right);

    // WASD 期望方向
    glm::vec3 wish_dir(0.0f);
    if (input_manager.isKeyPressed(GLFW_KEY_A)) wish_dir -= right;
    if (input_manager.isKeyPressed(GLFW_KEY_D)) wish_dir += right;
    if (input_manager.isKeyPressed(GLFW_KEY_W)) wish_dir += front;
    if (input_manager.isKeyPressed(GLFW_KEY_S)) wish_dir -= front;
    if (glm::dot(wish_dir, wish_dir) > 0.0f)
    {
        wish_dir = glm::normalize(wish_dir) * m_velocity * delta_time;
    }

    if (input_manager.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        wish_dir -= glm::vec3(0.0f, 1.0f, 0.0f) * delta_time * m_velocity / 3.0f;
    if (input_manager.isKeyPressed(GLFW_KEY_SPACE))
        wish_dir += glm::vec3(0.0f, 1.0f, 0.0f) * delta_time * m_velocity / 3.0f;

    m_owner->setPosition(m_owner->getPosition() + wish_dir);
}
} // entity