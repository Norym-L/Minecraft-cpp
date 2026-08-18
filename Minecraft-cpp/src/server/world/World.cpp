#include "World.h"
#include "render/RenderQueue.h"

#include <ranges>

#include "core/Core.h"

namespace server
{
void World::init()
{
    auto chunk = std::make_unique<Chunk>();
    chunk->m_sections[0].fill(1);
    chunk->setBlock(8, 0, 1, 0);
    chunk->setBlock(1, 3, 1, 0);
    chunk->setBlock(8, 2, 3, 0);
    chunk->setBlock(0, 10, 3, 0);
    chunk->setBlock(8, 12, 0, 0);
    chunk->fillBlocks(1,3,4,4,4,15,0);
    chunk->fillBlocks(13,13,4,15,15,15,0);
    m_worldMap[mymc::ChunkPos{.x = 0, .z = 0}] = std::move(chunk);
}

void World::render() const
{
    for (const auto &val: m_worldMap | std::views::values)
    {
        val.get()->createRenderBuf();
        mymc::Core::GetInstance().getRanderQueue().submit(val.get()->m_renderCommands3D);
    }
}
} // server
