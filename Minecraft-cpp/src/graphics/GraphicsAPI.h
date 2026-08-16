/**
 * @file GraphicsAPI.h
 * @brief 图形 API 封装模块：负责着色器程序、顶点/索引缓冲的创建与绑定，以及网格绘制等底层渲染操作。
 */
#pragma once
#include <GL/glew.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace mymc
{
class ShaderProgram;
class Material;
class Mesh;

/// @brief 混合模式。
enum class BlendMode : std::uint8_t
{
    DISABLE,  ///< 禁用混合
    ALPHA,    ///< Alpha 混合
    ADDITIVE, ///< 加法混合
    MULTIPLY  ///< 乘法混合
};

/// @brief 矩形区域。
struct Rect
{
    int x      = 0; ///< 左上角 x 坐标
    int y      = 0; ///< 左上角 y 坐标
    int width  = 0; ///< 宽度
    int height = 0; ///< 高度
};

/// @brief 着色器缓存键，由顶点与片段着色器源码组成。
struct ShaderKey
{
    std::string vertex_source;   ///< 顶点着色器源码
    std::string fragment_source; ///< 片段着色器源码

    bool operator==(const ShaderKey &other) const
    {
        return vertex_source == other.vertex_source && fragment_source == other.fragment_source;
    }
};

/// @brief ShaderKey 的哈希函数对象。
struct ShaderKeyHash
{
    std::size_t operator()(const ShaderKey &key) const
    {
        const std::size_t h1 = std::hash<std::string>{}(key.vertex_source);
        const std::size_t h2 = std::hash<std::string>{}(key.fragment_source);
        return h1 ^ (h2 << 1);
    }
};

/// @brief 图形 API 抽象层，负责着色器程序的创建与管理、缓冲区与网格绑定及渲染状态设置。
class GraphicsAPI
{
public:
    /**
         * @brief 初始化图形 API（启用深度测试）。
         * @return bool 始终返回 true
         */
    static bool Init();

    /**
         * @brief 编译顶点和片段着色器源码，并链接生成着色器程序（带缓存）。
         * @param vertex_source   顶点着色器的 GLSL 源码字符串
         * @param fragment_source 片段着色器的 GLSL 源码字符串
         * @return std::shared_ptr<ShaderProgram> 成功返回着色器程序共享指针；编译或链接失败返回 nullptr
         */
    std::shared_ptr<ShaderProgram> createShaderProgram(const std::string &vertex_source,
                                                       const std::string &fragment_source);

    /**
         * @brief 获取默认 3D 着色器（带光照），供无材质的网格使用。
         * @return const std::shared_ptr<ShaderProgram>& 默认着色器程序共享指针的引用
         */
    const std::shared_ptr<ShaderProgram> &getDefaultShaderProgram();

    /**
         * @brief 获取默认 2D 着色器程序。
         * @return const std::shared_ptr<ShaderProgram>& 默认 2D 着色器程序共享指针的引用
         */
    const std::shared_ptr<ShaderProgram> &getDefault2DShaderProgram();

    /**
         * @brief 获取默认 UI 着色器程序。
         * @return const std::shared_ptr<ShaderProgram>& 默认 UI 着色器程序共享指针的引用
         */
    const std::shared_ptr<ShaderProgram> &getDefaultUIShaderProgram();

    /**
         * @brief 创建顶点缓冲区对象（VBO）并上传顶点数据。
         * @param vertices 顶点数据的浮点数组
         * @return GLuint 生成的顶点缓冲区对象的 OpenGL 句柄
         */
    static GLuint CreateVertexBuffer(const std::vector<float> &vertices);

    /**
         * @brief 创建索引缓冲区对象（EBO）并上传索引数据。
         * @param indices 索引数据的无符号整数数组
         * @return GLuint 生成的索引缓冲区对象的 OpenGL 句柄
         */
    static GLuint CreateIndexBuffer(const std::vector<uint32_t> &indices);

    /**
         * @brief 设置清屏颜色。
         * @param r 红色分量
         * @param g 绿色分量
         * @param b 蓝色分量
         * @param a 透明度分量
         */
    static void SetClearColor(float r, float g, float b, float a);

    /// @brief 清除颜色缓冲区和深度缓冲区。
    static void ClearBuffers();

    /**
         * @brief 获取当前视口区域。
         * @return const Rect& 视口矩形
         */
    const Rect &getViewport() const;

    /**
         * @brief 设置视口区域。
         * @param x 左下角 x 坐标
         * @param y 左下角 y 坐标
         * @param width 宽度
         * @param height 高度
         */
    void setViewport(int x, int y, int width, int height);

    /**
         * @brief 启用或禁用深度测试。
         * @param enabled 为 true 表示启用
         */
    static void SetDepthTestEnabled(bool enabled);

    /**
         * @brief 设置混合模式。
         * @param mode 混合模式
         */
    static void SetBlendMode(BlendMode mode);

    /**
         * @brief 绑定着色器程序。
         * @param shader_program 需要绑定的着色器
         */
    static void BindShaderProgram(ShaderProgram *shader_program);

    /**
         * @brief 绑定材质（激活其着色器程序并设置所有参数）。
         * @param material 需要绑定的材质
         */
    static void BindMaterial(Material *material);

    /**
         * @brief 绑定网格的顶点数组对象。
         * @param mesh 需要绑定的网格
         */
    static void BindMesh(const Mesh *mesh);

    /**
         * @brief 解绑网格的顶点数组对象。
         * @param mesh 需要解绑的网格
         */
    static void UnbindMesh(const Mesh *mesh);

    /**
         * @brief 绘制网格。
         * @param mesh 需要绘制的网格
         */
    static void DrawMesh(const Mesh *mesh);

private:
    Rect m_viewport; ///< 当前视口区域

    std::shared_ptr<ShaderProgram> m_defaultShaderProgram; ///< 默认 3D 着色器程序（供无材质的网格使用）
    std::shared_ptr<ShaderProgram> m_default2DShaderProgram; ///< 默认 2D 着色器程序
    std::shared_ptr<ShaderProgram> m_defaultUIShaderProgram; ///< 默认 UI 着色器程序
    std::unordered_map<ShaderKey, std::shared_ptr<ShaderProgram>, ShaderKeyHash> m_shaderCache; ///< 着色器缓存
};
}
