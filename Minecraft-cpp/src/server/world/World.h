#ifndef MINECRAFT_CPP_WORLD_H
#define MINECRAFT_CPP_WORLD_H
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "server/chunk/Chunk.h"
#include "Common.h"
#include "WorldMaker.h"

namespace utility
{
class TaskPool;
}

namespace server
{
class World
{
public:
    /// @brief 射线检测命中结果
    struct RaycastHit
    {
        int blockX = 0, blockY = 0, blockZ = 0;  // 被命中方块的世界坐标
        int faceX = 0, faceY = 0, faceZ = 0;     // 命中面的法线方向（用于放置到相邻格）
        std::uint64_t blockId = 0;               // 被命中方块的注册 id
    };

    void init();

    /**
     * 请求阶段（主线程每帧调用）：轻量决策——扫描缺失/过远区块，提交生成与网格构建任务到线程池。
     */
    void update();

    void render();

    /// 设置任务池（由 Core 注入；nullptr 表示不使用异步构建）
    void setTaskPool(utility::TaskPool *pool) { m_taskPool = pool; }

    /**
     * DDA 体素射线遍历：从 origin 沿 dir 前进，返回首个非空气方块。
     * @param origin 射线起点（世界坐标）
     * @param dir    射线方向（世界坐标，未归一化亦可）
     * @param max_dist 最大探测距离（格）
     * @param hit    命中结果
     * @return true 命中；false 未命中
     */
    bool raycastBlock(const glm::vec3 &origin, const glm::vec3 &dir, float max_dist, RaycastHit &hit) const;

    /**
     * 读取某格方块 id（线程安全）。
     * @return 方块 id；区块不存在或越界返回 0（空气）
     */
    std::uint64_t getBlockAt(int wx, int wy, int wz) const;

    /**
     * 在世界坐标处写入方块（线程安全），并提交该区块（及受影响的相邻区块）重建任务。
     * @return 是否写入成功（区块存在且坐标合法）
     */
    bool setBlockAt(int wx, int wy, int wz, std::uint64_t block_id);

private:
    /// 在任务池工作线程中执行：地形生成 + 网格构建（CPU），完成后置 m_uploadRequested。
    void generateAndBuild(const std::shared_ptr<Chunk> &chunk);

    /// 提交某区块的"仅重建网格"任务（不重新生成地形）。
    void scheduleRebuild(const std::shared_ptr<Chunk> &chunk);

private:
    mutable std::mutex m_mapMutex;
    std::unordered_map<mymc::ChunkPos, std::shared_ptr<Chunk> > m_worldMap;
    WorldMaker m_worldMaker;
    utility::TaskPool *m_taskPool = nullptr;
};
} // server

#endif //MINECRAFT_CPP_WORLD_H
