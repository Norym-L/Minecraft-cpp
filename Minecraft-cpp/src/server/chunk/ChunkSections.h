// 区块的垂直切片 16*16*16
#ifndef MINECRAFT_CPP_CHUNKSECTIONS_H
#define MINECRAFT_CPP_CHUNKSECTIONS_H

#include <cstdint>
#include <array>
#include <memory>
#include <vector>

#include "render/Mesh.h"

namespace mymc
{
struct RenderCommand;
}

namespace server
{
class ChunkSections
{
public:
    friend class Chunk;

public:
    ChunkSections() = default;

    /**
         * 设置方块
         * @param x 0~15
         * @param y 0~15
         * @param z 0~15
         * @param block_id 方块的注册id
         */
    void setBlock(const int x, const int y, const int z, const std::uint64_t block_id)
    {
        m_blockIDs[GetBlockIndex(x, y, z)] = block_id;
        m_wasChanged = true;
    }

    /**
         * 获取方块注册id
         * @param x 0~15
         * @param y 0~15
         * @param z 0~15
         * @return 方块的注册id
         */
    [[nodiscard]] std::uint64_t getBlock(const int x, const int y, const int z) const
    {
        return m_blockIDs[GetBlockIndex(x, y, z)];
    }

    /**
         * 完全填充
         * @param block_id 方块的注册id
         */
    void fill(std::uint64_t block_id);

    /**
     * 建立渲染缓存
     * @param x_offset x偏移量
     * @param y_offset y偏移量
     * @param z_offset z偏移量
     */
    void createRenderBuf(int x_offset, int y_offset, int z_offset);

    /**
     * 区块是否发生变化
     * @return true 改变 false 没有改变
     */
    [[nodiscard]] bool wasChanged() const { return m_wasChanged; }

private:
    /**
         * 获取方块存储位置
         * @param x 0~15
         * @param y 0~15
         * @param z 0~15
         * @return 一维数组的下标
         */
    static int GetBlockIndex(const int x, const int y, const int z)
    {
        return (y << 8) | (z << 4) | x;
    }

private:
    bool m_wasChanged = false;                               // 区块是否发生改变
    std::array<std::uint64_t, 16 * 16 * 16> m_blockIDs = {}; // 存储方块的唯一id
    std::vector<mymc::RenderCommand> m_renderCommands3D;     // 渲染队列
    std::vector<std::shared_ptr<mymc::Mesh>> m_meshes;       // 网格缓存
};
} // server

#endif //MINECRAFT_CPP_CHUNKSECTIONS_H
