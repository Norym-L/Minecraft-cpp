/**
 * @file Mesh.h
 * @brief 网格模块：封装顶点数据（VBO）、索引数据（EBO）与顶点数组对象（VAO），供 GPU 绘制使用，并提供常见几何体的静态创建。
 */
#pragma once
#include <GL/glew.h>
#include "graphics/VertexLayout.h"
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace mymc
{
/// @brief 网格类，封装顶点数据（VBO）、索引数据（EBO）与顶点数组对象（VAO），用于 GPU 绘制。
class Mesh
{
public:
    /**
         * @brief 构造带索引的网格，上传顶点与索引数据到 GPU。
         * @param layout   顶点布局
         * @param vertices 顶点数据（交错排列的浮点数组）
         * @param indices  索引数据
         */
    Mesh(const VertexLayout &layout, const std::vector<float> &vertices, const std::vector<uint32_t> &indices);

    /**
         * @brief 构造不带索引的网格，仅上传顶点数据到 GPU。
         * @param layout   顶点布局
         * @param vertices 顶点数据（交错排列的浮点数组）
         */
    Mesh(const VertexLayout &layout, const std::vector<float> &vertices);

    /// @brief 析构函数，释放 VAO/VBO/EBO 等 OpenGL 资源。
    ~Mesh();

    Mesh(const Mesh &) = delete;

    Mesh &operator=(const Mesh &) = delete;

    /// @brief 绑定网格的顶点数组对象。
    void bind() const;

    /// @brief 解绑网格的顶点数组对象。
    static void Unbind();

    /// @brief 绘制网格；存在索引时使用索引绘制，否则按顶点数组绘制。
    void draw() const;

    /**
         * @brief 按索引范围绘制网格。
         * @param start_index 起始索引
         * @param index_count  索引数量
         */
    static void DrawIndexedRange(uint32_t start_index, uint32_t index_count);

    /**
         * @brief 动态更新顶点数据（重新上传顶点缓冲）。
         * @param vertices 新的顶点数据
         */
    void updateDynamic(const std::vector<float> &vertices);

    /**
         * @brief 动态更新顶点与索引数据（重新上传顶点与索引缓冲）。
         * @param vertices 新的顶点数据
         * @param indices  新的索引数据
         */
    void updateDynamic(const std::vector<float> &vertices, const std::vector<uint32_t> &indices);

    /**
         * @brief 创建立方体。
         * @param extents 尺寸（默认 1x1x1）
         * @return std::shared_ptr<Mesh> 立方体网格对象的共享指针
         */
    static std::shared_ptr<Mesh> CreateBox(const glm::vec3 &extents = glm::vec3(1.0f));

    /**
     * 创建立方体网格
     * @param faces 需要渲染的面的数组
     * @return std::shared_ptr<Mesh> 立方体网格对象的共享指针
     */
    static std::shared_ptr<Mesh> CreateBlockByFace(const std::array<bool, 6> &faces);

    /**
     * 合并多个网格为一个大网格（不做顶点去重，仅偏移位置）。
     * @param meshes 网格列表（网格，世界坐标偏移）
     * @return 合并后的网格
     */
    static std::shared_ptr<Mesh> MergeMeshes(
        const std::vector<std::pair<std::shared_ptr<Mesh>, glm::vec3>>& meshes);

    /**
     * 合并多个网格为一个大网格（自动去重相同顶点）。
     * @param meshes 网格列表（网格，世界坐标偏移）
     * @return 合并后的网格
     */
    static std::shared_ptr<Mesh> MergeMeshesDeduplicated(
        const std::vector<std::pair<std::shared_ptr<Mesh>, glm::vec3>>& meshes);


#if 0
    /**
         * @brief 创建球体。
         * @param radius 半径
         * @param sectors 经度分段数
         * @param stacks  纬度分段数
         * @return std::shared_ptr<Mesh> 球体网格对象的共享指针
         */
    static std::shared_ptr<Mesh> CreateSphere(float radius, int sectors, int stacks);
#endif

    /**
         * @brief 创建 2D 平面（单位四边形）。
         * @return std::shared_ptr<Mesh> 平面网格对象的共享指针
         */
    static std::shared_ptr<Mesh> CreatePlane();

    /// @brief 获取顶点数据（CPU 端副本）。
    [[nodiscard]] const std::vector<float>& getVertices() const { return m_verticesCPU; }

    /// @brief 获取索引数据（CPU 端副本）。
    [[nodiscard]] const std::vector<uint32_t>& getIndices() const { return m_indicesCPU; }

    /// @brief 获取顶点布局。
    [[nodiscard]] const VertexLayout& getVertexLayout() const { return m_vertexLayout; }

private:
    VertexLayout m_vertexLayout; // 顶点布局
    GLuint m_VBO = 0;            // 顶点缓冲区对象句柄
    GLuint m_EBO = 0;            // 索引缓冲区对象句柄
    GLuint m_VAO = 0;            // 顶点数组对象句柄

    size_t m_vertexCount = 0; // 顶点数量
    size_t m_indexCount = 0;  // 索引数量

    std::vector<float> m_verticesCPU;      // CPU 端顶点数据副本（供合并使用）
    std::vector<uint32_t> m_indicesCPU;    // CPU 端索引数据副本（供合并使用）
};
} // namespace mymc
