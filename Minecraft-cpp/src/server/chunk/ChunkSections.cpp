#include "ChunkSections.h"

#include <algorithm>
#include <array>
#include <unordered_map>

#include "core/Core.h"
#include "render/MeshBuilder.h"
#include "render/RenderQueue.h"

namespace server
{
void ChunkSections::fill(const std::uint64_t block_id)
{
    m_blockIDs.fill(block_id);
    m_wasChanged = true;
}

void ChunkSections::createRenderBuf(const int x_offset, const int y_offset, const int z_offset)
{
    if (!m_wasChanged)
    {
        return;
    }

    m_wasChanged = false;
    m_renderCommands3D.clear();
    m_meshes.clear();

    // 按方块ID分组收集顶点
    std::unordered_map<std::uint64_t, mymc::MeshBuilder> builders;

    // Greedy Meshing: 对6个方向分别扫描16×16层

    // ===== Y+ 方向（顶面）=====
    for (int y = 0; y < 16; ++y)
    {
        std::array<std::array<bool, 16>, 16> visited = {};

        for (int z = 0; z < 16; ++z)
        {
            for (int x = 0; x < 16; ++x)
            {
                if (visited[x][z]) continue;

                const auto block_id = getBlock(x, y, z);
                if (block_id == 0) continue;

                bool needRender = (y == 15) || (getBlock(x, y + 1, z) == 0);
                if (!needRender) continue;

                // 向右扩展宽度
                int width = 1;
                while (x + width < 16)
                {
                    if (getBlock(x + width, y, z) != block_id) break;
                    if ((y != 15 && getBlock(x + width, y + 1, z) != 0) || visited[x + width][z]) break;
                    width++;
                }

                // 向前扩展深度
                int depth = 1;
                while (z + depth < 16)
                {
                    bool canExtend = true;
                    for (int dx = 0; dx < width; ++dx)
                    {
                        if (getBlock(x + dx, y, z + depth) != block_id) { canExtend = false; break; }
                        if ((y != 15 && getBlock(x + dx, y + 1, z + depth) != 0) || visited[x + dx][z + depth]) { canExtend = false; break; }
                    }
                    if (!canExtend) break;
                    depth++;
                }

                for (int dz = 0; dz < depth; ++dz)
                    for (int dx = 0; dx < width; ++dx)
                        visited[x + dx][z + dz] = true;

                builders[block_id].addGreedyQuad(x, y, z, width, depth, mymc::Face::Y_PLUS);
            }
        }
    }

    // ===== Y- 方向（底面）=====
    for (int y = 0; y < 16; ++y)
    {
        std::array<std::array<bool, 16>, 16> visited = {};

        for (int z = 0; z < 16; ++z)
        {
            for (int x = 0; x < 16; ++x)
            {
                if (visited[x][z]) continue;

                const auto block_id = getBlock(x, y, z);
                if (block_id == 0) continue;

                bool needRender = (y == 0) || (getBlock(x, y - 1, z) == 0);
                if (!needRender) continue;

                int width = 1;
                while (x + width < 16)
                {
                    if (getBlock(x + width, y, z) != block_id) break;
                    if ((y != 0 && getBlock(x + width, y - 1, z) != 0) || visited[x + width][z]) break;
                    width++;
                }

                int depth = 1;
                while (z + depth < 16)
                {
                    bool canExtend = true;
                    for (int dx = 0; dx < width; ++dx)
                    {
                        if (getBlock(x + dx, y, z + depth) != block_id) { canExtend = false; break; }
                        if ((y != 0 && getBlock(x + dx, y - 1, z + depth) != 0) || visited[x + dx][z + depth]) { canExtend = false; break; }
                    }
                    if (!canExtend) break;
                    depth++;
                }

                for (int dz = 0; dz < depth; ++dz)
                    for (int dx = 0; dx < width; ++dx)
                        visited[x + dx][z + dz] = true;

                builders[block_id].addGreedyQuad(x, y, z, width, depth, mymc::Face::Y_MINUS);
            }
        }
    }

    // ===== Z- 方向（前面 z-）=====
    for (int z = 0; z < 16; ++z)
    {
        std::array<std::array<bool, 16>, 16> visited = {};

        for (int y = 0; y < 16; ++y)
        {
            for (int x = 0; x < 16; ++x)
            {
                if (visited[x][y]) continue;

                const auto block_id = getBlock(x, y, z);
                if (block_id == 0) continue;

                bool needRender = (z == 0) || (getBlock(x, y, z - 1) == 0);
                if (!needRender) continue;

                int width = 1;
                while (x + width < 16)
                {
                    if (getBlock(x + width, y, z) != block_id) break;
                    if ((z != 0 && getBlock(x + width, y, z - 1) != 0) || visited[x + width][y]) break;
                    width++;
                }

                int height = 1;
                while (y + height < 16)
                {
                    bool canExtend = true;
                    for (int dx = 0; dx < width; ++dx)
                    {
                        if (getBlock(x + dx, y + height, z) != block_id) { canExtend = false; break; }
                        if ((z != 0 && getBlock(x + dx, y + height, z - 1) != 0) || visited[x + dx][y + height]) { canExtend = false; break; }
                    }
                    if (!canExtend) break;
                    height++;
                }

                for (int dy = 0; dy < height; ++dy)
                    for (int dx = 0; dx < width; ++dx)
                        visited[x + dx][y + dy] = true;

                builders[block_id].addGreedyQuad(x, y, z, width, height, mymc::Face::Z_MINUS);
            }
        }
    }

    // ===== Z+ 方向（后面 z+）=====
    for (int z = 0; z < 16; ++z)
    {
        std::array<std::array<bool, 16>, 16> visited = {};

        for (int y = 0; y < 16; ++y)
        {
            for (int x = 0; x < 16; ++x)
            {
                if (visited[x][y]) continue;

                const auto block_id = getBlock(x, y, z);
                if (block_id == 0) continue;

                bool needRender = (z == 15) || (getBlock(x, y, z + 1) == 0);
                if (!needRender) continue;

                int width = 1;
                while (x + width < 16)
                {
                    if (getBlock(x + width, y, z) != block_id) break;
                    if ((z != 15 && getBlock(x + width, y, z + 1) != 0) || visited[x + width][y]) break;
                    width++;
                }

                int height = 1;
                while (y + height < 16)
                {
                    bool canExtend = true;
                    for (int dx = 0; dx < width; ++dx)
                    {
                        if (getBlock(x + dx, y + height, z) != block_id) { canExtend = false; break; }
                        if ((z != 15 && getBlock(x + dx, y + height, z + 1) != 0) || visited[x + dx][y + height]) { canExtend = false; break; }
                    }
                    if (!canExtend) break;
                    height++;
                }

                for (int dy = 0; dy < height; ++dy)
                    for (int dx = 0; dx < width; ++dx)
                        visited[x + dx][y + dy] = true;

                builders[block_id].addGreedyQuad(x, y, z, width, height, mymc::Face::Z_PLUS);
            }
        }
    }

    // ===== X+ 方向（右面 x+）=====
    for (int x = 0; x < 16; ++x)
    {
        std::array<std::array<bool, 16>, 16> visited = {};

        for (int z = 0; z < 16; ++z)
        {
            for (int y = 0; y < 16; ++y)
            {
                if (visited[y][z]) continue;

                const auto block_id = getBlock(x, y, z);
                if (block_id == 0) continue;

                bool needRender = (x == 15) || (getBlock(x + 1, y, z) == 0);
                if (!needRender) continue;

                int width = 1;
                while (y + width < 16)
                {
                    if (getBlock(x, y + width, z) != block_id) break;
                    if ((x != 15 && getBlock(x + 1, y + width, z) != 0) || visited[y + width][z]) break;
                    width++;
                }

                int height = 1;
                while (z + height < 16)
                {
                    bool canExtend = true;
                    for (int dy = 0; dy < width; ++dy)
                    {
                        if (getBlock(x, y + dy, z + height) != block_id) { canExtend = false; break; }
                        if ((x != 15 && getBlock(x + 1, y + dy, z + height) != 0) || visited[y + dy][z + height]) { canExtend = false; break; }
                    }
                    if (!canExtend) break;
                    height++;
                }

                for (int dz = 0; dz < height; ++dz)
                    for (int dy = 0; dy < width; ++dy)
                        visited[y + dy][z + dz] = true;

                // X+/X- 面：width沿Y轴，height沿Z轴；addGreedyQuad中w沿Z轴，h沿Y轴，需交换
                builders[block_id].addGreedyQuad(x, y, z, height, width, mymc::Face::X_PLUS);
            }
        }
    }

    // ===== X- 方向（左面 x-）=====
    for (int x = 0; x < 16; ++x)
    {
        std::array<std::array<bool, 16>, 16> visited = {};

        for (int z = 0; z < 16; ++z)
        {
            for (int y = 0; y < 16; ++y)
            {
                if (visited[y][z]) continue;

                const auto block_id = getBlock(x, y, z);
                if (block_id == 0) continue;

                bool needRender = (x == 0) || (getBlock(x - 1, y, z) == 0);
                if (!needRender) continue;

                int width = 1;
                while (y + width < 16)
                {
                    if (getBlock(x, y + width, z) != block_id) break;
                    if ((x != 0 && getBlock(x - 1, y + width, z) != 0) || visited[y + width][z]) break;
                    width++;
                }

                int height = 1;
                while (z + height < 16)
                {
                    bool canExtend = true;
                    for (int dy = 0; dy < width; ++dy)
                    {
                        if (getBlock(x, y + dy, z + height) != block_id) { canExtend = false; break; }
                        if ((x != 0 && getBlock(x - 1, y + dy, z + height) != 0) || visited[y + dy][z + height]) { canExtend = false; break; }
                    }
                    if (!canExtend) break;
                    height++;
                }

                for (int dz = 0; dz < height; ++dz)
                    for (int dy = 0; dy < width; ++dy)
                        visited[y + dy][z + dz] = true;

                // X+/X- 面：width沿Y轴，height沿Z轴；addGreedyQuad中w沿Z轴，h沿Y轴，需交换
                builders[block_id].addGreedyQuad(x, y, z, height, width, mymc::Face::X_MINUS);
            }
        }
    }

    // 构建每个方块ID的合并网格
    for (auto& [block_id, builder] : builders)
    {
        auto mergedMesh = builder.build();
        if (!mergedMesh)
        {
            continue;
        }
        m_meshes.push_back(mergedMesh);

        mymc::RenderCommand command = {
            .mesh = mergedMesh.get(),
            .material = mymc::Core::GetInstance().getBlockManager().getBlock(block_id).material().get(),
            .model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_offset, y_offset, z_offset))
        };

        m_renderCommands3D.push_back(command);
    }
}

} // namespace server
