#ifndef MINECRAFT_CPP_CHUNK_H
#define MINECRAFT_CPP_CHUNK_H
#include <atomic>
#include <memory>

#include "ChunkSections.h"
#include "Common.h"

namespace server
{
class Chunk
{
public:
    friend class World;

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
        m_wasChanged = true;
        m_sections[y / 16].setBlock(x, y % 16, z, block_id);
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
     * 构建区块网格的纯 CPU 顶点数据（合并所有小节，chunk 内部去重）。
     * 可在任意线程执行（不做任何 GL 调用）；完成后置 m_uploadRequested。
     */
    void buildMeshData();

    /**
     * 把待上传的 CPU 顶点数据升级为 GL 网格并更新渲染命令。
     * 必须在 OpenGL 线程调用。
     */
    void uploadMesh();

private:
    mymc::ChunkPos m_chunkPos = {};                                  // 区块的坐标
    std::array<ChunkSections, 16> m_sections;                        // 区块垂直切片
    std::atomic<bool> m_wasChanged{false};                           // 区块是否发生改变（方块数据层面）
    std::atomic<bool> m_buildInFlight{false};                        // 是否有生成/重建任务在飞行中
    std::shared_ptr<mymc::MeshData> m_meshData;                      // 待上传的 CPU 顶点数据（构建线程产出）
    std::atomic<bool> m_uploadRequested{false};                      // 有新 MeshData 待 GL 上传
    std::shared_ptr<mymc::Mesh> m_mesh;                              // GL 网格（GL 线程持有）
    std::vector<mymc::RenderCommand> m_renderCommands3D;             // 渲染命令缓存（GL 线程维护）
};
} // server

#endif //MINECRAFT_CPP_CHUNK_H
