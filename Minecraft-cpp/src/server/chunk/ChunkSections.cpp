//
// Created by Norym on 2026/8/14.
//

#include "ChunkSections.h"

#include "core/Core.h"
#include "render/RenderQueue.h"

namespace server
{
    void ChunkSections::fill(const std::uint64_t block_id)
    {
        m_blockIDs.fill(block_id);
    }

    void ChunkSections::render(const int x_offset, const int y_offset, const int z_offset) const
    {
        for (int index = 0; index < m_blockIDs.size(); ++index)
        {
            const int x = index & 0xF;         // 取低4位 (0-15)
            const int z = (index >> 4) & 0xF;  // 取中间4位 (0-15)
            const int y = (index >> 8) & 0xFF; // 取高8位 (0-255)

            auto block = mymc::Core::GetInstance().getBlockManager().getBlock(getBlock(x, y, z));

            mymc::RenderCommand command = {
                    .mesh = block.mesh().get(),
                    .material = block.material().get(),
                    .model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x_offset + x, y_offset + y, z_offset + z))
                    };
            mymc::Core::GetInstance().getRanderQueue().submit(command);
        }
    }
} // server
