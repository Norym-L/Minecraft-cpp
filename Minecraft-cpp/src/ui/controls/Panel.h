#ifndef MINECRAFT_CPP_PANEL_H
#define MINECRAFT_CPP_PANEL_H

#include <glm/vec4.hpp>

#include "ui/UiComponent.h"

namespace ui
{
class UiBatchBuilder;

/// @brief 面板控件：纯色矩形块
class Panel : public UiComponent
{
public:
    void emit(UiBatchBuilder &buffer) const override;

    /// @brief 设置填充颜色
    void setColor(const glm::vec4 &color);

    [[nodiscard]] const glm::vec4 &getColor() const;

private:
    glm::vec4 m_color{0.2f, 0.2f, 0.2f, 0.9f}; // 填充颜色
};
} // namespace ui

#endif //MINECRAFT_CPP_PANEL_H