#include "TextureAtlas.h"

#include <algorithm>
#include <iostream>

#include "Texture.h"

namespace mymc
{
TextureAtlas::TextureAtlas(const int tile_size, const int repeat)
    : m_tileSize(tile_size > 0 ? tile_size : 16), m_repeat(repeat > 0 ? repeat : 16)
{}

void TextureAtlas::addTexture(const Texture &texture)
{
    if (m_built)
    {
        return; // 已拼接后不允许再登记
    }
    if (m_uvMap.find(&texture) != m_uvMap.end())
    {
        return; // 同一张贴图只占一个超级块（TextureManager 缓存保证共享同一对象）
    }

    m_tiles.push_back(&texture);
    m_uvMap.emplace(&texture, UVRect{}); // build 时才写入真正的归一化矩形
}

std::shared_ptr<Texture> TextureAtlas::build()
{
    if (m_built)
    {
        return nullptr;
    }
    if (m_tiles.empty())
    {
        return nullptr;
    }

    // 超级块：每边 repeat 个贴图拷贝，供 greedy 大面整块重复同一个贴图
    const int block_size  = m_tileSize * m_repeat; // 超级块边长（像素）
    const int atlas_width = block_size;
    const int atlas_height = static_cast<int>(m_tiles.size()) * block_size;

    // OpenGL 纹理尺寸上限检查
    GLint max_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
    if (atlas_width > max_size || atlas_height > max_size)
    {
        std::cerr << "[TextureAtlas] 图集尺寸 " << atlas_width << "x" << atlas_height
                  << " 超过 GL_MAX_TEXTURE_SIZE=" << max_size << std::endl;
        return nullptr;
    }

    // 拼接像素（RGBA，默认全 0）
    std::vector<unsigned char> pixels(static_cast<size_t>(atlas_width) * atlas_height * 4, 0);

    for (size_t i = 0; i < m_tiles.size(); ++i)
    {
        const Texture *tex = m_tiles[i];
        if (!tex || !tex->hasPixels())
        {
            continue; // 没有 CPU 像素的贴图跳过（超级块保持全 0）
        }

        const int src_width   = tex->getWidth();
        const int src_height  = tex->getHeight();
        const int src_channels = tex->getChannels();
        const auto &src       = tex->getPixels();

        // 先把贴图最近邻缩放到 m_tileSize，写入临时缓存，再按 super 网格复制
        std::vector<unsigned char> tile_cache(static_cast<size_t>(m_tileSize) * m_tileSize * 4, 0);
        for (int ty = 0; ty < m_tileSize; ++ty)
        {
            const int sy = std::min((ty * src_height) / m_tileSize, src_height - 1);
            for (int tx = 0; tx < m_tileSize; ++tx)
            {
                const int sx = std::min((tx * src_width) / m_tileSize, src_width - 1);
                const size_t src_index = (static_cast<size_t>(sy) * src_width + sx) * src_channels;
                unsigned char *dst = &tile_cache[(static_cast<size_t>(ty) * m_tileSize + tx) * 4];
                dst[0] = src_channels > 0 ? src[src_index + 0] : 0;
                dst[1] = src_channels > 1 ? src[src_index + 1] : dst[0];
                dst[2] = src_channels > 2 ? src[src_index + 2] : dst[0];
                dst[3] = src_channels > 3 ? src[src_index + 3] : 255;
            }
        }

        // 拷贝到超级块内的每个格子
        const int super_row = static_cast<int>(i); // 每种贴图独占一行超块
        for (int ry = 0; ry < m_repeat; ++ry)
        {
            for (int rx = 0; rx < m_repeat; ++rx)
            {
                for (int ty = 0; ty < m_tileSize; ++ty)
                {
                    const int dst_y = super_row * block_size + ry * m_tileSize + ty;
                    for (int tx = 0; tx < m_tileSize; ++tx)
                    {
                        const int dst_x = rx * m_tileSize + tx;
                        const size_t dst = (static_cast<size_t>(dst_y) * atlas_width + dst_x) * 4;
                        const size_t tile_src = (static_cast<size_t>(ty) * m_tileSize + tx) * 4;
                        pixels[dst + 0] = tile_cache[tile_src + 0];
                        pixels[dst + 1] = tile_cache[tile_src + 1];
                        pixels[dst + 2] = tile_cache[tile_src + 2];
                        pixels[dst + 3] = tile_cache[tile_src + 3];
                    }
                }
            }
        }
    }

    auto atlas_texture = std::make_shared<Texture>(atlas_width, atlas_height, 4, pixels.data());
    atlas_texture->configurePixelArt(); // CLAMP_TO_EDGE + NEAREST：像素风且无图集渗色

    // UV：
    //   u 轴（超级块横向 = 256px = 完整宽度）：每方块步长 = 1/repeat；
    //   v 轴（超级块纵向 256px 归一化到总高 tiles*256）：每方块步长 = (1/repeat)/tiles，起点偏移到所属超级块。
    //   这样任意 <=16x16 的贪心大面都只会采样到自己的超级块，无跨瓦片污染。
    const float super_block_v = 1.0f / static_cast<float>(m_tiles.size());
    const float step_u = 1.0f / static_cast<float>(m_repeat);
    const float step_v = step_u * super_block_v;
    for (size_t i = 0; i < m_tiles.size(); ++i)
    {
        const auto it = m_uvMap.find(m_tiles[i]);
        if (it == m_uvMap.end())
        {
            continue;
        }
        it->second = UVRect{
            glm::vec2(0.0f, super_block_v * static_cast<float>(i)),
            glm::vec2(step_u, step_v)
        };
    }

    m_built = true;
    return atlas_texture;
}

UVRect TextureAtlas::getUV(const Texture *texture) const
{
    if (!m_built || !texture)
    {
        return UVRect{};
    }
    const auto it = m_uvMap.find(texture);
    return it != m_uvMap.end() ? it->second : UVRect{};
}
} // namespace mymc