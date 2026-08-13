#include "Component.h"

namespace entity
{
	const Entity& Component::getOwner() const
	{
		return *m_owner;
	}

	void Component::setOwner(Entity* entity)
	{
		m_owner = entity;
	}

	const std::string& Component::getName() const
	{
		return m_name;
	}
}
