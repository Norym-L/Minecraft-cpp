#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "render/Material.h"
#include "render/Mesh.h"

namespace block
{
class Block
{
public:
    // 禁止拷贝，允许移动
    // Block(const Block &) = delete;
    //
    // Block &operator=(const Block &) = delete;
    //
    // Block(Block &&) = default;
    //
    // Block &operator=(Block &&) = default;

    friend class BlockManager;

public: // TODO 方块注册
    // explicit Block(std::string &name);

    explicit Block(
        std::string                            name,
        const std::shared_ptr<mymc::Mesh> &    mesh,
        const std::shared_ptr<mymc::Material> &material,
        const std::shared_ptr<mymc::Texture> & texture);

    // explicit Block(
    //         std::string &name,
    //         std::shared_ptr<mymc::Mesh> mesh,
    //         std::array<std::shared_ptr<mymc::Texture>, 6> textures,
    //         std::shared_ptr<mymc::Texture> texture);

    // explicit Block(
    //         std::string &name,
    //         std::shared_ptr<mymc::Mesh> mesh,
    //         std::shared_ptr<mymc::Material> material,
    //         std::shared_ptr<mymc::Texture> texture,
    //         std::function<void(float)> update);

    // explicit Block(
    //         std::string &name,
    //         std::shared_ptr<mymc::Mesh> mesh,
    //         std::shared_ptr<mymc::Material> material,
    //         std::shared_ptr<mymc::Texture> texture,
    //         std::function<void(float)> update);
    [[nodiscard]] std::shared_ptr<mymc::Mesh>                   mesh() const { return m_mesh; }
    [[nodiscard]] std::shared_ptr<mymc::Material>               material() const { return m_material; }
    [[nodiscard]] std::array<std::shared_ptr<mymc::Texture>, 6> textures() const { return m_textures; }

private:
    std::uint64_t                                 m_id = 0;        // 唯一id
    std::string                                   m_name;          // 名称
    std::shared_ptr<mymc::Mesh>                   m_mesh;          // 方块网格
    std::shared_ptr<mymc::Material>               m_material;      // 方块材质
    std::array<std::shared_ptr<mymc::Texture>, 6> m_textures = {}; // 方块贴图
    std::function<void(float)>                    update     = {}; // 更新函数
};
}
