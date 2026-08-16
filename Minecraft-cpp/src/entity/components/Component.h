#pragma once
#include "entity/Entity.h"

namespace entity
{
class Entity;

class Component
{
public:
    virtual ~Component() = default;

    /**
		 * 初始化组件
		 */
    virtual void init() = 0;

    /**
		 * 更新组件
		 * @param delta_time 帧时间
		 */
    virtual void update(float delta_time) = 0;

    /**
		 * 获取此组件的所有者
		 * @return 所有者引用
		 */
    [[nodiscard]] const Entity &getOwner() const;

    /**
		 * 设置此组件的所有者
		 * @param entity 所有者指针
		 */
    void setOwner(Entity *entity);

    /**
		 * 获取组件名称
		 * @return 名称字符串
		 */
    [[nodiscard]] const std::string &getName() const;

protected:
    std::string m_name;            // 组件名称
    Entity *    m_owner = nullptr; // 所有者指针
};
}
