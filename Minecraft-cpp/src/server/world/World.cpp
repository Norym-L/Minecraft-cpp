#include "World.h"

#include <glm/mat4x4.hpp>

#include <cmath>
#include <iostream>

#include "render/RenderQueue.h"

#include "WorldMaker.h"
#include "core/Core.h"
#include "entity/components/CameraComponent.h"

namespace
{
/// @brief 从「视图 x 投影」矩阵提取视锥 6 个平面（Gribb-Hartmann 方法，法线指向视锥内部）
/// @param vp 视图 x 投影矩阵（列主序）
/// @param planes 输出：左/右/下/上/近/远，已归一化；点在平面内侧满足 dot(normal, p) + d >= 0
void extractFrustumPlanes(const glm::mat4 &vp, glm::vec4 planes[6])
{
    // GLM 为列主序（vp[col][row]），转置后 vp_t[row] 即为矩阵第 row 行
    const glm::mat4 vp_t = glm::transpose(vp);
    const glm::vec4 row0 = vp_t[0];
    const glm::vec4 row1 = vp_t[1];
    const glm::vec4 row2 = vp_t[2];
    const glm::vec4 row3 = vp_t[3];

    planes[0] = row3 + row0; // 左：x + w >= 0
    planes[1] = row3 - row0; // 右：w - x >= 0
    planes[2] = row3 + row1; // 下：y + w >= 0
    planes[3] = row3 - row1; // 上：w - y >= 0
    planes[4] = row3 + row2; // 近：z + w >= 0
    planes[5] = row3 - row2; // 远：w - z >= 0

    for (int i = 0; i < 6; ++i)
    {
        const float len = glm::length(glm::vec3(planes[i]));
        planes[i] /= len;
    }
}

/// @brief AABB 与视锥的保守相交测试（宽松：只裁掉确定在视锥外的盒子，不会误裁可见物体）
/// @return true 表示可能可见，需要渲染
bool aabbInFrustum(const glm::vec4 planes[6], const glm::vec3 &min_b, const glm::vec3 &max_b)
{
    for (int i = 0; i < 6; ++i)
    {
        // 取平面法线方向的「正向角点」：若它仍在平面外侧，则整个 AABB 都在该平面外侧
        const glm::vec3 positive_vertex(
            planes[i].x > 0.0f ? max_b.x : min_b.x,
            planes[i].y > 0.0f ? max_b.y : min_b.y,
            planes[i].z > 0.0f ? max_b.z : min_b.z);

        if (glm::dot(glm::vec3(planes[i]), positive_vertex) + planes[i].w < 0.0f)
        {
            return false;
        }
    }
    return true;
}
} // namespace

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
    const auto camera_entity = mymc::Core::GetInstance().getEntityManager().getMainCamera();
    const auto camera_component = camera_entity->getComponent<entity::CameraComponent>();
    const auto &camera_data = camera_component->getCameraData();

    // 每帧提取一次视锥平面（投影 x 视图）
    glm::vec4 frustum_planes[6];
    extractFrustumPlanes(camera_data.project_matrix * camera_data.view_matrix, frustum_planes);

    std::lock_guard<std::mutex> lock(m_mapMutex);

    for (auto &[pos, chunkPtr]: m_worldMap)
    {
        // 区块包围盒：16x256x16（网格模型已按区块原点做了世界坐标偏移）
        const float min_x = static_cast<float>(pos.x) * 16.0f;
        const float min_z = static_cast<float>(pos.z) * 16.0f;
        if (!aabbInFrustum(frustum_planes,
                           glm::vec3(min_x, 0.0f, min_z),
                           glm::vec3(min_x + 16.0f, 256.0f, min_z + 16.0f)))
        {
            continue; // 视锥外：跳过网格重建与提交
        }
        if (chunkPtr->m_wasChanged)
        {
            chunkPtr->createRenderBuf();
        }
        mymc::Core::GetInstance().getRanderQueue().submit(chunkPtr->m_renderCommands3D);
    }
}
} // server
