#pragma once
#include <unordered_map>

#include "entity/Entity.h"


namespace mymc
{
	/**
	 * 实体管理器
	 */
	class EntityManager
	{
	private:
		std::unordered_map<std::uint64_t, entity::Entity> m_entitys; // 实体容器
	};
}
