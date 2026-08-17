#include "ChunkSections.h"

#include <algorithm>

#include "core/Core.h"
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

    // 返回需要被渲染的面 x- x+ y- y+ z- z+
    auto needToBeRender = [this](const int x, const int y, const int z) -> std::array<bool, 6>
        {
        std::array<bool, 6> result = {};

        if (x == 0 || getBlock(x - 1, y, z) == 0) result[0] = true;
        if (x == 15 || getBlock(x + 1, y, z) == 0) result[1] = true;
        if (y == 0 || getBlock(x, y - 1, z) == 0) result[2] = true;
        if (y == 15 || getBlock(x, y + 1, z) == 0) result[3] = true;
        if (z == 0 || getBlock(x, y, z - 1) == 0) result[4] = true;
        if (z == 15 || getBlock(x, y, z + 1) == 0) result[5] = true;
        return result;
        };

    for (int index = 0; index < m_blockIDs.size(); ++index)
    {
        const int x = index & 0xF;         // 取低4位 (0-15)
        const int z = (index >> 4) & 0xF;  // 取中间4位 (0-15)
        const int y = (index >> 8) & 0xFF; // 取高8位 (0-255)

        const auto block_id = getBlock(x, y, z);
        if (block_id == 0)
        {
            continue;
        }
        auto block = mymc::Core::GetInstance().getBlockManager().getBlock(block_id);

        auto render_face = needToBeRender(x, y, z);

        // 去除无需渲染
        if (std::ranges::all_of(
            render_face,
            [](const bool flg) { return !flg; }))
        {
            continue;
        }

        // TODO 把相同方块的网格合成，材质合成

        auto mesh = mymc::Mesh::CreateBlockByFace(render_face);
        m_meshes.push_back(mesh); // 防止空悬指针

        mymc::RenderCommand command = {
            .mesh = mesh.get(),
            .material = block.material().get(),
            .model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_offset + x, y_offset + y, z_offset + z))
            };

        m_renderCommands3D.push_back(command);
    }
}
} // server
