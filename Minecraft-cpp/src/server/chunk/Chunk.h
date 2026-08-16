#ifndef MINECRAFT_CPP_CHUNK_H
#define MINECRAFT_CPP_CHUNK_H
#include <memory>

#include "ChunkSections.h"
#include "Common.h"

namespace server
{
class Chunk // TODO 区块加载和世界生成
{
public:
    /**
         * 获取区块坐标
         * @return 坐标结构体
         */
    [[nodiscard]] mymc::ChunkPos getChunkPos() const;

    /**
         * 设置区块坐标
         * @param pos 坐标结构体
         */
    void setChunkPos(mymc::ChunkPos pos);

    /**
         * 设置方块
         * @param x 0~15
         * @param y 0~255
         * @param z 0~15
         * @param block_id 方块的注册id
         */
    void setBlock(const int x, const int y, const int z, const std::uint64_t block_id)
    {
        m_sections[y / 16].setBlock(x, y, z, block_id);
    }

    /**
         * 填充方块
         * @param x1 0~15
         * @param y1 0~255
         * @param z1 0~15
         * @param x2 0~15
         * @param y2 0~255
         * @param z2 0~15
         * @param block_id 方块的注册id
         */
    void fillBlocks(int x1, int y1, int z1, int x2, int y2, int z2, std::uint64_t block_id);

    void fill(std::uint64_t block_id);

    /**
         * 获取方块注册id
         * @param x 0~15
         * @param y 0~255
         * @param z 0~15
         * @return 方块的注册id
         */
    [[nodiscard]] inline std::uint64_t getBlock(int x, int y, int z) const;

    /**
         * TODO 临时渲染
         */
    void render() const;

private:
    mymc::ChunkPos                m_chunkPos = {}; // 区块的坐标
    std::array<ChunkSections, 16> m_sections;      // 区块垂直切片
};
} // server

#endif //MINECRAFT_CPP_CHUNK_H
