#include "Text.h"

#include <cassert>

#include "ui/UiBatchBuilder.h"
#include "ui/font/IFont.h"

namespace ui
{
Text::Text(const IFont *font)
    : m_font(font)
{
    assert(m_font != nullptr && "Text 需要注入非空字体指针");
}

void Text::emit(UiBatchBuilder &buffer) const
{
    if (m_font == nullptr)
    {
        return;
    }
    m_font->drawText(buffer, m_text, m_position, m_scale, m_color);
}

void Text::setText(const std::string &text)
{
    m_text = text;
}

void Text::setScale(const float scale)
{
    m_scale = scale;
}

void Text::setColor(const glm::vec4 &color)
{
    m_color = color;
}

glm::vec2 Text::measureText() const
{
    if (m_font == nullptr)
    {
        return {};
    }
    return m_font->measure(m_text, m_scale);
}
} // namespace ui