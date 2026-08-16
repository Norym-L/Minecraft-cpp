#include "World.h"

#include <ranges>

namespace server
{
void World::init()
{
    auto chunk = std::make_unique<Chunk>();
    chunk->setBlock(1, 1, 1, 1);
    chunk->setBlock(1, 3, 1, 1);
    chunk->setBlock(8, 2, 3, 1);
    m_worldMap[mymc::ChunkPos{.x = 0, .z = 0}] = std::move(chunk);
}

void World::render() const
{
    for (const auto &val: m_worldMap | std::views::values)
    {
        val.get()->render();
    }
}
} // server
