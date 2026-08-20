#include "World.h"

#include "render/RenderQueue.h"

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
    chunk1->m_chunkPos = {.x = 0, .z = 0};
    chunk2->m_chunkPos = {.x = 1, .z = 0};
    chunk3->m_chunkPos = {.x = 0, .z = 1};
    chunk4->m_chunkPos = {.x = 1, .z = 1};

    const WorldMaker world_maker(100000000);
    world_maker.simplexNoise2D(*chunk1.get());
    world_maker.simplexNoise2D(*chunk2.get());
    world_maker.simplexNoise2D(*chunk3.get());
    world_maker.simplexNoise2D(*chunk4.get());

    m_worldMap[mymc::ChunkPos{.x = 0, .z = 0}] = std::move(chunk1);
    m_worldMap[mymc::ChunkPos{.x = 1, .z = 0}] = std::move(chunk2);
    m_worldMap[mymc::ChunkPos{.x = 0, .z = 1}] = std::move(chunk3);
    m_worldMap[mymc::ChunkPos{.x = 1, .z = 1}] = std::move(chunk4);
}

void World::update()
{
    auto dformChunk = [](const mymc::ChunkPos pos1, const mymc::ChunkPos pos2) -> int
        {
        return static_cast<int>(
            std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.z - pos2.z) * (pos1.z - pos2.z)));
        };

    int visiable_chunk_far = 8;
    auto main_camera_entity_pos = mymc::Core::GetInstance().getEntityManager().getMainCamera()->getPosition();
    // 世界坐标转区块坐标（除以16）
    auto main_chunk_pos = mymc::ChunkPos{
        .x = static_cast<int>(main_camera_entity_pos.x) / 16,
        .z = static_cast<int>(main_camera_entity_pos.z) / 16
        };
    for (auto it = m_worldMap.begin(); it != m_worldMap.end();)
    {
        if (dformChunk(it->first, main_chunk_pos) > visiable_chunk_far + 4)
        {
            std::lock_guard<std::mutex> lock(m_mapMutex);
            it = m_worldMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
    for (auto x_offset = -visiable_chunk_far; x_offset <= visiable_chunk_far; ++x_offset)
    {
        for (auto z_offset = -visiable_chunk_far; z_offset <= visiable_chunk_far; ++z_offset)
        {
            mymc::ChunkPos new_pos = {.x = main_chunk_pos.x + x_offset, .z = main_chunk_pos.z + z_offset};
            if (dformChunk(new_pos, main_chunk_pos) > 12)
            {
                continue;
            }
            // 跳过已存在的区块
            if (m_worldMap.contains(new_pos))
            {
                continue;
            }
            auto new_chunk = std::make_unique<Chunk>();
            new_chunk->setChunkPos(new_pos);
            m_worldMaker.simplexNoise2D(*new_chunk.get());
            std::lock_guard<std::mutex> lock(m_mapMutex);
            m_worldMap[new_pos] = std::move(new_chunk);
        }
    }
}

void World::render()
{
    std::lock_guard<std::mutex> lock(m_mapMutex);
    for (auto &[pos, chunkPtr] : m_worldMap)
    {
        chunkPtr->createRenderBuf();
        mymc::Core::GetInstance().getRanderQueue().submit(chunkPtr->m_renderCommands3D);
    }
}
} // server
