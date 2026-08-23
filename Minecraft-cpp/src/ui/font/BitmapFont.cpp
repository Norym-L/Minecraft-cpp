#include "BitmapFont.h"

#include <vector>

#include "../UiBatchBuilder.h"
#include "graphics/Texture.h"

namespace
{
using Glyph = std::array<uint8_t, 7>;

// 从 "10101" 式行串构造字形（7 行 × 5 列，'1' = 像素，bit4 为最左列）
Glyph makeGlyph(const std::array<const char *, 7> &rows)
{
    Glyph g{};
    for (int r = 0; r < 7; ++r)
    {
        uint8_t v = 0;
        for (int c = 0; c < 5; ++c)
        {
            if (rows[r][c] == '1')
            {
                v |= static_cast<uint8_t>(1u << (4 - c));
            }
        }
        g[r] = v;
    }
    return g;
}

// 构建 95 槽字形表（ASCII 32~126，槽位 = ch - 32）
std::array<Glyph, 95> buildGlyphTable()
{
    std::array<Glyph, 95> table{};
    table.fill(Glyph{}); // 默认空字形（空白）

    // 空格
    table[0] = makeGlyph({"00000", "00000", "00000", "00000", "00000", "00000", "00000"});
    // 数字 0-9（ASCII 48~57 → 槽 16~25）
    table[16] = makeGlyph({"11110", "10001", "10001", "10001", "10001", "10001", "11110"});
    table[17] = makeGlyph({"00010", "00110", "00010", "00010", "00010", "00010", "00111"});
    table[18] = makeGlyph({"11110", "00001", "00001", "11110", "10000", "10000", "11111"});
    table[19] = makeGlyph({"11110", "00001", "00001", "01110", "00001", "00001", "11110"});
    table[20] = makeGlyph({"10001", "10001", "10001", "11111", "00001", "00001", "00001"});
    table[21] = makeGlyph({"11111", "10000", "10000", "11110", "00001", "00001", "11110"});
    table[22] = makeGlyph({"11110", "10000", "10000", "11110", "10001", "10001", "11110"});
    table[23] = makeGlyph({"11111", "00001", "00001", "00010", "00100", "00100", "00100"});
    table[24] = makeGlyph({"11110", "10001", "10001", "11110", "10001", "10001", "11110"});
    table[25] = makeGlyph({"11110", "10001", "10001", "11111", "00001", "00001", "11110"});
    // 常用符号：. : - + % /（ASCII 46,58,45,43,37,47 → 槽 14,26,13,11,5,15），/ 用 47
    table[13] = makeGlyph({"00000", "00000", "00000", "11111", "00000", "00000", "00000"}); // '-'
    table[14] = makeGlyph({"00000", "00000", "00000", "00000", "00000", "00110", "00110"}); // '.'
    table[15] = makeGlyph({"00001", "00010", "00010", "00100", "01000", "01000", "10000"}); // '/'
    table[11] = makeGlyph({"00000", "00100", "00100", "11111", "00100", "00100", "00000"}); // '+'
    table[5]  = makeGlyph({"10010", "00100", "01000", "00010", "00100", "01001", "10010"}); // '%'
    table[26] = makeGlyph({"00000", "00110", "00110", "00000", "00110", "00110", "00000"}); // ':'
    // 大写 A-Z（ASCII 65~90 → 槽 33~58）
    table[33] = makeGlyph({"01110", "10001", "10001", "11111", "10001", "10001", "10001"}); // A
    table[34] = makeGlyph({"11110", "10001", "10001", "11110", "10001", "10001", "11110"}); // B
    table[35] = makeGlyph({"01110", "10001", "10000", "10000", "10000", "10001", "01110"}); // C
    table[36] = makeGlyph({"11100", "10010", "10001", "10001", "10001", "10010", "11100"}); // D
    table[37] = makeGlyph({"11111", "10000", "10000", "11110", "10000", "10000", "11111"}); // E
    table[38] = makeGlyph({"11111", "10000", "10000", "11110", "10000", "10000", "10000"}); // F
    table[39] = makeGlyph({"01110", "10001", "10000", "10111", "10001", "10001", "01111"}); // G
    table[40] = makeGlyph({"10001", "10001", "10001", "11111", "10001", "10001", "10001"}); // H
    table[41] = makeGlyph({"11111", "00100", "00100", "00100", "00100", "00100", "11111"}); // I
    table[42] = makeGlyph({"00111", "00010", "00010", "00010", "00010", "10010", "01100"}); // J
    table[43] = makeGlyph({"10001", "10010", "10100", "11000", "10100", "10010", "10001"}); // K
    table[44] = makeGlyph({"10000", "10000", "10000", "10000", "10000", "10000", "11111"}); // L
    table[45] = makeGlyph({"10001", "11011", "10101", "10101", "10001", "10001", "10001"}); // M
    table[46] = makeGlyph({"10001", "11001", "10101", "10011", "10001", "10001", "10001"}); // N
    table[47] = makeGlyph({"01110", "10001", "10001", "10001", "10001", "10001", "01110"}); // O
    table[48] = makeGlyph({"11110", "10001", "10001", "11110", "10000", "10000", "10000"}); // P
    table[49] = makeGlyph({"01110", "10001", "10001", "10001", "10101", "10010", "01101"}); // Q
    table[50] = makeGlyph({"11110", "10001", "10001", "11110", "10100", "10010", "10001"}); // R
    table[51] = makeGlyph({"01111", "10000", "10000", "01110", "00001", "00001", "11110"}); // S
    table[52] = makeGlyph({"11111", "00100", "00100", "00100", "00100", "00100", "00100"}); // T
    table[53] = makeGlyph({"10001", "10001", "10001", "10001", "10001", "10001", "01110"}); // U
    table[54] = makeGlyph({"10001", "10001", "10001", "10001", "10001", "01010", "00100"}); // V
    table[55] = makeGlyph({"10001", "10001", "10001", "10101", "10101", "10101", "01010"}); // W
    table[56] = makeGlyph({"10001", "10001", "01010", "00100", "01010", "10001", "10001"}); // X
    table[57] = makeGlyph({"10001", "10001", "01010", "00100", "00100", "00100", "00100"}); // Y
    table[58] = makeGlyph({"11111", "00001", "00010", "00100", "01000", "10000", "11111"}); // Z
    return table;
}

const std::array<Glyph, 95> &glyphTable()
{
    static const std::array<Glyph, 95> table = buildGlyphTable();
    return table;
}
} // namespace

namespace ui
{
const BitmapFont::Glyph &BitmapFont::glyphOf(const char ch) const
{
    const int index = static_cast<int>(ch) - 32;
    if (index < 0 || index >= static_cast<int>(glyphTable().size()))
    {
        return glyphTable()[0]; // 越界 → 空格字形
    }
    return glyphTable()[static_cast<size_t>(index)];
}

void BitmapFont::ensureTexture()
{
    if (m_textureReady)
    {
        return;
    }
    createTexture();
    m_textureReady = true;
}

void BitmapFont::createTexture()
{
    constexpr int kCols  = 16; // 图集列数
    constexpr int kRows  = 6;  // 图集行数
    constexpr int kCellW = 6;  // 格宽（5 字形列 + 1 内边距）
    constexpr int kCellH = 8;  // 格高（7 字形行 + 1 内边距）
    constexpr int kTexW  = kCols * kCellW; // 96
    constexpr int kTexH  = kRows * kCellH; // 48

    std::vector<unsigned char> pixels(static_cast<size_t>(kTexW) * kTexH * 4, 0);
    for (int i = 0; i < 95; ++i)
    {
        const char ch = static_cast<char>(i + 32);
        const Glyph &glyph = glyphOf(ch);
        const int col      = i % kCols;
        const int row      = i / kCols;
        const int ox       = col * kCellW;
        const int oy       = row * kCellH;
        for (int r = 0; r < 7; ++r)
        {
            for (int c = 0; c < 5; ++c)
            {
                const bool on = (glyph[r] & (1u << (4 - c))) != 0;
                if (!on)
                {
                    continue;
                }
                const int   px  = ox + c;
                const int   py  = oy + r;
                const auto  idx = (static_cast<size_t>(py) * kTexW + static_cast<size_t>(px)) * 4u;
                pixels[idx + 0] = 255; // R
                pixels[idx + 1] = 255; // G
                pixels[idx + 2] = 255; // B
                pixels[idx + 3] = 255; // A
            }
        }
    }

    m_texture = std::make_shared<mymc::Texture>(kTexW, kTexH, 4, pixels.data());
    m_texture->configurePixelArt(); // 像素风：NEAREST + CLAMP_TO_EDGE
}

void BitmapFont::drawText(UiBatchBuilder &buffer, const std::string &text,
                          const glm::vec2 &pos, const float scale, const glm::vec4 &color) const
{
    if (m_texture == nullptr)
    {
        return; // ensureTexture 未调用：静默跳过（GL 线程约束由调用方保证）
    }

    constexpr float kAdvance = 6.0f; // 字形步进（5 宽 + 1 间距）
    constexpr float kGlyphW  = 5.0f; // 字形宽
    constexpr float kGlyphH  = 7.0f; // 字形高
    constexpr float kTexW    = 96.0f;
    constexpr float kTexH    = 48.0f;

    for (size_t k = 0; k < text.size(); ++k)
    {
        char ch = text[k];
        if (ch >= 'a' && ch <= 'z')
        {
            ch = static_cast<char>(ch - 'a' + 'A'); // 小写映射大写（字模表无小写）
        }
        if (ch < 32 || ch > 126)
        {
            continue; // 不可打印字符：不占绘制（仍按步进前进）
        }

        const int  cell = ch - 32;
        const int  col  = cell % 16;
        const int  row  = cell / 16;
        const auto u0   = (static_cast<float>(col) * 6.0f + 0.0f) / kTexW;
        const auto v0   = 1.0f - (static_cast<float>(row) * 8.0f + 7.0f) / kTexH;
        const auto u1   = (static_cast<float>(col) * 6.0f + 5.0f) / kTexW;
        const auto v1   = 1.0f - (static_cast<float>(row) * 8.0f) / kTexH;

        const float x0 = pos.x + static_cast<float>(k) * kAdvance * scale;
        const float x1 = x0 + kGlyphW * scale;
        const float y0 = pos.y;
        const float y1 = y0 + kGlyphH * scale;

        buffer.addQuad(UiQuad{glm::vec2(x0, y0), glm::vec2(x1, y1), color, m_texture.get(),
                              glm::vec2(u0, v0), glm::vec2(u1, v1)});
    }
}

glm::vec2 BitmapFont::measure(const std::string &text, const float scale) const
{
    return glm::vec2(static_cast<float>(text.size()) * 6.0f * scale, 7.0f * scale);
}

float BitmapFont::getLineHeight(const float scale) const
{
    return 7.0f * scale;
}
} // namespace ui