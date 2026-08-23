#include "BlockManager.h"

#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "core/Core.h"
#include "graphics/Texture.h"
#include "render/Material.h"
#include "nlohmann/json.hpp"

namespace block
{
BlockManager::BlockManager() = default;

void BlockManager::init() // TODO 完善注册结构
{
    loadFromJson();
    buildAtlas();
}

void BlockManager::buildAtlas()
{
    m_atlas = std::make_shared<mymc::TextureAtlas>();

    // 登记所有方块的面贴图（TextureManager 缓存保证同一路径只占用一个瓦片）
    for (const auto &[id, block]: m_blockMap)
    {
        for (const auto &tex: block->m_textures)
        {
            if (tex)
            {
                m_atlas->addTexture(*tex);
            }
        }
    }

    const auto atlas_texture = m_atlas->build();
    if (!atlas_texture)
    {
        std::cerr << "BlockManager：贴图图集构建失败" << std::endl;
        return;
    }
    m_atlasTexture = atlas_texture; // 供 UI 图标等直接引用

    // 全局方块材质：一张图集 + 默认 3D 着色器
    m_atlasMaterial = std::make_shared<mymc::Material>();
    m_atlasMaterial->setShaderProgram(mymc::Core::GetInstance().getGraphicsAPI().getDefaultShaderProgram());
    m_atlasMaterial->setParam("uAtlas", atlas_texture);

    // 所有非空气方块共用该材质
    for (auto &[id, block]: m_blockMap)
    {
        if (block->m_id != 0)
        {
            block->m_material = m_atlasMaterial;
        }
    }

    // 运行时自检：打印各方块六面 UV，便于核对面贴图映射是否正确
    std::cerr << "[BlockManager] 图集 " << atlas_texture->getWidth() << "x" << atlas_texture->getHeight()
              << " 瓦片数 " << m_atlas->getTileCount() << std::endl;
    for (const auto &[id, block]: m_blockMap)
    {
        if (block->m_id == 0 || !m_atlas)
        {
            continue;
        }
        std::cerr << "  block[" << block->m_id << "] " << block->m_name << " UV:";
        for (int i = 0; i < 6; ++i)
        {
            const mymc::UVRect r = m_atlas->getUV(block->m_textures[i].get());
            std::cerr << " {" << r.min.x << "," << r.min.y << " x" << r.size.x << "," << r.size.y << "}";
        }
        std::cerr << std::endl;
    }
}

int BlockManager::faceToTextureIndex(const mymc::Face face)
{
    // 与历史 Material::setFaceTextures + 着色器按法线选面的语义保持一致：
    // [0]=顶(+Y) [1]=底(-Y) [2]=前(+Z) [3]=后(-Z) [4]=右(+X) [5]=左(-X)
    switch (face)
    {
    case mymc::Face::Y_PLUS:  return 0;
    case mymc::Face::Y_MINUS: return 1;
    case mymc::Face::Z_PLUS:  return 2;
    case mymc::Face::Z_MINUS: return 3;
    case mymc::Face::X_PLUS:  return 4;
    case mymc::Face::X_MINUS: return 5;
    }
    return 0;
}

mymc::UVRect BlockManager::getAtlasUV(const std::uint64_t block_id, const mymc::Face face) const
{
    if (!m_atlas)
    {
        return mymc::UVRect{};
    }

    const auto &block = getBlock(block_id);
    const auto *tex = block.m_textures[faceToTextureIndex(face)].get();
    return tex ? m_atlas->getUV(tex) : mymc::UVRect{};
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
