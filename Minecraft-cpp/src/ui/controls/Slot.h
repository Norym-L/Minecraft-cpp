#ifndef MINECRAFT_CPP_SLOT_H
#define MINECRAFT_CPP_SLOT_H

#include <functional>
#include <string>

#include <glm/vec4.hpp>

#include "graphics/TextureAtlas.h"
#include "ui/UiComponent.h"

namespace ui
{
class IFont;
class UiBatchBuilder;

/// @brief 格子控件：背景 + 可选图标（图集 UV）+ 角标文本，支持选中/悬停态与点击回调。
/// 只负责渲染与事件通知；物品数据由拥有者（HotbarWidget）每帧写入。
class Slot : public UiComponent
{
public:
    /// @param font 字体指针（不拥有；用于序号与数量角标）
    explicit Slot(const IFont *font);

    void emit(UiBatchBuilder &buffer) const override;

    void onMouseClick(const glm::vec2 &pos) override;

    // ---- 渲染状态（由拥有者每帧设置）----
    bool m_selected      = false;        // 是否选中（高亮背景）
    bool m_empty         = true;         // 是否空槽（不绘制图标）
    mymc::Texture *m_iconTexture = nullptr; // 图标纹理（不拥有）
    mymc::UVRect m_iconUV{glm::vec2(0.0f), glm::vec2(1.0f)}; // 图标 UV 区域
    std::string m_numberText;            // 左下角序号
    std::string m_cornerText;            // 右上角数量
    std::function<void()> onClickCallback; // 点击回调（由拥有者绑定）

private:
    const IFont *m_font = nullptr;          // 字体指针（不拥有）
    glm::vec4 m_bgColor{0.2f, 0.2f, 0.2f, 0.85f};        // 常规背景
    glm::vec4 m_bgColorHover{0.3f, 0.3f, 0.3f, 0.9f};    // 悬停背景
    glm::vec4 m_bgColorSelected{0.45f, 0.45f, 0.45f, 1.0f}; // 选中背景
};
} // namespace ui

#endif //MINECRAFT_CPP_SLOT_H