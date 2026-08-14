#include "Entity.h"

namespace entity
{
	Entity::Entity(const std::uint64_t id, std::string name)
		: m_id(id), m_name(std::move(name))
	{}

	std::uint64_t Entity::getId() const
	{
		return m_id;
	}

	void Entity::update(const float delta_time)
	{
		if (!m_components.empty())
		{
			for (const auto& it : m_components)
			{
				it->update(delta_time);
			}
		}
	}

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

	const std::vector<std::unique_ptr<Component>>& Entity::getComponents() const
	{
		return m_components;
	}
}
