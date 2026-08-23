#include "Image.h"

#include "ui/UiBatchBuilder.h"

namespace ui
{
void Image::emit(UiBatchBuilder &buffer) const
{
    buffer.addQuad(UiQuad{m_position, m_position + m_size, m_tint, m_texture, m_uvMin, m_uvMax});
}

void Image::setTexture(mymc::Texture *texture)
{
    m_texture = texture;
}

void Image::setTint(const glm::vec4 &tint)
{
    m_tint = tint;
}

void Image::setUV(const glm::vec2 &uv_min, const glm::vec2 &uv_max)
{
    m_uvMin = uv_min;
    m_uvMax = uv_max;
}
} // namespace ui