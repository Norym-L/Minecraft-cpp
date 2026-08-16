#include "Player.h"

#include <utility>

#include "components/CameraComponent.h"
#include "components/PlayerControlComponent.h"

namespace entity
{
Player::Player(const std::uint64_t id, std::string name)
    : Entity(id, std::move(name))
{
    addComponent<CameraComponent>();
    addComponent<PlayerControlComponent>();
}

void Player::update(const float delta_time)
{
    Entity::update(delta_time);
}
}
