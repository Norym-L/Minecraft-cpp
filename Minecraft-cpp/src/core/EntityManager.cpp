#include "EntityManager.h"

#include "entity/components/CameraComponent.h"

namespace mymc
{
	void EntityManager::removeEntity(const uint64_t id)
	{
		std::scoped_lock lock(m_mutex);
		auto it = m_entitys.find(id);
		if (it == m_entitys.end())
		{
			return;
		}
		if (it->second.get() == m_mainCameraEntity)
		{
			m_mainCameraEntity = nullptr;	// 主相机被删除，置空等待重新定位
		}
		m_entitys.erase(it);
	}

	void EntityManager::removeNotAliveEntity()
	{
		std::scoped_lock lock(m_mutex);
		std::erase_if(m_entitys, [this](const auto& item)
			{
				const auto& entity = item.second;
				if (entity->m_isAlive)
				{
					return false;
				}
				if (entity.get() == m_mainCameraEntity)
				{
					m_mainCameraEntity = nullptr;	// 主相机死亡，置空
				}
				return true;
			});
	}

	void EntityManager::reloadMainCamera()
	{
		std::scoped_lock lock(m_mutex);

		auto it = std::find_if(m_entitys.begin(), m_entitys.end(),
			[](const auto& item) -> bool
			{
				return item.second->getComponent<entity::CameraComponent>() != nullptr;
			});

		m_mainCameraEntity = (it != m_entitys.end()) ? it->second.get() : nullptr;
	}

	entity::Entity* EntityManager::getMainCamera() const
	{
		return m_mainCameraEntity;
	}
}
