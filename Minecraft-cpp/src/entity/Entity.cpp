#include "Entity.h"

namespace entity
{
	std::string& Entity::getName()
	{
		return m_name;
	}

	const glm::vec3& Entity::getPosition() const
	{
		return m_position;
	}

	void Entity::setPosition(const glm::vec3& position)
	{
		m_position = position;
	}

	const glm::quat& Entity::getRotation() const
	{
		return m_rotation;
	}

	void Entity::setRotation(const glm::quat& rotation)
	{
		m_rotation = rotation;
	}

	const std::vector<std::unique_ptr<entity::Component>>& Entity::getComponents() const
	{
		return m_components;
	}
}
