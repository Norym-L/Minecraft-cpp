#include "UiComponent.h"

namespace ui
{
void UiComponent::addChild(std::unique_ptr<UiComponent> child)
{
    if (!child)
    {
        return;
    }
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<UiComponent>> &UiComponent::getChildren() const
{
    return m_children;
}

std::vector<std::unique_ptr<UiComponent>> &UiComponent::getChildren()
{
    return m_children;
}

void UiComponent::initializeIfNeeded()
{
    if (m_initialized)
    {
        return;
    }
    init();
    m_initialized = true;
}

UiComponent *UiComponent::getParent() const
{
    return m_parent;
}

void UiComponent::setPosition(const glm::vec2 &position)
{
    m_position = position;
}

void UiComponent::setSize(const glm::vec2 &size)
{
    m_size = size;
}

const glm::vec2 &UiComponent::getPosition() const
{
    return m_position;
}

const glm::vec2 &UiComponent::getSize() const
{
    return m_size;
}

bool UiComponent::contains(const glm::vec2 &point) const
{
    return point.x >= m_position.x && point.x <= m_position.x + m_size.x &&
           point.y >= m_position.y && point.y <= m_position.y + m_size.y;
}

void UiComponent::onMouseEnter()
{
    m_hovered = true;
}

void UiComponent::onMouseLeave()
{
    m_hovered = false;
}

bool UiComponent::isHovered() const
{
    return m_hovered;
}
} // namespace ui