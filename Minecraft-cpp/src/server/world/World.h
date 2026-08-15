#ifndef MINECRAFT_CPP_WORLD_H
#define MINECRAFT_CPP_WORLD_H
#include <unordered_map>

#include "server/chunk/Chunk.h"
#include "Common.h"

namespace server
{
    class World
    {
    public:
        void init();

        void render() const;

    private:
        std::unordered_map<mymc::ChunkPos, std::unique_ptr<Chunk> > m_worldMap;
    };
} // server

#endif //MINECRAFT_CPP_WORLD_H
