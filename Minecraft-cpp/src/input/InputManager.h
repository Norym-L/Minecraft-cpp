#ifndef MINECRAFT_CPP_INPUTMANAGER_H
#define MINECRAFT_CPP_INPUTMANAGER_H

#include <array>

#include "glm/vec2.hpp"

namespace mymc
{
    class InputManager
    {
    public:
        InputManager() = default;
        InputManager(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager& operator=(InputManager&&) = delete;

    public:
        /**
         * @brief 设置指定键盘按键的按下状态。
         * @param key     键码（GLFW 键码）
         * @param pressed 是否按下
         */
        void setKeyPressed(int key, bool pressed);

        /**
         * @brief 查询指定键盘按键是否被按下。
         * @param key 键码（GLFW 键码）
         * @return bool 按下返回 true，否则返回 false
         */
        [[nodiscard]] bool isKeyPressed(int key) const;

        /**
         * 标记指定键盘按键在本帧被按下。
         * @param key 键码（GLFW 键码）
         * @param pressed 是否按下
         */
        void setKeyWasPressed(int key, bool pressed);

        /**
         * @brief 查询指定键盘按键在本帧是否被按下。
         * @param key 键码（GLFW 键码）
         * @return bool 本帧被按下返回 true
         */
        [[nodiscard]] bool wasKeyPressed(int key) const;

        /**
         * 标记指定键盘按键在本帧被释放。
         * @param key 键码（GLFW 键码）
         * @param Released 是否标记释放
         */
        void setKeyWasReleased(int key, bool Released);

        /**
         * @brief 查询指定键盘按键在本帧是否被释放。
         * @param key 键码（GLFW 键码）
         * @return bool 本帧释放下返回 true
         */
        [[nodiscard]] bool wasKeyReleased(int key) const;

        /**
         * @brief 设置指定鼠标按键的按下状态。
         * @param button  鼠标按键（GLFW 鼠标键码）
         * @param pressed 是否按下
         */
        void setMouseButtonPressed(int button, bool pressed);

        /**
         * @brief 查询指定鼠标按键是否被按下。
         * @param button 鼠标按键（GLFW 鼠标键码）
         * @return bool 按下返回 true，否则返回 false
         */
        [[nodiscard]] bool isMouseButtonPressed(int button) const;

        /**
         * @brief 标记指定鼠标按键在本帧被按下。
         * @param button  鼠标按键（GLFW 鼠标键码）
         * @param pressed 是否标记为按下
         */
        void setMouseButtonWasPressed(int button, bool pressed);
        /**
         * @brief 查询指定鼠标按键在本帧是否被按下。
         * @param button 鼠标按键（GLFW 鼠标键码）
         * @return bool 本帧被按下返回 true
         */
        [[nodiscard]] bool wasMouseButtonPressed(int button) const;

        /**
         * @brief 标记指定鼠标按键在本帧被释放。
         * @param button   鼠标按键（GLFW 鼠标键码）
         * @param Released 是否标记为释放
         */
        void setMouseButtonWasReleased(int button, bool Released);
        /**
         * @brief 查询指定鼠标按键在本帧是否被释放。
         * @param button 鼠标按键（GLFW 鼠标键码）
         * @return bool 本帧被释放返回 true
         */
        [[nodiscard]] bool wasMouseButtonReleased(int button) const;

        /**
         * @brief 设置上一帧的鼠标位置。
         * @param pos 上一帧鼠标位置
         */
        void setMousePositionOld(const glm::vec2& pos);

        /**
         * @brief 获取上一帧的鼠标位置。
         * @return const glm::vec2& 上一帧鼠标位置
         */
        [[nodiscard]] const glm::vec2& getMousePositionOld() const;

        /**
         * @brief 设置当前帧的鼠标位置。
         * @param pos 当前帧鼠标位置
         */
        void setMousePositionCurrent(const glm::vec2& pos);

        /**
         * @brief 获取当前帧的鼠标位置。
         * @return const glm::vec2& 当前帧鼠标位置
         */
        [[nodiscard]] const glm::vec2& getMousePositionCurrent() const;

        /**
         * @brief 设置鼠标位置是否改变。
         * @param changed 是否改变
         */
        void setMousePositionChanged(bool changed);
        /**
         * @brief 查询鼠标位置是否改变。
         * @return bool 改变返回 true
         */
        [[nodiscard]] bool isMousePositionChanged() const;

        /// @brief 清空本帧的瞬时状态（鼠标位置改变标志与按键按下/释放标志）。
        void clearStates();

    private:
        std::array<bool, 512> m_keys = { false };               // 键盘按键状态表
        std::array<bool, 512> m_KeyPressed = { false };         // 键盘按键本帧按下状态表
        std::array<bool, 512> m_KeyReleased = { false };        // 键盘按键本帧释放状态表
        std::array<bool, 16> m_mouseKeys = { false };                       // 鼠标按键状态表
        std::array<bool, 16> m_mouseKeysPressed = { false };                // 鼠标按键本帧按下状态表
        std::array<bool, 16> m_mouseKeysReleased = { false };               // 鼠标按键本帧释放状态表
        glm::vec2 m_mousePositionOld = glm::vec2(0.0f);               // 上一帧鼠标位置
        glm::vec2 m_mousePositionCurrent = glm::vec2(0.0f);           // 当前帧鼠标位置
        bool m_mousePositionChanged = false;                                // 鼠标位置是否改变
    };
} // mymc

#endif //MINECRAFT_CPP_INPUTMANAGER_H
