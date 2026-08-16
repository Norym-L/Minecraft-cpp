/**
 * @file RenderQueue.h
 * @brief 渲染队列模块：收集并排序渲染命令，统一执行 3D/2D/UI 场景绘制。
 */
#pragma once

#include "Common.h"
#include "glm/ext/vector_float2.hpp"
#include "graphics/Texture.h"
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include "graphics/ShaderProgram.h"

namespace mymc
{
class Mesh;
class Material;
class GraphicsAPI;
class Texture;
class ShaderProgram;

/// @brief 3D 渲染命令，描述一次绘制所需的网格、材质与模型矩阵。
struct RenderCommand
{
    Mesh *    mesh     = nullptr; // 待绘制的网格
    Material *material = nullptr; // 使用的材质
    glm::mat4 model_matrix{};     // 模型变换矩阵
};

/// @brief 2D 渲染命令，描述一个矩形的变换、贴图、颜色与 UV。
struct RenderCommand2D
{
    glm::mat4 model_matrix;      // 模型变换矩阵
    Texture * texture = nullptr; // 使用的贴图
    glm::vec4 color;             // 颜色
    glm::vec2 size;              // 尺寸
    glm::vec2 lower_left_uv;     // 左下角 UV
    glm::vec2 upper_right_uv;    // 右上角 UV
    glm::vec2 pivot;             // 中心点
};

/// @brief UI 渲染命令，描述一次 UI 批量绘制所需的网格、着色器与批次。
struct RenderCommandUI
{
    Mesh *         mesh;           // 待绘制的网格
    ShaderProgram *shader_program; // 使用的着色器程序
    size_t         screen_width;   // 屏幕宽度
    size_t         screen_height;  // 屏幕高度
};

/// @brief 渲染队列类，用于存储和管理 3D/2D/UI 渲染命令并统一执行绘制。
class RenderQueue
{
public:
    /// @brief 初始化渲染队列（创建内部 2D 平面网格）。
    void init();

    /**
         * @brief 提交 3D 渲染命令到队列。
         * @param command 渲染命令
         */
    void submit(const RenderCommand &command);

    /**
         * @brief 提交 2D 渲染命令到队列。
         * @param command 渲染命令
         */
    void submit(const RenderCommand2D &command);

    /**
         * @brief 提交 UI 渲染命令到队列。
         * @param command 渲染命令
         */
    void submit(const RenderCommandUI &command);

    /**
         * @brief 执行所有队列中的渲染命令并清空队列。
         * @param graphicsAPI 图形 API
         * @param camera_data 相机数据
         */
    void draw(
        GraphicsAPI &     graphicsAPI,
        const CameraData &camera_data); // TODO 增加一个线框渲染模式

private:
    std::vector<RenderCommand>   m_commands;   ///< 3D 渲染命令队列
    std::vector<RenderCommand2D> m_commands2D; ///< 2D 渲染命令队列
    std::vector<RenderCommandUI> m_commandsUI; ///< UI 渲染命令队列
    std::shared_ptr<Mesh>        m_mesh2D;     ///< 2D 绘制复用的平面网格
};
}
