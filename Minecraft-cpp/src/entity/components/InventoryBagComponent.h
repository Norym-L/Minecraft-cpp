
#ifndef MINECRAFT_CPP_INVENTORYBAGCOMPONENT_H
#define MINECRAFT_CPP_INVENTORYBAGCOMPONENT_H

#include <array>
#include <cstdint>
#include <mutex>

#include "Component.h"

namespace entity
{
// 物品堆叠：blockId == 0 表示空槽（空气），此时 count 恒为 0
struct ItemStack
{
    std::uint64_t blockId = 0; // 方块注册 id（0 = 空气/空）
    int           count   = 0; // 堆叠数量
};

inline constexpr int HOTBAR_SIZE    = 9;                          // 快捷栏槽位数
inline constexpr int BACKPACK_START = HOTBAR_SIZE;                // 背包起始下标（紧邻快捷栏之后）
inline constexpr int BACKPACK_SIZE  = 27;                         // 背包槽位数
inline constexpr int TOTAL_SLOTS    = HOTBAR_SIZE + BACKPACK_SIZE; // 总槽位数 36
inline constexpr int MAX_STACK_SIZE = 64;                         // 单格最大堆叠数

class InventoryBagComponent final : public Component
{
public:
    void init() override;

    void update(float delta_time) override;

    /**
     * 获取总槽位数
     * @return 槽位总数
     */
    [[nodiscard]] int getSlotCount() const;

    /**
     * 获取指定槽位的物品堆叠副本
     * @param index 槽位下标（0~35）
     * @return 槽位物品副本；越界返回空堆叠
     */
    [[nodiscard]] ItemStack getSlot(int index) const;

    /**
     * 设置指定槽位
     * @param index 槽位下标
     * @param stack 物品堆叠（自动规范化：空方块强制数量 0，数量钳制 0~64）
     */
    void setSlot(int index, const ItemStack &stack);

    /**
     * 获取快捷栏槽位
     * @param hotbarIndex 快捷栏下标（0~8）
     * @return 槽位物品副本；越界返回空堆叠
     */
    [[nodiscard]] ItemStack getHotbarSlot(int hotbarIndex) const;

    /**
     * 获取背包槽位
     * @param backpackIndex 背包下标（0~26，映射到全局下标 9~35）
     * @return 槽位物品副本；越界返回空堆叠
     */
    [[nodiscard]] ItemStack getBackpackSlot(int backpackIndex) const;

    /**
     * 设置快捷栏槽位
     * @param hotbarIndex 快捷栏下标（0~8）
     * @param stack 物品堆叠
     */
    void setHotbarSlot(int hotbarIndex, const ItemStack &stack);

    /**
     * 设置背包槽位
     * @param backpackIndex 背包下标（0~26）
     * @param stack 物品堆叠
     */
    void setBackpackSlot(int backpackIndex, const ItemStack &stack);

    /**
     * 尽可能把物品堆叠进物品栏：先合并进同类未满格，再放入空槽
     * @param stack 待放入的物品堆叠
     * @return 未能放入的剩余堆叠（count == 0 表示全部放入）
     */
    [[nodiscard]] ItemStack addItem(ItemStack stack);

    /**
     * 从指定槽位移除指定数量的物品
     * @param index 槽位下标
     * @param count 移除数量
     * @return 实际移除的物品堆叠；槽位越界、数量非正或槽位为空时返回空堆叠
     */
    [[nodiscard]] ItemStack removeItem(int index, int count);

    /**
     * 交换两个槽位的物品（越界或同槽位时不操作）
     * @param indexA 槽位 A
     * @param indexB 槽位 B
     */
    void swapSlots(int indexA, int indexB);

    /**
     * 物品栏是否已满（所有槽位均达到最大堆叠）
     * @return true 已满 false 未满
     */
    [[nodiscard]] bool isFull() const;

    /**
     * 清空物品栏所有槽位
     */
    void clear();

private:
    std::array<ItemStack, TOTAL_SLOTS> m_slots{}; // 槽位数据：下标 0~8 快捷栏，9~35 背包
    mutable std::mutex m_mutex;                   // 保护槽位数据的互斥锁
};
} // entity

#endif //MINECRAFT_CPP_INVENTORYBAGCOMPONENT_H
