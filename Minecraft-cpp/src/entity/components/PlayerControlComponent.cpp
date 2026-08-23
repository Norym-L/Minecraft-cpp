#include "PlayerControlComponent.h"

#include <algorithm>

#include "core/Core.h"
#include "InventoryBagComponent.h"
#include "InventorySelectorComponent.h"

namespace entity
{
void PlayerControlComponent::init()
{
    // 缓存所属实体的物品栏组件指针（须在 Player 构造中先于本组件装配）
    // 指针指向 Entity::m_components 中的堆对象，实体存活期间稳定
    m_inventory = m_owner->getComponent<InventoryBagComponent>();
    m_selector  = m_owner->getComponent<InventorySelectorComponent>();
}

std::uint64_t PlayerControlComponent::getSelectedBlockId() const
{
    if (m_inventory == nullptr || m_selector == nullptr)
    {
        return 0; // 组件缺失时视为空气（无法放置）
    }
    return m_inventory->getHotbarSlot(m_selector->getSelectedIndex()).blockId;
}

void PlayerControlComponent::update(float delta_time)
{
    const auto &input_manager = mymc::Core::GetInstance().getInputManager();
    auto        rotation      = m_owner->getRotation();


    if (input_manager.isMousePositionChanged() && input_manager.isMousePositionChangedEventEnable())
    {
        const auto &old_pos     = input_manager.getMousePositionOld();
        const auto &current_pos = input_manager.getMousePositionCurrent();

        const float delta_x = current_pos.x - old_pos.x;
        const float delta_y = current_pos.y - old_pos.y;

        //绕Y轴旋转
        const float y_delta_angle = -delta_x * m_sensitivity;
        m_yRot                    += y_delta_angle;
        const glm::quat y_rot     = glm::angleAxis(glm::radians(m_yRot), glm::vec3(0.0f, 1.0f, 0.0f));

        //绕X轴旋转
        const float x_delta_angle = -delta_y * m_sensitivity;
        m_xRot                    += x_delta_angle;
        m_xRot                    = std::clamp(m_xRot, -89.99f, 89.99f);
        const glm::quat x_rot     = glm::angleAxis(glm::radians(m_xRot), glm::vec3(1.0f, 0.0f, 0.0f));

        rotation = glm::normalize(y_rot * x_rot);

        m_owner->setRotation(rotation);
    }

    // 前/右方向（水平投影）
    glm::vec3 front = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
    front.y         = 0.0f;
    front           = glm::normalize(front);

    glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
    right.y         = 0.0f;
    right           = glm::normalize(right);

    // WASD 期望方向
    glm::vec3 wish_dir(0.0f);
    if (input_manager.isKeyPressed(GLFW_KEY_A)) wish_dir -= right;
    if (input_manager.isKeyPressed(GLFW_KEY_D)) wish_dir += right;
    if (input_manager.isKeyPressed(GLFW_KEY_W)) wish_dir += front;
    if (input_manager.isKeyPressed(GLFW_KEY_S)) wish_dir -= front;
    if (glm::dot(wish_dir, wish_dir) > 0.0f)
    {
        wish_dir = glm::normalize(wish_dir) * m_velocity * delta_time;
    }

    if (input_manager.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        wish_dir -= glm::vec3(0.0f, 1.0f, 0.0f) * delta_time * m_velocity / 3.0f;
    if (input_manager.isKeyPressed(GLFW_KEY_SPACE))
        wish_dir += glm::vec3(0.0f, 1.0f, 0.0f) * delta_time * m_velocity / 3.0f;

    m_owner->setPosition(m_owner->getPosition() + wish_dir);

    // ===== 方块交互（射线拾取 + 放置/破坏）=====
    auto &world = mymc::Core::GetInstance().getWorld();
    const glm::vec3 eye_dir  = glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    const glm::vec3 eye_pos  = m_owner->getPosition() + eye_dir * 0.1f; // 轻微前移，避免点到自身格
    constexpr float kReach   = 6.0f;
    constexpr float kBreakInterval = 0.15f; // 按住左键每 0.15s 破坏一块

    m_breakCooldown -= delta_time;

    // 左键：按住持续破坏（目标块变为空气）
    if (input_manager.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && m_breakCooldown <= 0.0f)
    {
        server::World::RaycastHit hit;
        if (world.raycastBlock(eye_pos, eye_dir, kReach, hit))
        {
            world.setBlockAt(hit.blockX, hit.blockY, hit.blockZ, 0);
            m_breakCooldown = kBreakInterval;
        }
    }

    // 右键：单击放置当前选中快捷栏槽位的方块到命中面的相邻格
    if (input_manager.wasMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        const std::uint64_t selected_block_id = getSelectedBlockId();
        if (selected_block_id != 0) // 空槽（空气）不执行放置，避免误删相邻方块
        {
            server::World::RaycastHit hit;
            if (world.raycastBlock(eye_pos, eye_dir, kReach, hit))
            {
                world.setBlockAt(hit.blockX + hit.faceX, hit.blockY + hit.faceY, hit.blockZ + hit.faceZ,
                                 selected_block_id);
            }
        }
    }
}
} // entity