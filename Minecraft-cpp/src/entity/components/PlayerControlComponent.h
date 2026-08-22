#ifndef MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
#define MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
#include <cstdint>

#include "Component.h"

namespace entity
{
class PlayerControlComponent : public Component
{
public:
    void init() override;

    void update(float delta_time) override;

private:
    float m_velocity    = 10.0f;  // 速度
    float m_sensitivity = 0.125f; // 鼠标灵敏度
    float m_xRot        = 0.0f;   // 与 X 轴夹角
    float m_yRot        = 0.0f;   // 与 Y 轴夹角

    std::uint64_t m_selectedBlockId = 1; // 当前选中的放置方块（预留物品栏接口，默认石头）
    float m_breakCooldown = 0.0f;        // 破坏冷却（秒），限制按住左键时的破坏速率
};
} // entity

#endif //MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
