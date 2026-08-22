/**
 * @file MeshBuilder.h
 * @brief 网格构建器：用于合并多个网格为一个大网格，支持顶点去重和位置偏移，以及 Greedy Meshing。
 */
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "graphics/VertexLayout.h"

namespace mymc
{
class Mesh;

/// @brief 面方向枚举
enum class Face
{
    X_MINUS, // 左面
    X_PLUS,  // 右面
    Y_MINUS, // 底面
    Y_PLUS,  // 顶面
    Z_MINUS, // 前面
    Z_PLUS   // 后面
};

/// @brief 网格构建器，用于合并多个网格为一个大网格。
class MeshBuilder
{
public:
    /**
     * @brief 添加网格（不做顶点去重，仅偏移位置）。
     * @param vertices 顶点数据（交错排列的浮点数组）
     * @param indices  索引数据
     * @param layout   顶点布局
     * @param offset   世界坐标偏移
     */
    void addMesh(const std::vector<float>& vertices,
                 const std::vector<uint32_t>& indices,
                 const VertexLayout& layout,
                 const glm::vec3& offset);

    /**
     * @brief 添加网格（自动去重相同顶点）。
     * @param vertices 顶点数据（交错排列的浮点数组）
     * @param indices  索引数据
     * @param layout   顶点布局
     * @param offset   世界坐标偏移
     */
    void addMeshDeduplicated(const std::vector<float>& vertices,
                             const std::vector<uint32_t>& indices,
                             const VertexLayout& layout,
                             const glm::vec3& offset);

    /**
     * @brief 添加一个 Greedy Meshing 的大矩形面。
     * @param x      方块x坐标
     * @param y      方块y坐标
     * @param z      方块z坐标
     * @param size1  第一维大小（宽度）
     * @param size2  第二维大小（高度）
     * @param face   面方向
     * @param uv_min 图集 UV 矩形左上角（归一化）
     * @param uv_size 图集 UV 矩形宽高（归一化）
     */
    void addGreedyQuad(int x, int y, int z, int size1, int size2, Face face,
                       const glm::vec2 &uv_min, const glm::vec2 &uv_size);

    /**
     * @brief 构建合并后的网格。
     * @return 合并后的网格
     */
    std::shared_ptr<Mesh> build();

    /// @brief 清空所有累积的数据。
    void clear();

    /// @brief 获取当前累积的顶点数量。
    [[nodiscard]] size_t getVertexCount() const
    {
        return m_floatsPerVertex == 0 ? 0 : m_vertices.size() / m_floatsPerVertex;
    }

    /// @brief 获取当前累积的索引数量。
    [[nodiscard]] size_t getIndexCount() const { return m_indices.size(); }

private:
    /// @brief 顶点去重用的哈希键（位置 + 法线 + UV）。
    struct VertexKey
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        bool operator==(const VertexKey& other) const
        {
            return position == other.position && normal == other.normal && uv == other.uv;
        }
    };

    /// @brief VertexKey 的哈希函数。
    struct VertexKeyHash
    {
        size_t operator()(const VertexKey& key) const
        {
            size_t h = 0;
            // 位置
            h ^= std::hash<float>{}(key.position.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>{}(key.position.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>{}(key.position.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
            // 法线
            h ^= std::hash<float>{}(key.normal.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>{}(key.normal.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>{}(key.normal.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
            // UV
            h ^= std::hash<float>{}(key.uv.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<float>{}(key.uv.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    /**
     * @brief 从顶点数组中提取指定偏移处的 position/normal/UV。
     * @param vertexData 顶点数据数组
     * @param vertexIndex 顶点索引（第几个顶点）
     * @param floatsPerVertex 每个顶点的 float 数
     * @param posOffset position 在顶点中的起始偏移（float 单位）
     * @param normalOffset normal 在顶点中的起始偏移（float 单位）
     * @param uvOffset UV 在顶点中的起始偏移（float 单位）
     * @return VertexKey
     */
    static VertexKey extractVertexKey(const std::vector<float>& vertexData,
                                      size_t vertexIndex,
                                      size_t floatsPerVertex,
                                      size_t posOffset,
                                      size_t normalOffset,
                                      size_t uvOffset);

    /// @brief 确保布局已初始化（首次使用时设置默认布局）。
    void ensureLayout();

private:
    std::vector<float> m_vertices;       ///< 合并后的顶点数据
    std::vector<uint32_t> m_indices;     ///< 合并后的索引数据
    VertexLayout m_layout;               ///< 顶点布局（从第一个添加的网格确定）
    size_t m_floatsPerVertex = 0;        ///< 每个顶点的 float 数
    bool m_layoutSet = false;            ///< 布局是否已设置
};
} // namespace mymc
