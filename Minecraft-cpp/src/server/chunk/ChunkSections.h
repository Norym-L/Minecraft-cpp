// 区块的垂直切片 16*16*16
#ifndef MINECRAFT_CPP_CHUNKSECTIONS_H
#define MINECRAFT_CPP_CHUNKSECTIONS_H
#include <cstdint>
#include <array>

namespace server
{
    class ChunkSections
    {
    public:
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
        }

        /**
         * 获取方块注册id
         * @param x 0~15
         * @param y 0~15
         * @param z 0~15
         * @return 方块的注册id
         */
        [[nodiscard]] std::uint64_t getBlock(int x, int y, int z) const
        {
            return m_blockIDs[GetBlockIndex(x, y, z)];
        }

        /**
         * 完全填充
         * @param block_id 方块的注册id
         */
        void fill(std::uint64_t block_id);

        /**
         * TODO 临时渲染
         */
        void render(int x_offset, int y_offset, int z_offset) const;

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
        std::array<std::uint64_t, 16 * 16 * 16> m_blockIDs = {}; // 存储方块的唯一id
    };
} // server

#endif //MINECRAFT_CPP_CHUNKSECTIONS_H
