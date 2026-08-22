#ifndef MINECRAFT_CPP_WORLD_H
#define MINECRAFT_CPP_WORLD_H
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
    void init();

    /**
     * 请求阶段（主线程每帧调用）：轻量决策——扫描缺失/过远区块，提交生成与网格构建任务到线程池。
     */
    void update();

    void render();

    /// 设置任务池（由 Core 注入；nullptr 表示不使用异步构建）
    void setTaskPool(utility::TaskPool *pool) { m_taskPool = pool; }

private:
    /// 在任务池工作线程中执行：地形生成 + 网格构建（CPU），完成后置 m_uploadRequested。
    void generateAndBuild(const std::shared_ptr<Chunk> &chunk);

private:
    std::mutex m_mapMutex;
    std::unordered_map<mymc::ChunkPos, std::shared_ptr<Chunk> > m_worldMap;
    WorldMaker m_worldMaker;
    utility::TaskPool *m_taskPool = nullptr;
};
} // server

#endif //MINECRAFT_CPP_WORLD_H
