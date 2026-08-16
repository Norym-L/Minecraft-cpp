#include "BlockManager.h"

#include <ranges>

#include "core/Core.h"

namespace block
{
BlockManager::BlockManager() = default;

void BlockManager::init() // TODO 完善注册结构
{
    registerBlock("Air", nullptr, nullptr, nullptr);

    const auto mesh     = mymc::Mesh::CreateBox(glm::vec3(1.0f));
    const auto material = std::make_shared<mymc::Material>();
    material->setShaderProgram(mymc::Core::GetInstance().getGraphicsAPI().getDefaultShaderProgram());

    registerBlock("testBlovc", mesh, material, nullptr);
}

BlockManager &BlockManager::GetInstance()
{
    static BlockManager instance;
    return instance;
}

void BlockManager::update(float delta_time)
{
    for (auto &val: m_blockMap | std::views::values)
    {
        const auto block = val.get();
        if (!val->m_mesh)
        {
            continue;
        }
        //TODO 删除渲染逻辑
        mymc::RenderCommand command = {
            .mesh         = block->m_mesh.get(),
            .material     = block->m_material.get(),
            .model_matrix = glm::mat4(1.0f)
        };
        mymc::Core::GetInstance().getRanderQueue().submit(command);
    }
}

void BlockManager::registerBlock(std::unique_ptr<Block> block)
{
    m_blockMap[m_next_id] = std::move(block);
    ++m_next_id;
}

const Block &BlockManager::getBlock(const std::uint64_t block_id) const
{
    const auto it = m_blockMap.find(block_id);
    if (it == m_blockMap.end())
    {
        throw std::out_of_range("Block ID not found");
    }
    return *it->second;
}
} // block
