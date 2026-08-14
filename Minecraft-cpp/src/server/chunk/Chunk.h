#ifndef MINECRAFT_CPP_CHUNK_H
#define MINECRAFT_CPP_CHUNK_H
#include <vector>

#include "ChunkSections.h"

namespace server
{
    class Chunk // TODO 区块加载和世界生成
    {
    private:
        std::vector<ChunkSections> m_sectionses; // 区块垂直切片
    };
} // server

#endif //MINECRAFT_CPP_CHUNK_H
