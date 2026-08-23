#ifndef MINECRAFT_CPP_IMAGE_H
#define MINECRAFT_CPP_IMAGE_H

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "ui/UiComponent.h"

namespace mymc
{
class Texture;
}

namespace ui
{
class UiBatchBuilder;

/// @brief 图片控件：纹理矩形（可调色，可指定 UV 子区域；不拥有纹理）
class Image : public UiComponent
{
public:
    void emit(UiBatchBuilder &buffer) const override;

    /// @brief 设置纹理（不拥有；nullptr 时行为同纯色块）
    void setTexture(mymc::Texture *texture);

    /// @brief 设置色调（与纹理采样相乘；无纹理时即纯色）
    void setTint(const glm::vec4 &tint);

    /// @brief 设置 UV 子区域（归一化）
    void setUV(const glm::vec2 &uv_min, const glm::vec2 &uv_max);

private:
    mymc::Texture *m_texture = nullptr; // 纹理指针（不拥有）
    glm::vec4 m_tint{1.0f};             // 色调
    glm::vec2 m_uvMin{0.0f};            // 左下 UV
    glm::vec2 m_uvMax{1.0f};            // 右上 UV
};
} // namespace ui

#endif //MINECRAFT_CPP_IMAGE_H