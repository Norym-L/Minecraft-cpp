#include "Player.h"

#include "components/Camera.h"

namespace entity
{
	Player::Player()
	{
		addComponent<Camera>();
	}

	void Player::update(float delta_time)
	{
		
	}
}
