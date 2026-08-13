#pragma once

namespace block
{
	class Block
	{
	public:
		virtual ~Block() = default;

		virtual void update(float delta_time) = 0;
	};
}
