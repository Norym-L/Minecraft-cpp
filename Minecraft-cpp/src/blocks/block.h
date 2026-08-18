#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "render/Material.h"
#include "render/Mesh.h"

namespace block
{
/// @brief 贴图数组索引约定（与 CreateBox 面顺序一致）
enum FaceTex : int
{
    FRONT = 0,  ///< +Z (正面)
    TOP = 1,    ///< +Y (顶面)
    RIGHT = 2,  ///< +X (右面)
    LEFT = 3,   ///< -X (左面)
    BOTTOM = 4, ///< -Y (底面)
    BACK = 5,   ///< -Z (后面)
};

class Block
{
public:
    friend class BlockManager;

public:
    /// @brief 构造方块单列实体
    explicit Block(
        std::uint64_t id,
        std::string name,
        const std::shared_ptr<mymc::Material> &material,
        const std::array<std::shared_ptr<mymc::Texture>, 6> &texture);

    [[nodiscard]] std::shared_ptr<mymc::Material> material() const { return m_material; }

    /// @brief 获取六面贴图数组
    [[nodiscard]] const std::array<std::shared_ptr<mymc::Texture>, 6> &textures() const { return m_textures; }

private:
    std::uint64_t m_id;                                            // id
    std::string m_name;                                            // 名称
    std::shared_ptr<mymc::Material> m_material;                    // 方块材质
    std::array<std::shared_ptr<mymc::Texture>, 6> m_textures = {}; // 六面贴图
};
}
