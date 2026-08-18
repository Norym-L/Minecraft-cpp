/**
 * @file ShaderProgram.h
 * @brief 着色器程序模块：封装编译链接后的 GLSL 程序，负责 uniform 参数设置与贴图采样器绑定。
 */
#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>

namespace mymc
{
class Texture;

/// @brief 着色器程序类，封装编译并链接后的 GLSL 程序，负责 uniform 设置与贴图绑定。
class ShaderProgram
{
public:
    ShaderProgram() = delete;

    ShaderProgram(const ShaderProgram &) = delete;

    ShaderProgram &operator=(const ShaderProgram &) = delete;

    /**
		 * @brief 用已有的着色器程序句柄构造对象。
		 * @param shader_rogram_id OpenGL 着色器程序句柄
		 */
    explicit ShaderProgram(GLuint shader_rogram_id);

    /// @brief 析构函数，释放着色器程序。
    ~ShaderProgram();

    /// @brief 绑定当前着色器程序。
    void bind();

    /**
		 * @brief 获取 uniform 变量的位置（带缓存）。
		 * @param name uniform 变量名
		 * @return GLint uniform 位置；未找到返回 -1
		 */
    GLint getUniformLocation(const std::string &name);

    /**
		 * @brief 设置一维浮点 uniform。
		 * @param name  uniform 变量名
		 * @param value 浮点值
		 */
    void setUniform(const std::string &name, float value);

    /**
		 * @brief 设置整型 uniform。
		 * @param name  uniform 变量名
		 * @param value 整型值
		 */
    void setUniform(const std::string &name, int value);

    /**
		 * @brief 设置二维浮点 uniform。
		 * @param name uniform 变量名
		 * @param v0   第一个分量
		 * @param v1   第二个分量
		 */
    void setUniform(const std::string &name, float v0, float v1);

    /**
		 * @brief 设置 4x4 矩阵 uniform。
		 * @param name  uniform 变量名
		 * @param mat  4x4 矩阵
		 */
    void setUniform(const std::string &name, const glm::mat4 &mat);

    /**
		 * @brief 设置 3x3 矩阵 uniform（法线矩阵等）。
		 * @param name  uniform 变量名
		 * @param mat  3x3 矩阵
		 */
    void setUniform(const std::string &name, const glm::mat3 &mat);

    /**
		 * @brief 设置 vec3 向量 uniform。
		 * @param name  uniform 变量名
		 * @param value vec3 向量
		 */
    void setUniform(const std::string &name, const glm::vec3 &value);

    /**
		 * @brief 设置 vec4 向量 uniform。
		 * @param name  uniform 变量名
		 * @param value vec4 向量
		 */
    void setUniform(const std::string &name, const glm::vec4 &value);

    /**
		 * @brief 将贴图绑定到指定 uniform 采样器。
		 * @param name    uniform 采样器名
		 * @param texture 贴图指针
		 */
    void setTexture(const std::string &name, const Texture *texture);

private:
    std::unordered_map<std::string, GLuint> m_uniformLocationCache;   ///< uniform 位置缓存
    GLuint                                  m_shaderProgramID    = 0; ///< 着色器程序句柄
    int                                     m_currentTextureUnit = 0; ///< 当前贴图单元索引
};
}

