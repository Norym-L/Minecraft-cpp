#include "BlockManager.h"

#include <iostream>
#include <stdexcept>

#include "core/Core.h"
#include "nlohmann/json.hpp"

namespace block
{
BlockManager::BlockManager() = default;

void BlockManager::init() // TODO 完善注册结构
{
    loadFromJson();
}

BlockManager &BlockManager::GetInstance()
{
    static BlockManager instance;
    return instance;
}

void BlockManager::registerBlock(std::unique_ptr<Block> block)
{
    // 若未指定着色器，自动绑定默认 3D 着色器
    if (block->m_material && !block->m_material->getShaderProgram())
    {
        block->m_material->setShaderProgram(mymc::Core::GetInstance().getGraphicsAPI().getDefaultShaderProgram());
    }
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

void BlockManager::loadFromJson(const std::string &path)
{
    std::string json_string = mymc::FileSystem::LoadAssetFileText(path);
    if (json_string.empty())
    {
        std::cerr << "load block_register.json failed \n";
        return;
    }

    nlohmann::json json = nlohmann::json::parse(json_string);
    if (json.empty() || !json.contains("blocks"))
    {
        std::cerr << "load block_register.json failed \n";
        return;
    }

    json = json["blocks"];

    for (const auto &block_json: json)
    {
        if (block_json.empty())
        {
            continue;
        }

        std::uint64_t block_id = block_json["id"];
        std::string name = block_json["name"];
        std::array<std::shared_ptr<mymc::Texture>, 6> block_tex = {};

        if (block_json.contains("texture"))
        {
            if (!block_json.contains("texture"))
            {
                continue;
            }
            auto texture_json = block_json["texture"];

            if (texture_json["type"] == "all_same")
            {
                block_tex.fill(mymc::Texture::Load(texture_json["side"]));
            }
            else if (texture_json["type"] == "different_faces")
            {
                const auto tex_top = mymc::Texture::Load(texture_json["top"]);
                const auto tex_button = mymc::Texture::Load(texture_json["bottom"]);
                const auto tex_side = mymc::Texture::Load(texture_json["side"]);
                block_tex = {
                    tex_top,
                    tex_button,
                    tex_side,
                    tex_side,
                    tex_side,
                    tex_side
                    };
            }
        }

        auto material = std::make_shared<mymc::Material>();

        if (block_id != 0)
        {
            material->setFaceTextures(block_tex);
        }
        else
        {
            material = nullptr;
        }

        registerBlock(block_id,
                      name,
                      material,
                      block_tex);
    }
}
} // block
