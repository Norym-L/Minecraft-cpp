
#include "UiRoot.h"

#include <vector>

#include "UiBatchBuilder.h"
#include "core/Core.h"
#include "font/BitmapFont.h"
#include "graphics/VertexLayout.h"
#include "render/Mesh.h"
#include "render/RenderQueue.h"

#include <GLFW/glfw3.h> // 必须在 glew 之后（Core.h 链已引入 glew）

namespace ui
{
UiRoot::UiRoot(const int screen_width, const int screen_height)
    : m_screenWidth(screen_width), m_screenHeight(screen_height)
{
    m_root = std::make_unique<UiComponent>(); // 根级容器节点
}

UiRoot::~UiRoot() = default;

void UiRoot::init()
{
    // 字体：默认内置位图字体，GL 就绪后创建字形纹理
    if (!m_font)
    {
        m_font = std::make_unique<BitmapFont>();
    }
    if (auto *bitmap = dynamic_cast<BitmapFont *>(m_font.get()))
    {
        bitmap->ensureTexture();
    }
    m_fontInitialized = true;

    // UI 顶点布局：pos(2) + color(4) + uv(2)，与 UI 着色器 attribute 0/1/2 对齐
    mymc::VertexLayout layout;
    layout.elements = {
        {0, 2, GL_FLOAT, 0},  // position
        {1, 4, GL_FLOAT, 8},  // color
        {2, 2, GL_FLOAT, 24}, // uv
    };
    layout.stride = 32;
    m_mesh = std::make_unique<mymc::Mesh>(layout, std::vector<float>{}, std::vector<uint32_t>{});
}

IFont *UiRoot::getFont() const
{
    return m_font.get();
}

void UiRoot::setFont(std::unique_ptr<IFont> font)
{
    m_font = std::move(font);
}

void UiRoot::addChild(std::unique_ptr<UiComponent> child)
{
    if (!child)
    {
        return;
    }
    m_root->addChild(std::move(child));
}

void UiRoot::setWindowSize(const int width, const int height)
{
    m_screenWidth  = width;
    m_screenHeight = height;
}

glm::vec2 UiRoot::toUiSpace(const glm::vec2 &mouse_pos) const
{
    // GLFW 鼠标坐标左上原点 → UI 正交投影左下原点（y 翻转）
    return glm::vec2(mouse_pos.x, static_cast<float>(m_screenHeight) - mouse_pos.y);
}

void UiRoot::routeInput()
{
    auto &input = mymc::Core::GetInstance().getInputManager();

    // 光标锁定（游戏模式）：不路由鼠标交互，清空悬停态
    if (!input.isMousePositionChangedEventEnable())
    {
        if (m_hover)
        {
            m_hover->onMouseLeave();
            m_hover = nullptr;
        }
        return;
    }

    const glm::vec2 mouse = toUiSpace(input.getMousePositionCurrent());

    // 悬停跟踪：鼠标移动或首次进入时刷新命中
    if (input.isMousePositionChanged() || m_hover == nullptr)
    {
        UiComponent *hit = m_root ? hitTest(mouse, *m_root) : nullptr;
        if (hit != m_hover)
        {
            if (m_hover)
            {
                m_hover->onMouseLeave();
            }
            m_hover = hit;
            if (m_hover)
            {
                m_hover->onMouseEnter();
            }
        }
    }

    // 左键点击：分发给当前悬停的最深层组件
    if (m_hover && input.wasMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        m_hover->onMouseClick(mouse);
    }
}

void UiRoot::updateRecursive(UiComponent &node, const float delta_time)
{
    node.initializeIfNeeded();
    node.update(delta_time);
    for (auto &child: node.getChildren())
    {
        updateRecursive(*child, delta_time);
    }
}

void UiRoot::update(const float delta_time)
{
    routeInput();
    if (m_root)
    {
        updateRecursive(*m_root, delta_time);
    }
}

UiComponent *UiRoot::hitTest(const glm::vec2 &point, UiComponent &node) const
{
    // 逆序检查子组件（后加入者在上层），命中则向深层递归
    const auto &children = node.getChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        if (!(*it)->contains(point))
        {
            continue;
        }
        if (UiComponent *deep = hitTest(point, **it))
        {
            return deep;
        }
        return it->get();
    }
    return node.contains(point) ? &node : nullptr;
}

void UiRoot::emitRecursive(UiBatchBuilder &buffer, const UiComponent &node) const
{
    node.emit(buffer);
    for (const auto &child: node.getChildren())
    {
        emitRecursive(buffer, *child);
    }
}

void UiRoot::render()
{
    if (!m_mesh || !m_root)
    {
        return;
    }

    UiBatchBuilder builder;
    emitRecursive(builder, *m_root);

    std::vector<float>         vertices;
    std::vector<uint32_t>      indices;
    std::vector<mymc::UIBatch> batches;
    builder.build(vertices, indices, batches);

    m_mesh->updateDynamic(vertices, indices);

    auto &core = mymc::Core::GetInstance();
    mymc::RenderCommandUI command{
        .mesh           = m_mesh.get(),
        .shader_program = core.getGraphicsAPI().getDefaultUIShaderProgram().get(),
        .screen_width   = static_cast<size_t>(m_screenWidth),
        .screen_height  = static_cast<size_t>(m_screenHeight),
        .batches        = std::move(batches),
    };
    core.getRanderQueue().submit(command);
}
} // namespace ui