#ifndef MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
#define MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
#include <cstdint>

#include "Component.h"

namespace entity
{
class InventoryBagComponent;
class InventorySelectorComponent;

class PlayerControlComponent : public Component
{
public:
    void init() override;

    void update(float delta_time) override;

private:
    /**
     * 获取当前选中快捷栏槽位的方块 id
     * @return 方块注册 id；槽位为空或组件缺失时返回 0（空气）
     */
    [[nodiscard]] std::uint64_t getSelectedBlockId() const;

    InventoryBagComponent *     m_inventory = nullptr; // 背包组件指针（init 时缓存，不拥有）
    InventorySelectorComponent *m_selector  = nullptr; // 快捷栏选择组件指针（init 时缓存，不拥有）

    float m_velocity    = 10.0f;  // 速度
    float m_sensitivity = 0.125f; // 鼠标灵敏度
    float m_xRot        = 0.0f;   // 与 X 轴夹角
    float m_yRot        = 0.0f;   // 与 Y 轴夹角
    float m_breakCooldown = 0.0f; // 破坏冷却（秒），限制按住左键时的破坏速率
};
} // entity

#endif //MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
