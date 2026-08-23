#ifndef MINECRAFT_CPP_HOTBARWIDGET_H
#define MINECRAFT_CPP_HOTBARWIDGET_H

#include "ui/UiComponent.h"

namespace ui
{
class IFont;
class Slot;

/// @brief 快捷栏热栏组件：9 格（HOTBAR_SIZE），绑定玩家实体的物品栏与选择组件。
/// 支持点击选格（Slot::onClickCallback）、数字键 1-9 与滚轮轮换（selectNext/selectPrevious）。
class HotbarWidget : public UiComponent
{
public:
    /// @param font 字体指针（不拥有；透传给各 Slot 角标）
    explicit HotbarWidget(const IFont *font);

    void update(float delta_time) override;

private:
    /// @brief 设置指定槽位为选中
    void selectSlot(int index);

    static constexpr float kSlotSize = 56.0f; // 单格尺寸（px）
    static constexpr float kSlotGap  = 4.0f;  // 格间距（px）
};
} // namespace ui

#endif //MINECRAFT_CPP_HOTBARWIDGET_H