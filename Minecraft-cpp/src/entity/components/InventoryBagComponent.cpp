
#include "InventoryBagComponent.h"

#include <algorithm>
#include <utility>

namespace
{
// 判断槽位下标是否合法
bool isValidSlot(const int index)
{
    return index >= 0 && index < entity::TOTAL_SLOTS;
}

// 规范化物品堆叠：空方块强制数量 0，数量钳制到 [0, MAX_STACK_SIZE]
void normalize(entity::ItemStack &stack)
{
    if (stack.blockId == 0)
    {
        stack.count = 0;
    }
    else if (stack.count <= 0)
    {
        stack.count = 0;
    }
    else if (stack.count > entity::MAX_STACK_SIZE)
    {
        stack.count = entity::MAX_STACK_SIZE;
    }
}
} // namespace

namespace entity
{
void InventoryBagComponent::init()
{}

void InventoryBagComponent::update(float delta_time)
{}

int InventoryBagComponent::getSlotCount() const
{
    std::scoped_lock lock(m_mutex);
    return TOTAL_SLOTS;
}

ItemStack InventoryBagComponent::getSlot(const int index) const
{
    std::scoped_lock lock(m_mutex);
    if (!isValidSlot(index))
    {
        return {};
    }
    return m_slots[index];
}

void InventoryBagComponent::setSlot(const int index, const ItemStack &stack)
{
    std::scoped_lock lock(m_mutex);
    if (!isValidSlot(index))
    {
        return;
    }
    ItemStack normalized = stack;
    normalize(normalized);
    m_slots[index] = normalized;
}

ItemStack InventoryBagComponent::getHotbarSlot(const int hotbarIndex) const
{
    std::scoped_lock lock(m_mutex);
    if (hotbarIndex < 0 || hotbarIndex >= HOTBAR_SIZE)
    {
        return {};
    }
    return m_slots[hotbarIndex];
}

ItemStack InventoryBagComponent::getBackpackSlot(const int backpackIndex) const
{
    std::scoped_lock lock(m_mutex);
    if (backpackIndex < 0 || backpackIndex >= BACKPACK_SIZE)
    {
        return {};
    }
    return m_slots[BACKPACK_START + backpackIndex];
}

void InventoryBagComponent::setHotbarSlot(const int hotbarIndex, const ItemStack &stack)
{
    std::scoped_lock lock(m_mutex);
    if (hotbarIndex < 0 || hotbarIndex >= HOTBAR_SIZE)
    {
        return;
    }
    ItemStack normalized = stack;
    normalize(normalized);
    m_slots[hotbarIndex] = normalized;
}

void InventoryBagComponent::setBackpackSlot(const int backpackIndex, const ItemStack &stack)
{
    std::scoped_lock lock(m_mutex);
    if (backpackIndex < 0 || backpackIndex >= BACKPACK_SIZE)
    {
        return;
    }
    ItemStack normalized = stack;
    normalize(normalized);
    m_slots[BACKPACK_START + backpackIndex] = normalized;
}

ItemStack InventoryBagComponent::addItem(ItemStack stack)
{
    std::scoped_lock lock(m_mutex);
    normalize(stack);
    if (stack.blockId == 0 || stack.count <= 0)
    {
        return {};
    }

    int remaining = stack.count;

    // 第一趟：合并进同类（同 blockId）且未满的槽
    for (auto &slot: m_slots)
    {
        if (remaining <= 0)
        {
            break;
        }
        if (slot.blockId == stack.blockId && slot.count < MAX_STACK_SIZE)
        {
            const int capacity = MAX_STACK_SIZE - slot.count;
            const int moved    = std::min(remaining, capacity);
            slot.count += moved;
            remaining -= moved;
        }
    }

    // 第二趟：放入空槽
    for (auto &slot: m_slots)
    {
        if (remaining <= 0)
        {
            break;
        }
        if (slot.blockId == 0)
        {
            const int moved = std::min(remaining, MAX_STACK_SIZE);
            slot            = ItemStack{stack.blockId, moved};
            remaining -= moved;
        }
    }

    if (remaining <= 0)
    {
        return {};
    }
    return ItemStack{stack.blockId, remaining};
}

ItemStack InventoryBagComponent::removeItem(const int index, const int count)
{
    std::scoped_lock lock(m_mutex);
    if (!isValidSlot(index) || count <= 0 || m_slots[index].blockId == 0)
    {
        return {};
    }

    const std::uint64_t removedBlockId = m_slots[index].blockId;
    const int           removed        = std::min(m_slots[index].count, count);
    m_slots[index].count -= removed;
    if (m_slots[index].count == 0)
    {
        m_slots[index] = ItemStack{}; // 保持空槽不变量：blockId == 0 且 count == 0
    }
    return ItemStack{removedBlockId, removed};
}

void InventoryBagComponent::swapSlots(const int indexA, const int indexB)
{
    std::scoped_lock lock(m_mutex);
    if (indexA == indexB || !isValidSlot(indexA) || !isValidSlot(indexB))
    {
        return;
    }
    std::swap(m_slots[indexA], m_slots[indexB]);
}

bool InventoryBagComponent::isFull() const
{
    std::scoped_lock lock(m_mutex);
    return std::all_of(m_slots.begin(), m_slots.end(), [](const ItemStack &slot)
    {
        return slot.count == MAX_STACK_SIZE;
    });
}

void InventoryBagComponent::clear()
{
    std::scoped_lock lock(m_mutex);
    m_slots.fill(ItemStack{});
}
} // entity