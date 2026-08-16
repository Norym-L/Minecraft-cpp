/**
 * @file VertexLayout.h
 * @brief 顶点布局模块：定义顶点属性元素与顶点布局，描述顶点数据在缓冲区中的组织方式。
 */
#pragma once
#include <GL/glew.h>
#include <vector>
#include <stdint.h>

namespace mymc
{
/// @brief 顶点属性元素，描述单个顶点属性的布局信息。
struct VertexElement
{
    GLuint   index;  ///< 顶点属性索引（attribute location）
    GLuint   size;   ///< 顶点属性大小（例如，3 表示 vec3）
    GLuint   type;   ///< 顶点属性类型（例如，GL_FLOAT）
    uint32_t offset; ///< 顶点属性在顶点结构体中的偏移量

    static constexpr int PositionIndex = 0; ///< 位置索引
    static constexpr int ColorIndex    = 1; ///< 颜色索引
    static constexpr int UVIndex       = 2; ///< UV 索引
    static constexpr int NormalIndex   = 3; ///< 法线索引
};

/// @brief 顶点布局，描述顶点数据的组织方式。
struct VertexLayout
{
    std::vector<VertexElement> elements;   ///< 顶点属性元素列表
    uint32_t                   stride = 0; ///< 单个顶点结构体的总大小（字节数，步长）
};
}

