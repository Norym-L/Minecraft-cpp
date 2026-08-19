#ifndef MINECRAFT_CPP_WORLDMAKER_H
#define MINECRAFT_CPP_WORLDMAKER_H
#include <random>

#include "server/chunk/Chunk.h"

namespace server
{
class WorldMaker
{
public:
    WorldMaker(std::uint64_t seed = 100ULL);

    /**
     * 值噪声算法
     * @param chunk 区块
     */
    void valueNoise(Chunk& chunk);

private:
    std::mt19937_64 m_rendom;
    std::uint64_t m_seed = 100ULL;
};
} // server

#endif //MINECRAFT_CPP_WORLDMAKER_H
