#ifndef MINECRAFT_CPP_TEXT_H
#define MINECRAFT_CPP_TEXT_H

#include <string>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "ui/UiComponent.h"

namespace ui
{
class IFont;
class UiBatchBuilder;

/// @brief 文本控件：使用注入的 IFont 绘制文本（本批仅 ASCII）
class Text : public UiComponent
{
public:
    /// @param font 字体指针（不拥有；须非空）
    explicit Text(const IFont *font);

    void emit(UiBatchBuilder &buffer) const override;

    void setText(const std::string &text);

    void setScale(float scale);

    void setColor(const glm::vec4 &color);

    /// @brief 便捷测量当前文本宽度
    [[nodiscard]] glm::vec2 measureText() const;

private:
    const IFont *m_font = nullptr;  // 字体指针（不拥有）
    std::string m_text;             // 文本内容
    float m_scale = 1.0f;           // 缩放
    glm::vec4 m_color{1.0f};        // 颜色
};
} // namespace ui

#endif //MINECRAFT_CPP_TEXT_H