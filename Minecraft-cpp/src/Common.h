/**
 * @file Common.h
 * @brief 引擎通用数据结构定义：包含相机数据、光照数据等跨模块共享的类型。
 */
#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

/// 定义可重复使用的数据结构
namespace mymc
{
    class Texture;

    /// @brief 相机数据，包含视图矩阵与投影矩阵。
    struct CameraData
    {
        glm::mat4 view_matrix;    ///< 视图矩阵
        glm::mat4 project_matrix; ///< 投影矩阵
        glm::mat4 ortho_martix;   ///< 2D 正交投影
        glm::vec3 position;       ///< 相机位置
    };

    /// @brief 光照数据，包含光源颜色与光源位置。
    struct LightData
    {
        glm::vec3 color;    ///< 光源颜色
        glm::vec3 position; ///< 光源位置
    };

    /// @brief UI 批次，描述一次使用相同贴图的索引绘制范围。
    struct UIBatch
    {
        Texture* texture = nullptr; ///< 批次使用的贴图
        uint32_t index_count = 0;   ///< 批次的索引数量
    };
}

