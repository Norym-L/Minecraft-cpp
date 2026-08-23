#include "CameraComponent.h"

#include <mutex>

namespace entity
{
void CameraComponent::init()
{
    std::scoped_lock lock(m_cameraDataMutex);
    m_cameraData = {
        .view_matrix    = getViewMatrix(),
        .project_matrix = getProjectMatrix(800.0f / 600.0f), // to do 实现从Core获取屏幕宽高比
        .ortho_martix   = glm::ortho(
            0.0f, static_cast<float>(m_windowWidth),
            0.0f, static_cast<float>(m_windowHeight)),
        .position = m_owner->getPosition()
    };
    m_name = "Camera";
}

void CameraComponent::update(float delta_time)
{
    std::scoped_lock lock(m_cameraDataMutex);
    m_cameraData.view_matrix    = getViewMatrix();
    m_cameraData.project_matrix =
            getProjectMatrix(static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight));
    m_cameraData.ortho_martix = glm::ortho(
        0.0f, static_cast<float>(m_windowWidth),
        0.0f, static_cast<float>(m_windowHeight));
    m_cameraData.position = m_owner->getPosition();
}

glm::mat4 CameraComponent::getViewMatrix() const
{
    auto mat = glm::mat4(1.0f);
    mat      = glm::mat4_cast(m_owner->getRotation());

    mat[3] = glm::vec4(m_owner->getPosition(), 1.0f);

    return glm::inverse(mat);
}

glm::mat4 CameraComponent::getProjectMatrix(const float aspect) const
{
    return glm::perspective(glm::radians(m_fov), aspect, m_nearPlane, m_farPlane);
}

mymc::CameraData &CameraComponent::getCameraData()
{
    std::scoped_lock lock(m_cameraDataMutex);
    return m_cameraData;
}

void CameraComponent::setWindowSize(const int width, const int height)
{
    m_windowWidth  = width;
    m_windowHeight = height;
}
}
