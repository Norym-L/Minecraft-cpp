#ifndef MINECRAFT_CPP_BITMAPFONT_H
#define MINECRAFT_CPP_BITMAPFONT_H

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "IFont.h"

namespace mymc
{
class Texture;
}

namespace ui
{
/// @brief 内置 5x7 位图字体。
/// 字形表覆盖 ASCII 32~126 全部 95 槽位；实际字形包含数字 0-9、大写 A-Z 与常用符号
/// （空格 . : - + % /），其余槽位为空字形（渲染为空白）。小写字母绘制时映射为大写。
/// 图集纹理（96x48，16 列 6 行，格 6x8）在 ensureTexture() 时惰性创建（须 GL 线程）。
class BitmapFont final : public IFont
{
public:
    void drawText(UiBatchBuilder &buffer, const std::string &text,
                  const glm::vec2 &pos, float scale, const glm::vec4 &color) const override;

    [[nodiscard]] glm::vec2 measure(const std::string &text, float scale) const override;

    [[nodiscard]] float getLineHeight(float scale) const override;

    /// @brief 确保字形图集纹理已创建（GL 上下文就绪后调用，幂等）
    void ensureTexture();

private:
    using Glyph = std::array<uint8_t, 7>; // 每字符 7 行，行内低 5 位（bit4 = 最左列）

    /// @brief 取字符字形（32~126 之外返回空格字形）
    [[nodiscard]] const Glyph &glyphOf(char ch) const;

    void createTexture();

    std::shared_ptr<mymc::Texture> m_texture; // 字形图集纹理
    bool m_textureReady = false;              // 纹理是否已创建
};
} // namespace ui

#endif //MINECRAFT_CPP_BITMAPFONT_H