#ifndef MINECRAFT_CPP_WORLD_H
#define MINECRAFT_CPP_WORLD_H
#include <unordered_map>

#include "server/chunk/Chunk.h"
#include "Common.h"
#include "WorldMaker.h"

namespace server
{
class World
{
public:
    void init();

    void update();

    void render();

private:
    std::mutex m_mapMutex;
    std::unordered_map<mymc::ChunkPos, std::unique_ptr<Chunk> > m_worldMap;
    WorldMaker m_worldMaker;
};
} // server

#endif //MINECRAFT_CPP_WORLD_H
