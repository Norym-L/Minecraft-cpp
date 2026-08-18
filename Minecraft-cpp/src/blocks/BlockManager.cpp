#include "BlockManager.h"

#include <ranges>

#include "core/Core.h"

namespace block
{
BlockManager::BlockManager() = default;

void BlockManager::init() // TODO 完善注册结构
{
    registerBlock(0, "Air", nullptr, nullptr, nullptr);

    const auto mesh = mymc::Mesh::CreateBox();

    const auto texture = mymc::Texture::Load("mymc/block/stone.png");
    const auto material = std::make_shared<mymc::Material>();
    material->setShaderProgram(mymc::Core::GetInstance().getGraphicsAPI().getDefaultShaderProgram());
    material->setParam("stone", texture);

    registerBlock(1, "stone", mesh, material, texture);
}

BlockManager &BlockManager::GetInstance()
{
    static BlockManager instance;
    return instance;
}

void BlockManager::registerBlock(std::unique_ptr<Block> block)
{
    m_blockMap[block->m_id] = std::move(block);
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
