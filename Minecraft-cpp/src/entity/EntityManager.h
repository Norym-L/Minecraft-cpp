#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>

#include "Entity.h"

namespace entity
{
    /**
	 * 实体管理器（多线程安全）
	 * - ID 分配：原子计数器，无锁
	 * - 容器写入：互斥锁保护
	 */
    class EntityManager
    {
    public:
        /**
		 * 更新管理的所有实体
		 * @param delta_time 帧时间
		 */
        void update(float delta_time);

        /**
		 * 添加实体（直接构造）
		 * @tparam T 实体类型（需派生自 entity::Entity）
		 * @tparam Args 构造参数（ID 作为第一个参数传入）
		 * @return 新实体指针
		 */
        template<typename T, typename... Args>
        T* addEntity(Args &&... args)
        {
            static_assert(std::is_base_of_v<entity::Entity, T>, "T must be derived from Entity");

            std::uint64_t id = m_nextId.fetch_add(1, std::memory_order_relaxed);

            auto entity = std::make_unique<T>(id, std::forward<Args>(args)...);
            T* ptr = entity.get();

            std::scoped_lock lock(m_mutex);
            auto [it, inserted] = m_entitys.emplace(id, std::move(entity));
            if (!inserted)
            {
                return nullptr; // 理论上不会发生（原子分配）
            }

            return ptr;
        }

        /**
		 * 删除实体
		 * @param id 实体id
		 */
        void removeEntity(uint64_t id);

        /**
		 * 删除所有非存活实体
		 */
        void removeNotAliveEntity();

        /**
		 * 重新定位主相机所在实体
		 */
        void reloadMainCamera();

        /**
		 * 获取主相机所在实体
		 * @return 实体指针，无相机返回 nullptr
		 */
        Entity* getMainCamera() const;

    private:
        std::atomic<std::uint64_t> m_nextId{1};                              // 原子 ID 分配器，从 1 开始
        std::mutex m_mutex;                                                    // 保护 m_entitys
        std::unordered_map<std::uint64_t, std::unique_ptr<Entity> > m_entitys; // 实体容器
        Entity* m_mainCameraEntity = nullptr;                                  // 主相机所在实体指针
    };
}
