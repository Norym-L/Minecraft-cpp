#ifndef MINECRAFT_CPP_IFONT_H
#define MINECRAFT_CPP_IFONT_H

#include <string>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace ui
{
class UiBatchBuilder;

/// @brief 字体接口：文本绘制与测量。
/// 本批提供内置位图实现（BitmapFont）；后续接入 freetype 时实现同一接口即可无缝替换。
class IFont
{
public:
    virtual ~IFont() = default;

    /**
     * @brief 向批量构建器提交文本的字形四边形
     * @param buffer 批量构建器
     * @param text   文本内容（本批仅支持 ASCII；不可打印字符按空格跳过）
     * @param pos    文本左下角（像素空间）
     * @param scale  缩放倍数（1.0 = 原始 5x7 尺寸）
     * @param color  字形颜色
     */
    virtual void drawText(UiBatchBuilder &buffer, const std::string &text,
                          const glm::vec2 &pos, float scale, const glm::vec4 &color) const = 0;

    /**
     * @brief 测量文本宽度
     * @param text  文本内容
     * @param scale 缩放倍数
     * @return 文本宽高（像素）
     */
    [[nodiscard]] virtual glm::vec2 measure(const std::string &text, float scale) const = 0;

    /**
     * @brief 获取行高
     * @param scale 缩放倍数
     * @return 行高（像素）
     */
    [[nodiscard]] virtual float getLineHeight(float scale) const = 0;
};
} // namespace ui

#endif //MINECRAFT_CPP_IFONT_H