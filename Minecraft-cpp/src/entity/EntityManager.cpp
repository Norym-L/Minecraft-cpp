#include "EntityManager.h"

#include <ranges>

#include "entity/components/CameraComponent.h"

namespace entity
{
void EntityManager::update(const float delta_time)
{
    for (const auto &val: m_entitys | std::views::values)
    {
        val->update(delta_time);
    }
}

void EntityManager::removeEntity(const uint64_t id)
{
    std::scoped_lock lock(m_mutex);
    const auto       it = m_entitys.find(id);
    if (it == m_entitys.end())
    {
        return;
    }
    if (it->second.get() == m_mainCameraEntity)
    {
        m_mainCameraEntity = nullptr; // 主相机被删除，置空等待重新定位
    }
    m_entitys.erase(it);
}

void EntityManager::removeNotAliveEntity()
{
    std::scoped_lock lock(m_mutex);
    std::erase_if(m_entitys, [this](const auto &item)
    {
        const auto &entity = item.second;
        if (entity->m_isAlive)
        {
            return false;
        }
        if (entity.get() == m_mainCameraEntity)
        {
            m_mainCameraEntity = nullptr; // 主相机死亡，置空
        }
        return true;
    });
}

void EntityManager::reloadMainCamera()
{
    std::scoped_lock lock(m_mutex);

    const auto it = std::ranges::find_if(m_entitys,
                                         [](const auto &item) -> bool
                                         {
                                             return item.second->template getComponent<entity::CameraComponent>() !=
                                                    nullptr;
                                         });

    m_mainCameraEntity = it != m_entitys.end() ? it->second.get() : nullptr;
}

entity::Entity *EntityManager::getMainCamera() const
{
    return m_mainCameraEntity;
}
}
