#include "Panel.h"

#include "ui/UiBatchBuilder.h"

namespace ui
{
void Panel::emit(UiBatchBuilder &buffer) const
{
    buffer.addQuad(UiQuad{m_position, m_position + m_size, m_color, nullptr, glm::vec2(0.0f), glm::vec2(1.0f)});
}

void Panel::setColor(const glm::vec4 &color)
{
    m_color = color;
}

const glm::vec4 &Panel::getColor() const
{
    return m_color;
}
} // namespace ui