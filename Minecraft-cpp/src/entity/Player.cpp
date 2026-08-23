#include "Player.h"

#include <utility>

#include "components/CameraComponent.h"
#include "components/InventoryBagComponent.h"
#include "components/InventorySelectorComponent.h"
#include "components/PlayerControlComponent.h"

namespace entity
{
Player::Player(const std::uint64_t id, std::string name)
    : Entity(id, std::move(name))
{
    // 物品栏组件须先于 PlayerControlComponent 装配，其 init() 才能缓存组件指针
    auto *inventory = addComponent<InventoryBagComponent>();
    addComponent<InventorySelectorComponent>();

    // 初始给快捷栏发放测试方块（1 = 石头，2 = 草，见 BlockManager 注册表）
    inventory->setHotbarSlot(0, ItemStack{1, 64});
    inventory->setHotbarSlot(1, ItemStack{2, 64});

    addComponent<CameraComponent>();
    addComponent<PlayerControlComponent>();
}

void Player::update(const float delta_time)
{
    Entity::update(delta_time);
}
}
