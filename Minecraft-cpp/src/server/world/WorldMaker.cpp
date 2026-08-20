#include "WorldMaker.h"

#include <algorithm>
#include <cmath>
#include <array>
#include <unordered_map>

namespace
{
struct Pos2D
{
    std::int64_t x, z;

    bool operator==(const Pos2D &other) const
    {
        return x == other.x && z == other.z;
    }

    Pos2D operator<<(const Pos2D &other) const
    {
        return {
            .x = x + other.x * 16,
            .z = z + other.z * 16
            };
    }
};

// 算两点之间距离的平方（避免开根号）
std::int64_t distSquared(const Pos2D &a, const Pos2D &b)
{
    std::int64_t dx = a.x - b.x;
    std::int64_t dz = a.z - b.z;
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
    corners[Pos2D{.x = 0, .z = 0}] = height(m_rendom);
    corners[Pos2D{.x = 0, .z = 15}] = height(m_rendom);
    corners[Pos2D{.x = 15, .z = 0}] = height(m_rendom);
    corners[Pos2D{.x = 15, .z = 15}] = height(m_rendom);

    int minheight = 256;
    for (const auto &[key, h] : corners)
    {
        if (h < minheight)
        {
            minheight = h;
        }
    }

    chunk.fillBlocks(0, 0, 0, 15, minheight - 6, 15, 1);

    for (int64_t x = 0; x < 16; ++x)
    {
        for (int64_t z = 0; z < 16; ++z)
        {
            Pos2D current{.x = x, .z = z};

            // 反距离加权插值（IDW）
            float weightSum = 0.0f;
            float heightSum = 0.0f;

            for (const auto &[corner, h]: corners)
            {
                const int64_t d2 = distSquared(current, corner);
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

            const auto blockHeight = static_cast<int64_t>(heightSum / weightSum);

            for (int64_t h = minheight - 6; h < blockHeight - 6; ++h)
            {
                chunk.setBlock(x, h, z, 1);
            }
            for (int64_t h = blockHeight - 6; h < blockHeight; ++h)
            {
                chunk.setBlock(x, h, z, 3);
            }
            chunk.setBlock(x, blockHeight, z, 2);
        }
    }
}

// 单次 Simplex 2D 噪声，返回 [-1, 1]
double WorldMaker::simplexNoise2D(double x, double y) const
{
    constexpr double F2 = 0.366025403; // (sqrt(3) - 1) / 2
    constexpr double G2 = 0.211324865; // (3 - sqrt(3)) / 6

    // 梯度向量（12个方向）
    static constexpr int grad2[12][2] = {
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

    // 歪斜输入空间，确定所在的单纯形网格
    const double s = (x + y) * F2;
    const auto i = static_cast<int64_t>(std::floor(x + s));
    const auto j = static_cast<int64_t>(std::floor(y + s));

    // 反歪斜回真实坐标
    const double t = static_cast<double>(i + j) * G2;
    const double x0 = x - static_cast<double>(i) + t;
    const double y0 = y - static_cast<double>(j) + t;

    // 判断在上三角还是下三角
    int64_t i1, j1;
    if (x0 > y0)
    {
        i1 = 1;
        j1 = 0; // 上三角
    }
    else
    {
        i1 = 0;
        j1 = 1; // 下三角
    }

    // 三个角的偏移
    const double x1 = x0 - static_cast<double>(i1) + G2;
    const double y1 = y0 - static_cast<double>(j1) + G2;
    const double x2 = x0 - 1.0 + 2.0 * G2;
    const double y2 = y0 - 1.0 + 2.0 * G2;

    // 哈希索引（0-255）
    const int64_t ii = ((i % 256) + 256) % 256;
    const int64_t jj = ((j % 256) + 256) % 256;

    // 每个角的梯度索引
    const int64_t gi0 = m_permutation[ii + m_permutation[jj]] % 12;
    const int64_t gi1 = m_permutation[ii + i1 + m_permutation[jj + j1]] % 12;
    const int64_t gi2 = m_permutation[ii + 1 + m_permutation[jj + 1]] % 12;

    // 计算每个角的贡献
    auto cornerContrib = [&](double cx, double cy, int64_t gi) -> double
    {
        double t2 = 0.5 - cx * cx - cy * cy;
        if (t2 < 0.0)
            return 0.0;
        t2 *= t2;
        return t2 * t2 * (grad2[gi][0] * cx + grad2[gi][1] * cy);
    };

    const double n0 = cornerContrib(x0, y0, gi0);
    const double n1 = cornerContrib(x1, y1, gi1);
    const double n2 = cornerContrib(x2, y2, gi2);

    // 缩放到 [-1, 1]
    return 70.0 * (n0 + n1 + n2);
}

// 分形布朗运动（多频叠加），返回 [-1, 1]
double WorldMaker::fbm(double x, double y, int octaves, double lacunarity, double gain) const
{
    double sum = 0.0;
    double amp = 1.0;
    double freq = 1.0;
    double maxAmp = 0.0;

    for (int o = 0; o < octaves; ++o)
    {
        sum += amp * simplexNoise2D(x * freq, y * freq);
        maxAmp += amp;
        amp *= gain;
        freq *= lacunarity;
    }

    return sum / maxAmp; // 归一化到 [-1, 1]
}

void WorldMaker::simplexNoise2D(Chunk &chunk) const
{
    const auto [cx, cz] = chunk.getChunkPos();
    const double worldX = static_cast<double>(cx) * 16.0;
    const double worldZ = static_cast<double>(cz) * 16.0;

    // 噪声参数
    constexpr double noiseScale = 0.02;
    constexpr int octaves = 4;
    constexpr double minHeight = 30.0;
    constexpr double maxHeight = 60.0;
    constexpr double baseHeight = (minHeight + maxHeight) / 2.0;
    constexpr double heightRange = (maxHeight - minHeight) / 2.0;

    // 单次遍历：计算高度 + 记录最低点
    int heights[16][16];
    int localMin = static_cast<int>(maxHeight);

    for (int lx = 0; lx < 16; ++lx)
    {
        for (int lz = 0; lz < 16; ++lz)
        {
            const double px = (worldX + static_cast<double>(lx)) * noiseScale;
            const double pz = (worldZ + static_cast<double>(lz)) * noiseScale;

            const double noise = fbm(px, pz, octaves);
            const int h = static_cast<int>(baseHeight + noise * heightRange);
            heights[lx][lz] = h;

            if (h < localMin)
                localMin = h;
        }
    }

    // 填充方块
    const int baseY = std::max(0, localMin - 6);
    chunk.fillBlocks(0, 0, 0, 15, baseY, 15, 1); // 石头基底

    for (int lx = 0; lx < 16; ++lx)
    {
        for (int lz = 0; lz < 16; ++lz)
        {
            const int blockHeight = heights[lx][lz];

            // 泥土层
            for (int h = baseY; h < blockHeight - 1; ++h)
            {
                chunk.setBlock(lx, h, lz, 3);
            }
            // 草方块表层
            chunk.setBlock(lx, blockHeight - 1, lz, 2);
        }
    }
}
} // server
