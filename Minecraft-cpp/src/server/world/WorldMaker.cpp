#include "WorldMaker.h"

#include <cmath>
#include <array>
#include <ranges>
#include <unordered_map>

namespace
{
struct Pos2D
{
    int x, z;

    bool operator==(const Pos2D &other) const
    {
        return x == other.x && z == other.z;
    }
};

// 算两点之间距离的平方（避免开根号）
int distSquared(const Pos2D &a, const Pos2D &b)
{
    int dx = a.x - b.x;
    int dz = a.z - b.z;
    return dx * dx + dz * dz;
}
}

template<>
struct std::hash<Pos2D>
{
    size_t operator()(const Pos2D &pos) const noexcept
    {
        const size_t h1 = std::hash<int>{}(pos.x);
        const size_t h2 = std::hash<int>{}(pos.z);
        return h1 ^ (h2 << 1);
    }
};


namespace server
{
WorldMaker::WorldMaker(const std::uint64_t seed)
    : m_rendom(seed), m_seed(seed)
{}

void WorldMaker::valueNoise(Chunk &chunk)
{
    std::uniform_int_distribution<int> height(30, 50);

    // 四个角的随机高度
    std::unordered_map<Pos2D, int> corners;
    corners[Pos2D{.x = 0, .z = 0}]   = height(m_rendom);
    corners[Pos2D{.x = 0, .z = 15}]  = height(m_rendom);
    corners[Pos2D{.x = 15, .z = 0}]  = height(m_rendom);
    corners[Pos2D{.x = 15, .z = 15}] = height(m_rendom);

    int minheight = 256;
    for (const auto &[key, h] : corners)
    {
        if (h < minheight)
        {
            minheight = h;
        }
    }

    chunk.fillBlocks(0 ,0 ,0, 15, minheight - 6, 15, 1);

    for (int x = 0; x < 16; ++x)
    {
        for (int z = 0; z < 16; ++z)
        {
            Pos2D current{.x = x, .z = z};

            // 反距离加权插值（IDW）
            float weightSum = 0.0f;
            float heightSum = 0.0f;

            for (const auto &[corner, h] : corners)
            {
                const int d2 = distSquared(current, corner);
                if (d2 == 0)
                {
                    // 恰好在角上，直接用该高度
                    heightSum = static_cast<float>(h);
                    weightSum = 1.0f;
                    break;
                }
                const float w = 1.0f / static_cast<float>(d2);
                weightSum += w;
                heightSum += w * static_cast<float>(h);
            }

            const int blockHeight = static_cast<int>(heightSum / weightSum);

            for (int h = minheight - 6; h < blockHeight - 6; ++h)
            {
                chunk.setBlock(x, h, z, 1);
            }
            for (int h = blockHeight - 6; h < blockHeight; ++h)
            {
                chunk.setBlock(x, h, z, 3);
            }
            chunk.setBlock(x, blockHeight, z, 2);
        }
    }
}
} // server
