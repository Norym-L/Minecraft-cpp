#include "Chunk.h"

namespace server
{
mymc::ChunkPos Chunk::getChunkPos() const
{
    return m_chunkPos;
}

void Chunk::setChunkPos(const mymc::ChunkPos pos)
{
    m_chunkPos = pos;
}

void Chunk::fillBlocks(const int           x1, const int y1, const int z1, const int x2, const int y2, const int z2,
                       const std::uint64_t block_id)
{
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
    for (auto &sections: m_sections)
    {
        sections.fill(block_id);
    }
}

std::uint64_t Chunk::getBlock(const int x, const int y, const int z) const
{
    return m_sections[y / 16].getBlock(x, y, z);
}

void Chunk::render() const
{
    for (int index = 0; index < m_sections.size(); ++index)
    {
        m_sections[index].render(m_chunkPos.x * 16, index * 16, m_chunkPos.z * 16);
    }
}
} // server
