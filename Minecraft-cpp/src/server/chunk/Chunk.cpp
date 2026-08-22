#include "Chunk.h"

#include "Minecraft-cpp/logger/SpeedTest.h"
#include "core/Core.h"
#include "render/Mesh.h"
#include "render/MeshBuilder.h"
#include "render/RenderQueue.h"

#include <glm/gtc/matrix_transform.hpp>

namespace server
{
mymc::ChunkPos Chunk::getChunkPos() const
{
    return m_chunkPos;
}

void Chunk::setChunkPos(const mymc::ChunkPos pos)
{
    m_wasChanged = true;
    m_chunkPos = pos;
}

void Chunk::fillBlocks(const int x1, const int y1, const int z1, const int x2, const int y2, const int z2,
                       const std::uint64_t block_id)
{
    m_wasChanged = true;

    const int xStart = std::min(x1, x2), xEnd = std::max(x1, x2);
    const int yStart = std::min(y1, y2), yEnd = std::max(y1, y2);
    const int zStart = std::min(z1, z2), zEnd = std::max(z1, z2);

    for (int y = yStart; y <= yEnd; ++y)
    {
        for (int z = zStart; z <= zEnd; ++z)
        {
            for (int x = xStart; x <= xEnd; ++x)
            {
                setBlock(x, y, z, block_id);
            }
        }
    }
}

void Chunk::fill(const std::uint64_t block_id)
{
    m_wasChanged = true;
    for (auto &sections: m_sections)
    {
        sections.fill(block_id);
    }
}

std::uint64_t Chunk::getBlock(const int x, const int y, const int z) const
{
    // 越界保护：外部调用方可能传入世界边界外的坐标
    if (x < 0 || x >= 16 || z < 0 || z >= 16 || y < 0 || y >= 256)
    {
        return 0; // 空气
    }
    // y 为全局 0..255，section 需要的是局部 0..15（与 setBlock 的 y%16 一致）
    return m_sections[y / 16].getBlock(x, y % 16, z);
}

std::uint64_t Chunk::getBlockSafe(const int x, const int y, const int z) const
{
    std::lock_guard<std::mutex> lock(m_blockMutex);
    return getBlock(x, y, z);
}

void Chunk::buildMeshData()
{
    // 全程持块数据锁：网格构建期间读取方块数据，须与主线程编辑（setBlock）互斥
    std::lock_guard<std::mutex> lock(m_blockMutex);

    m_meshData.reset();
    m_uploadRequested.store(false);

    // 图集方案下所有方块共用同一材质：整个区块合并成单个 MeshData（chunk 内部去重）。
    // 各 section 顶点均为该 section 局部坐标，合并时补上 Y 偏移。
    mymc::MeshBuilder builder;

    for (int index = 0; index < static_cast<int>(m_sections.size()); ++index)
    {
        // 仅对发生变化的 section 重新生成网格；未变化的沿用上次生成的网格一并合并，
        // 避免"只改动其中一个 section 时丢失其他 section 的几何"。
        if (m_sections[index].wasChanged())
        {
            m_sections[index].buildMeshData(m_chunkPos.x * 16, index * 16, m_chunkPos.z * 16);
        }
        for (const auto &data: m_sections[index].m_meshData)
        {
            if (!data)
            {
                continue;
            }
            builder.addMeshDeduplicated(data->vertices, data->indices, data->layout,
                                        glm::vec3(0.0f, static_cast<float>(index) * 16.0f, 0.0f));
        }
    }

    m_meshData = builder.buildMeshData();
    m_uploadRequested.store(m_meshData != nullptr);
}

void Chunk::uploadMesh()
{
    if (!m_meshData)
    {
        return;
    }

    m_mesh = std::make_shared<mymc::Mesh>(*m_meshData); // GL 上传（必须在 GL 线程调用）
    m_meshData.reset();
    m_uploadRequested.store(false);

    if (!m_mesh)
    {
        return;
    }

    m_renderCommands3D.clear();
    m_renderCommands3D.push_back({
        .mesh = m_mesh.get(),
        .material = mymc::Core::GetInstance().getBlockManager().getAtlasMaterial().get(),
        .model_matrix = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(static_cast<float>(m_chunkPos.x) * 16.0f, 0.0f,
                                                 static_cast<float>(m_chunkPos.z) * 16.0f)),
    });
}
} // server
