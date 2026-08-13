#pragma once
#include "Entity.h"
#include "Common.h"

namespace entity
{
	class Player : public Entity
	{
	public:
		Player(const std::uint64_t id, std::string name);

		void update(float delta_time) override;

	private:
	};
}
