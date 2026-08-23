
#ifndef MINECRAFT_CPP_INVENTORYSELECTORCOMPONENT_H
#define MINECRAFT_CPP_INVENTORYSELECTORCOMPONENT_H

#include <mutex>

#include "Component.h"
#include "InventoryBagComponent.h"

namespace entity
{
class InventorySelectorComponent final : public Component
{
public:
    void init() override;

    void update(float delta_time) override;

    /**
     * 获取当前选中的快捷栏下标
     * @return 下标（0~8）
     */
    [[nodiscard]] int getSelectedIndex() const;

    /**
     * 设置选中的快捷栏下标
     * @param index 目标下标（越界钳制到 [0, HOTBAR_SIZE-1]）
     */
    void setSelectedIndex(int index);

    /**
     * 轮换到下一个快捷栏位（0→1→…→8→0）
     */
    void selectNext();

    /**
     * 轮换到上一个快捷栏位（0→8→…→1→0）
     */
    void selectPrevious();

private:
    int m_selectedIndex = 0;   // 当前选中的快捷栏下标
    mutable std::mutex m_mutex; // 保护选中下标的互斥锁
};
} // entity

#endif //MINECRAFT_CPP_INVENTORYSELECTORCOMPONENT_H
