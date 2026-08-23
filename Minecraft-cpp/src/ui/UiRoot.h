
#ifndef MINECRAFT_CPP_UIROOT_H
#define MINECRAFT_CPP_UIROOT_H

#include <memory>

#include <glm/vec2.hpp>

#include "UiComponent.h"
#include "font/IFont.h"

namespace mymc
{
class Mesh;
}

namespace ui
{
class UiBatchBuilder;

/// @brief UI 根组件：组件树挂载点，驱动每帧更新/渲染与输入路由。
/// 生命周期：构造（记录屏幕尺寸）→ init（GL 就绪后，创建字体与动态网格）→
/// update（每帧，路由输入 + 递归子组件更新）→ render（每帧，收集四边形→更新网格→提交 UI 命令）。
class UiRoot
{
public:
    UiRoot(int screen_width, int screen_height);

    ~UiRoot();

    UiRoot(const UiRoot &) = delete;

    UiRoot &operator=(const UiRoot &) = delete;

    /// @brief 初始化（必须在 GL 上下文就绪后调用）：创建字体与动态 UI 网格
    void init();

    /// @brief 每帧更新：输入路由（hover/click 分发）+ 递归子组件 update
    void update(float delta_time);

    /// @brief 每帧渲染：递归采集四边形 → 更新动态网格 → 提交 RenderCommandUI
    void render();

    /// @brief 同步窗口尺寸（GLFW 窗口尺寸回调调用）
    void setWindowSize(int width, int height);

    /// @brief 添加根级子组件
    void addChild(std::unique_ptr<UiComponent> child);

    /// @brief 设置字体（默认使用内置 BitmapFont；须在 init 前调用）
    void setFont(std::unique_ptr<IFont> font);

    /// @brief 获取当前字体指针（未 init 时为构造传入或 nullptr）
    [[nodiscard]] IFont *getFont() const;

private:
    void routeInput();

    void updateRecursive(UiComponent &node, float delta_time);

    void emitRecursive(UiBatchBuilder &buffer, const UiComponent &node) const;

    /// @brief 递归命中测试：返回最深层命中的组件
    [[nodiscard]] UiComponent *hitTest(const glm::vec2 &point, UiComponent &node) const;

    /// @brief GLFW 鼠标坐标（左上原点）→ UI 坐标（左下原点）
    [[nodiscard]] glm::vec2 toUiSpace(const glm::vec2 &mouse_pos) const;

    int m_screenWidth  = 0;                 // 屏幕宽度（像素）
    int m_screenHeight = 0;                 // 屏幕高度（像素）
    std::unique_ptr<IFont> m_font;          // 字体（默认 BitmapFont，归本对象）
    bool m_fontInitialized = false;         // 字体纹理已就绪
    std::unique_ptr<mymc::Mesh> m_mesh;     // 复用的动态 UI 网格（每帧 updateDynamic）
    std::unique_ptr<UiComponent> m_root;    // 根级容器节点（自身不渲染）
    UiComponent *m_hover = nullptr;         // 当前悬停组件（不拥有）
};
} // namespace ui

#endif //MINECRAFT_CPP_UIROOT_H
