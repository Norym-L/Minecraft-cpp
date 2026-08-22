/**
 * @file TextureAtlas.h
 * @brief 贴图图集：多张小贴图打包进一张大图，供方块等固定精灵使用。
 *
 * 布局约定（与 greedy meshing 兼容的关键）：
 *   每种贴图独占一个「超级块」：大小为 (repeat * tile_size) x (repeat * tile_size)，
 *   内部是 repeat x repeat 个贴图拷贝的网格。一个贪心大面（最多 16x16 方块）的 UV
 *   沿 x/y 各方向每方块只前进 1 个贴图（16px），永远不会跨出所属超级块，
 *   因此：无漂移、无拉伸、无跨瓦片串色、末端无黑边。配合 CLAMP_TO_EDGE 即可。
 */
#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace mymc
{
class Texture;

/// @brief 图集中一个瓦片的归一化 UV 矩形
struct UVRect
{
    glm::vec2 min{0.0f};  ///< 左上角（归一化 0~1）
    glm::vec2 size{0.0f}; ///< 宽高（归一化）
};

/// @brief 贴图图集构建器：把每张贴图按 repeat x repeat 拷贝排布进各自超级块。
class TextureAtlas
{
public:
    /**
     * @brief 构造图集
     * @param tile_size 贴图边长（像素），所有贴图会被最近邻缩放到该尺寸
     * @param repeat    超级块每边包含的拷贝数（= 一个方块面能含的最大块数，默认 16）
     */
    explicit TextureAtlas(int tile_size = 16, int repeat = 16);

    TextureAtlas(const TextureAtlas &) = delete;

    TextureAtlas &operator=(const TextureAtlas &) = delete;

    /// @brief 登记一张贴图（后续 build 时采样其 CPU 像素），同一张贴图只占一个超级块。
    void addTexture(const Texture &texture);

    /**
     * @brief 拼接所有已登记的贴图并创建 GL 纹理（CLAMP_TO_EDGE + NEAREST）。
     * @return 图集纹理；尺寸超限或失败时返回 nullptr
     */
    std::shared_ptr<Texture> build();

    /**
     * @brief 查询某张贴图对应的 UV 矩形（须在 build() 之后调用）。
     * @param texture 贴图
     * @return UV 矩形；未登记返回全零
     */
    [[nodiscard]] UVRect getUV(const Texture *texture) const;

    /// @brief 已登记的贴图数量
    [[nodiscard]] size_t getTileCount() const { return m_tiles.size(); }

private:
    int m_tileSize;                       ///< 贴图边长（像素）
    int m_repeat;                         ///< 超级块每边拷贝数（默认 16）
    std::vector<const Texture *> m_tiles; ///< 按登记顺序记录的贴图
    bool m_built = false;                 ///< 是否已拼接
    // build() 后填充：贴图 -> UV 矩形
    std::unordered_map<const Texture *, UVRect> m_uvMap;
};
} // namespace mymc