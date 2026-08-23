#include "UiBatchBuilder.h"

#include <algorithm>
#include <utility>

namespace ui
{
void UiBatchBuilder::addQuad(const UiQuad &quad)
{
    m_quads.push_back(quad);
}

void UiBatchBuilder::clear()
{
    m_quads.clear();
}

void UiBatchBuilder::build(std::vector<float> &out_vertices,
                           std::vector<uint32_t> &out_indices,
                           std::vector<mymc::UIBatch> &out_batches) const
{
    out_vertices.clear();
    out_indices.clear();
    out_batches.clear();

    // 按纹理指针稳定排序：同纹理四边形连续成段（nullptr 视为"纯色"组）
    std::vector<UiQuad> sorted = m_quads;
    std::stable_sort(sorted.begin(), sorted.end(), [](const UiQuad &a, const UiQuad &b)
    {
        return static_cast<const void *>(a.texture) < static_cast<const void *>(b.texture);
    });

    out_vertices.reserve(sorted.size() * 4 * 8);
    out_indices.reserve(sorted.size() * 6);

    mymc::Texture *current_texture = nullptr;
    uint32_t       batch_start     = 0;
    uint32_t       batch_count     = 0;

    for (const auto &quad: sorted)
    {
        // 纹理变化 → 收束上一批次，另起新段
        if (quad.texture != current_texture)
        {
            if (batch_count > 0)
            {
                out_batches.push_back({current_texture, batch_start, batch_count});
            }
            current_texture = quad.texture;
            batch_start     = static_cast<uint32_t>(out_indices.size());
            batch_count     = 0;
        }

        const uint32_t base = static_cast<uint32_t>(out_vertices.size() / 8u);

        // 四顶点：v0 左下 → v1 右下 → v2 右上 → v3 左上
        const glm::vec2 bottom_right(quad.max.x, quad.min.y);
        const glm::vec2 top_left(quad.min.x, quad.max.y);

        const float v0[8] = {quad.min.x,        quad.min.y,        quad.color.r, quad.color.g, quad.color.b, quad.color.a, quad.uv_min.x, quad.uv_min.y};
        const float v1[8] = {bottom_right.x,    bottom_right.y,    quad.color.r, quad.color.g, quad.color.b, quad.color.a, quad.uv_max.x, quad.uv_min.y};
        const float v2[8] = {quad.max.x,        quad.max.y,        quad.color.r, quad.color.g, quad.color.b, quad.color.a, quad.uv_max.x, quad.uv_max.y};
        const float v3[8] = {top_left.x,        top_left.y,        quad.color.r, quad.color.g, quad.color.b, quad.color.a, quad.uv_min.x, quad.uv_max.y};

        out_vertices.insert(out_vertices.end(), std::begin(v0), std::end(v0));
        out_vertices.insert(out_vertices.end(), std::begin(v1), std::end(v1));
        out_vertices.insert(out_vertices.end(), std::begin(v2), std::end(v2));
        out_vertices.insert(out_vertices.end(), std::begin(v3), std::end(v3));

        out_indices.insert(out_indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
        batch_count += 6;
    }

    if (batch_count > 0)
    {
        out_batches.push_back({current_texture, batch_start, batch_count});
    }
}
} // namespace ui