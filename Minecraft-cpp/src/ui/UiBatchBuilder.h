#ifndef MINECRAFT_CPP_UIBATCHBUILDER_H
#define MINECRAFT_CPP_UIBATCHBUILDER_H

#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "Common.h"

namespace mymc
{
class Texture;
}

namespace ui
{
/// @brief UI 四边形：像素空间矩形 + 顶点色 + 可选纹理与 UV。
/// 坐标语义：y 轴向上（与修复后的正交投影一致），origin 为左下角。
struct UiQuad
{
    glm::vec2       min{0.0f};       // 左下角（像素空间）
    glm::vec2       max{0.0f};       // 右上角
    glm::vec4       color{1.0f};     // 顶点色（有纹理时与采样相乘；无纹理时即纯色）
    mymc::Texture  *texture = nullptr; // nullptr = 纯色块（uUseTexture = 0，不采样）
    glm::vec2       uv_min{0.0f};    // 左下 UV（归一化）
    glm::vec2       uv_max{1.0f};    // 右上 UV
};

/// @brief UI 批量构建器：采集四边形，构建 8 浮点交错顶点（pos2+color4+uv2）与按纹理分组的批次。
class UiBatchBuilder
{
public:
    /// @brief 追加一个四边形（4 顶点 + 6 索引）
    void addQuad(const UiQuad &quad);

    /// @brief 清空已采集的四边形
    void clear();

    /**
     * @brief 构建顶点/索引/批次（按纹理指针稳定分组，同纹理连续成段）
     * @param out_vertices 输出交错顶点，8 浮点/顶点：[x,y | r,g,b,a | u,v]（与 UI 着色器 attribute 0/1/2 对齐）
     * @param out_indices  输出索引（每四边形 6 个）
     * @param out_batches  输出批次（UIBatch{texture, index_offset, index_count}）
     */
    void build(std::vector<float> &out_vertices,
               std::vector<uint32_t> &out_indices,
               std::vector<mymc::UIBatch> &out_batches) const;

private:
    std::vector<UiQuad> m_quads; // 已采集的四边形
};
} // namespace ui

#endif //MINECRAFT_CPP_UIBATCHBUILDER_H