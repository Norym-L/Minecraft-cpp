#ifndef MINECRAFT_CPP_UICOMPONENT_H
#define MINECRAFT_CPP_UICOMPONENT_H

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

namespace ui
{
class UiBatchBuilder;

/// @brief UI 组件基类：保留式组件树的节点。
/// 生命周期：init（挂载后调用一次）→ update（每帧）→ emit（每帧向批量器提交四边形）。
/// 约定：update/emit 不递归 children（递归由 UiRoot 驱动）；事件由 UiRoot 路由。
class UiComponent
{
public:
    virtual ~UiComponent() = default;

    /// @brief 挂载后初始化（UiRoot 调用一次；默认空）
    virtual void init() {}

    /// @brief 每帧更新（输入消费/状态刷新；默认空）
    virtual void update(float delta_time) {}

    /// @brief 每帧向批量构建器提交本组件的四边形（不递归 children）
    virtual void emit(UiBatchBuilder &buffer) const {}

    /// @brief 添加子组件（所有权转移）
    void addChild(std::unique_ptr<UiComponent> child);

    /// @brief 获取子组件列表
    [[nodiscard]] const std::vector<std::unique_ptr<UiComponent>> &getChildren() const;

    /// @brief 获取子组件列表（非 const：供 UiRoot 驱动 update 递归使用）
    [[nodiscard]] std::vector<std::unique_ptr<UiComponent>> &getChildren();

    /// @brief 若未初始化则调用 init() 并置位（由 UiRoot 驱动）
    void initializeIfNeeded();

    /// @brief 获取父组件（根组件为 nullptr）
    [[nodiscard]] UiComponent *getParent() const;

    /// @brief 设置位置（矩形左下角，像素空间，y 轴向上）
    void setPosition(const glm::vec2 &position);

    /// @brief 设置尺寸（像素）
    void setSize(const glm::vec2 &size);

    [[nodiscard]] const glm::vec2 &getPosition() const;

    [[nodiscard]] const glm::vec2 &getSize() const;

    /// @brief 命中测试（默认矩形包含）
    [[nodiscard]] virtual bool contains(const glm::vec2 &point) const;

    /// @brief 鼠标点击事件（UiRoot 顶层命中后分发）
    virtual void onMouseClick(const glm::vec2 &pos) {}

    /// @brief 鼠标进入事件（默认置悬停位）
    virtual void onMouseEnter();

    /// @brief 鼠标离开事件（默认清悬停位）
    virtual void onMouseLeave();

    /// @brief 当前是否悬停
    [[nodiscard]] bool isHovered() const;

protected:
    glm::vec2 m_position{0.0f};                                             // 矩形左下角（像素空间）
    glm::vec2 m_size{0.0f};                                                 // 矩形尺寸
    UiComponent *m_parent                             = nullptr;            // 父组件
    std::vector<std::unique_ptr<UiComponent>> m_children;                   // 子组件
    bool m_hovered                                                          = false; // 悬停状态
    bool m_initialized                                                     = false; // init 已调用
};
} // namespace ui

#endif //MINECRAFT_CPP_UICOMPONENT_H