#include "Player.h"

#include "components/CameraComponent.h"

namespace entity
{
	Player::Player(const std::uint64_t id, std::string name)
		: Entity(id, name)
	{
		addComponent<CameraComponent>();
	}

	void Player::update(float delta_time)
	{
		
	}
}
