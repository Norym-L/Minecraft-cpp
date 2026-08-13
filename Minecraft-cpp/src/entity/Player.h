#pragma once
#include "Entity.h"
#include "Common.h"

namespace entity
{
	class Player : public Entity
	{
	public:
		Player();

		void update(float delta_time) override;

	private:
	};
}
