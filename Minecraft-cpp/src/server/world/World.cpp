#include "World.h"
#include "render/RenderQueue.h"

#include <ranges>

#include "WorldMaker.h"
#include "core/Core.h"

namespace server
{
void World::init()
{
    auto chunk1 = std::make_unique<Chunk>();
    auto chunk2 = std::make_unique<Chunk>();
    auto chunk3 = std::make_unique<Chunk>();
    auto chunk4 = std::make_unique<Chunk>();
    chunk1->m_chunkPos = {.x = 0,.z = 0};
    chunk2->m_chunkPos = {.x = 1,.z = 0};
    chunk3->m_chunkPos = {.x = 0,.z = 1};
    chunk4->m_chunkPos = {.x = 1,.z = 1};
    // chunk->m_sections[0].fill(1);
    // chunk->setBlock(8, 0, 1, 0);
    // chunk->setBlock(1, 3, 1, 2);
    // chunk->setBlock(8, 2, 3, 2);
    // chunk->setBlock(0, 10, 3, 2);
    // chunk->setBlock(8, 12, 0, 0);
    // chunk->fillBlocks(1,3,4,4,4,15,0);
    // chunk->fillBlocks(13,13,4,15,15,15,0);
    // chunk->fillBlocks(13,20,4,15,40,15,2);
    // chunk->fillBlocks(8,10,5,13,20,14,3);

    WorldMaker world_maker(100000000);
    world_maker.valueNoise(*chunk1.get());
    world_maker.valueNoise(*chunk2.get());
    world_maker.valueNoise(*chunk3.get());
    world_maker.valueNoise(*chunk4.get());

    m_worldMap[mymc::ChunkPos{.x = 0, .z = 0}] = std::move(chunk1);
    m_worldMap[mymc::ChunkPos{.x = 1, .z = 0}] = std::move(chunk2);
    m_worldMap[mymc::ChunkPos{.x = 0, .z = 1}] = std::move(chunk3);
    m_worldMap[mymc::ChunkPos{.x = 1, .z = 1}] = std::move(chunk4);
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
