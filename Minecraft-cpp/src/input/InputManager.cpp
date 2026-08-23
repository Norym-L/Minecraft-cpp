#include "InputManager.h"

namespace mymc
{
void InputManager::setKeyPressed(const int key, const bool pressed)
{
    if (key < 0 || key >= static_cast<int>(m_keys.size()))
    {
        return;
    }

    m_keys[key] = pressed;
}

bool InputManager::isKeyPressed(const int key) const
{
    if (key < 0 || key >= static_cast<int>(m_keys.size()))
    {
        return false;
    }

    return m_keys[key];
}

void InputManager::setKeyWasPressed(const int key, const bool pressed)
{
    if (key < 0 || key >= static_cast<int>(m_KeyPressed.size()))
    {
        return;
    }

    m_KeyPressed[key] = pressed;
}

bool InputManager::wasKeyPressed(const int key) const
{
    if (key < 0 || key >= static_cast<int>(m_KeyPressed.size()))
    {
        return false;
    }

    return m_KeyPressed[key];
}

void InputManager::setKeyWasReleased(const int key, const bool Released)
{
    if (key < 0 || key >= static_cast<int>(m_KeyReleased.size()))
    {
        return;
    }

    m_KeyReleased[key] = Released;
}

bool InputManager::wasKeyReleased(const int key) const
{
    if (key < 0 || key >= static_cast<int>(m_KeyReleased.size()))
    {
        return false;
    }

    return m_KeyReleased[key];
}

void InputManager::setMouseButtonPressed(const int button, const bool pressed)
{
    if (button < 0 || button >= static_cast<int>(m_mouseKeys.size()))
    {
        return;
    }

    m_mouseKeys[button] = pressed;
}

bool InputManager::isMouseButtonPressed(const int button) const
{
    if (button < 0 || button >= static_cast<int>(m_mouseKeys.size()))
    {
        return false;
    }

    return m_mouseKeys[button];
}

void InputManager::setMouseButtonWasPressed(const int button, const bool pressed)
{
    if (button < 0 || button >= static_cast<int>(m_mouseKeysPressed.size()))
    {
        return;
    }

    m_mouseKeysPressed[button] = pressed;
}

bool InputManager::wasMouseButtonPressed(const int button) const
{
    if (button < 0 || button >= static_cast<int>(m_mouseKeysPressed.size()))
    {
        return false;
    }

    return m_mouseKeysPressed[button];
}

void InputManager::setMouseButtonWasReleased(const int button, const bool Released)
{
    if (button < 0 || button >= static_cast<int>(m_mouseKeysReleased.size()))
    {
        return;
    }

    m_mouseKeysReleased[button] = Released;
}

bool InputManager::wasMouseButtonReleased(const int button) const
{
    if (button < 0 || button >= static_cast<int>(m_mouseKeysReleased.size()))
    {
        return false;
    }

    return m_mouseKeysReleased[button];
}

void InputManager::setMousePositionOld(const glm::vec2 &pos)
{
    m_mousePositionOld = pos;
}

const glm::vec2 &InputManager::getMousePositionOld() const
{
    return m_mousePositionOld;
}

void InputManager::setMousePositionCurrent(const glm::vec2 &pos)
{
    m_mousePositionCurrent = pos;
}

const glm::vec2 &InputManager::getMousePositionCurrent() const
{
    return m_mousePositionCurrent;
}

void InputManager::setMousePositionChanged(const bool changed)
{
    m_mousePositionChanged = changed;
}

bool InputManager::isMousePositionChanged() const
{
    return m_mousePositionChanged;
}

bool InputManager::isMousePositionChangedEventEnable() const
{
    return m_mousePositionChangedEventEnable;
}

void InputManager::setMousePositionChangedEventEnable(const bool mouse_position_changed_event_enable)
{
    m_mousePositionChangedEventEnable = mouse_position_changed_event_enable;
}

void InputManager::addScrollOffset(const glm::vec2 &offset)
{
    m_scrollOffset += offset;
}

glm::vec2 InputManager::getScrollOffset() const
{
    return m_scrollOffset;
}

void InputManager::clearStates()
{
    setMousePositionChanged(false);
    m_scrollOffset = glm::vec2(0.0f);
    for (bool &i: m_KeyPressed)
    {
        i = false;
    }
    for (bool &i: m_KeyReleased)
    {
        i = false;
    }
    for (bool &i: m_mouseKeysPressed)
    {
        i = false;
    }
    for (bool &i: m_mouseKeysReleased)
    {
        i = false;
    }
}
} // mymc
