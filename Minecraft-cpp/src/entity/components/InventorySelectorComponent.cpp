
#include "InventorySelectorComponent.h"

#include <algorithm>

namespace entity
{
void InventorySelectorComponent::init()
{}

void InventorySelectorComponent::update(float delta_time)
{}

int InventorySelectorComponent::getSelectedIndex() const
{
    std::scoped_lock lock(m_mutex);
    return m_selectedIndex;
}

void InventorySelectorComponent::setSelectedIndex(const int index)
{
    std::scoped_lock lock(m_mutex);
    m_selectedIndex = std::clamp(index, 0, HOTBAR_SIZE - 1);
}

void InventorySelectorComponent::selectNext()
{
    std::scoped_lock lock(m_mutex);
    m_selectedIndex = (m_selectedIndex + 1) % HOTBAR_SIZE;
}

void InventorySelectorComponent::selectPrevious()
{
    std::scoped_lock lock(m_mutex);
    m_selectedIndex = (m_selectedIndex - 1 + HOTBAR_SIZE) % HOTBAR_SIZE;
}
} // entity