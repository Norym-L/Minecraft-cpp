#include "World.h"

#include <glm/mat4x4.hpp>

#include <array>
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
    constexpr std::array<std::pair<int, int>, 4> kInitChunks = {{
        {0, 0}, {1, 0}, {0, 1}, {1, 1}
    }};

    for (const auto [cx, cz]: kInitChunks)
    {
        auto chunk = std::make_shared<Chunk>();
        chunk->setChunkPos(mymc::ChunkPos{.x = cx, .z = cz});
        {
            std::lock_guard<std::mutex> lock(m_mapMutex);
            m_worldMap[mymc::ChunkPos{.x = cx, .z = cz}] = chunk;
        }
        chunk->m_buildInFlight.store(true);
        if (m_taskPool)
        {
            m_taskPool->submit([chunk, this]() { generateAndBuild(chunk); });
        }
        else
        {
            generateAndBuild(chunk); // 无池（应为 init 后不再发生）：同步构建
        }
    }
}

void World::generateAndBuild(const std::shared_ptr<Chunk> &chunk)
{
    // 地形生成 + 网格构建均为纯 CPU 操作，可在工作线程安全执行
    m_worldMaker.simplexNoise2D(*chunk); // 填充方块数据（含设置 m_wasChanged）
    chunk->m_wasChanged.store(false);
    chunk->buildMeshData();              // CPU 网格 + chunk 内部去重，置 m_uploadRequested
    chunk->m_buildInFlight.store(false);
}

void World::update()
{
    auto dformChunk = [](const mymc::ChunkPos pos1, const mymc::ChunkPos pos2) -> int
        {
        return static_cast<int>(
            std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.z - pos2.z) * (pos1.z - pos2.z)));
        };

    const int visiable_chunk_far = 8;
    auto main_camera_entity_pos = mymc::Core::GetInstance().getEntityManager().getMainCamera()->getPosition();
    // 世界坐标转区块坐标（除以16）
    auto main_chunk_pos = mymc::ChunkPos{
        .x = static_cast<int>(main_camera_entity_pos.x) / 16,
        .z = static_cast<int>(main_camera_entity_pos.z) / 16
        };

    // 卸载：移除过远区块（在飞行中的区块由 shared_ptr 保活，不影响安全）
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

    // 请求阶段：扫描缺失区块并提交 生成+构建 任务（重活全部进线程池）
    for (auto x_offset = -visiable_chunk_far; x_offset <= visiable_chunk_far; ++x_offset)
    {
        for (auto z_offset = -visiable_chunk_far; z_offset <= visiable_chunk_far; ++z_offset)
        {
            mymc::ChunkPos new_pos = {.x = main_chunk_pos.x + x_offset, .z = main_chunk_pos.z + z_offset};
            if (dformChunk(new_pos, main_chunk_pos) > 12)
            {
                continue;
            }
            if (m_worldMap.contains(new_pos))
            {
                continue;
            }

            auto new_chunk = std::make_shared<Chunk>();
            new_chunk->setChunkPos(new_pos);
            {
                std::lock_guard<std::mutex> lock(m_mapMutex);
                m_worldMap[new_pos] = new_chunk;
            }
            new_chunk->m_buildInFlight.store(true);
            if (m_taskPool)
            {
                m_taskPool->submit([new_chunk, this]() { generateAndBuild(new_chunk); });
            }
            else
            {
                generateAndBuild(new_chunk);
            }
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
            continue; // 视锥外：跳过上传与提交
        }
        // CPU 顶点数据由工作线程构建；这里只做 GL 上传（必须在 GL 线程）
        if (chunkPtr->m_uploadRequested.exchange(false))
        {
            chunkPtr->uploadMesh();
        }
        if (!chunkPtr->m_renderCommands3D.empty())
        {
            mymc::Core::GetInstance().getRanderQueue().submit(chunkPtr->m_renderCommands3D);
        }
    }
}
} // server
