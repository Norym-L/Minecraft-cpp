#include "Chunk.h"

#include "Minecraft-cpp/logger/SpeedTest.h"
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
    return m_sections[y / 16].getBlock(x, y, z);
}

void Chunk::createRenderBuf()
{
    if (!m_wasChanged)
    {
        return;
    }

    m_renderCommands3D.clear();
    m_mergedMeshes.clear();
    m_wasChanged = false;

    // 图集方案下所有方块共用同一材质，因此整个区块可以合并成单个网格 + 单条渲染命令。
    // 各 section 网格顶点均为区块局部坐标，合并时只需补上该 section 的 Y 偏移。
    mymc::MeshBuilder builder;
    mymc::Material    *atlas_material = nullptr;

    for (int index = 0; index < static_cast<int>(m_sections.size()); ++index)
    {
        // 仅对发生变化的 section 重新生成网格；未变化的沿用上次生成的网格一并合并，
        // 避免"只改动其中一个 section 时丢失其他 section 的几何"。
        if (m_sections[index].wasChanged())
        {
            m_sections[index].createRenderBuf(m_chunkPos.x * 16, index * 16, m_chunkPos.z * 16);
        }
        for (const auto &command: m_sections[index].m_renderCommands3D)
        {
            if (!command.mesh)
            {
                continue;
            }
            builder.addMesh(command.mesh->getVertices(), command.mesh->getIndices(),
                            command.mesh->getVertexLayout(),
                            glm::vec3(0.0f, static_cast<float>(index) * 16.0f, 0.0f));
            if (!atlas_material)
            {
                atlas_material = command.material;
            }
        }
    }

    auto merged_mesh = builder.build();
    if (!merged_mesh)
    {
        return; // 空区块：无网格可渲染
    }

    m_mergedMeshes.push_back(merged_mesh);
    m_renderCommands3D.push_back({
        .mesh = merged_mesh.get(),
        .material = atlas_material,
        .model_matrix = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(static_cast<float>(m_chunkPos.x) * 16.0f, 0.0f,
                                                 static_cast<float>(m_chunkPos.z) * 16.0f))
    });
}
} // server
