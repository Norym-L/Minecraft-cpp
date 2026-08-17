#include "Chunk.h"

#include "Minecraft-cpp/logger/SpeedTest.h"
#include "render/RenderQueue.h"

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

    for (int y = yStart; y < yEnd; ++y)
    {
        for (int z = zStart; z < zEnd; ++z)
        {
            for (int x = xStart; x < xEnd; ++x)
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
    m_wasChanged = false;

    for (int index = 0; index < m_sections.size(); ++index)
    {
        if (!m_sections[index].wasChanged())
        {
            continue;
        }

        m_sections[index].createRenderBuf(m_chunkPos.x * 16, index * 16, m_chunkPos.z * 16);
        for (auto command : m_sections[index].m_renderCommands3D)
        {
            m_renderCommands3D.push_back(command);
        }
    }
}
} // server
