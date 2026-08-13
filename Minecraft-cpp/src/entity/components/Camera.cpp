#include "Camera.h"

namespace entity
{
	void Camera::init()
	{
		m_cameraData = {
			.view_matrix = getViewMatrix(),
			.project_matrix = getProjectMatrix(600.0f / 800.0f),
			.ortho_martix = glm::ortho(
				0.0f, static_cast<float>(600),
				0.0f, static_cast<float>(800)),
			.position = m_owner->getPosition()
		};
		m_name = "Camera";
	}

	void Camera::update(float delta_time)
	{

	}

	glm::mat4 Camera::getViewMatrix() const
	{
		auto mat = glm::mat4(1.0f);
		mat = glm::mat4_cast(m_owner->getRotation());

		mat[3] = glm::vec4(m_owner->getPosition(), 1.0f);

		return glm::inverse(mat);
	}

	glm::mat4 Camera::getProjectMatrix(const float aspect) const
	{
		return glm::perspective(glm::radians(m_fov), aspect, m_nearPlane, m_farPlane);
	}

	mymc::CameraData& Camera::getCameraData()
	{
		return m_cameraData;
	}
}
