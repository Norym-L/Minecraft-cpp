#include "Slot.h"

#include "ui/UiBatchBuilder.h"
#include "ui/font/IFont.h"

namespace ui
{
Slot::Slot(const IFont *font)
    : m_font(font)
{}

void Slot::emit(UiBatchBuilder &buffer) const
{
    // 背景（选中 > 悬停 > 常规）
    glm::vec4 bg = m_bgColor;
    if (m_selected)
    {
        bg = m_bgColorSelected;
    }
    else if (m_hovered)
    {
        bg = m_bgColorHover;
    }
    buffer.addQuad(UiQuad{m_position, m_position + m_size, bg, nullptr, glm::vec2(0.0f), glm::vec2(1.0f)});

    // 图标（内缩 8px）
    if (!m_empty && m_iconTexture != nullptr)
    {
        const glm::vec2 icon_min = m_position + glm::vec2(8.0f, 8.0f);
        const glm::vec2 icon_max = m_position + m_size - glm::vec2(8.0f, 8.0f);
        buffer.addQuad(UiQuad{icon_min, icon_max, glm::vec4(1.0f), m_iconTexture,
                              m_iconUV.min, m_iconUV.min + m_iconUV.size});
    }

    // 序号（左下）与数量（右上）
    if (m_font != nullptr)
    {
        if (!m_numberText.empty())
        {
            const float scale = 0.8f;
            m_font->drawText(buffer, m_numberText, m_position + glm::vec2(3.0f, 3.0f), scale,
                             glm::vec4(0.95f, 0.95f, 0.95f, 1.0f));
        }
        if (!m_cornerText.empty())
        {
            const float  scale = 0.8f;
            const glm::vec2 text_size = m_font->measure(m_cornerText, scale);
            const glm::vec2 pos(m_position.x + m_size.x - text_size.x - 3.0f,
                                m_position.y + m_size.y - text_size.y - 3.0f);
            m_font->drawText(buffer, m_cornerText, pos, scale, glm::vec4(1.0f));
        }
    }
}

void Slot::onMouseClick(const glm::vec2 &pos)
{
    if (onClickCallback)
    {
        onClickCallback();
    }
    UiComponent::onMouseClick(pos);
}
} // namespace ui