#ifndef MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
#define MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
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
};
} // entity

#endif //MINECRAFT_CPP_PLAYERCONTROLCOMPONENT_H
