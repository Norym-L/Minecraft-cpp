/**
 * @file Material.h
 * @brief 材质模块：封装着色器程序、浮点参数与贴图，定义物体表面的渲染属性，并提供 JSON 材质加载。
 */
#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include <glm/vec3.hpp>

namespace mymc
{
	class ShaderProgram;
	class Texture;

	/// @brief 材质类，封装着色器程序、浮点参数与贴图，定义物体表面的渲染属性。
	class Material
	{
	public:
		/**
		 * @brief 设置材质的着色器程序。
		 * @param shader_program 着色器程序的智能指针
		 */
		void setShaderProgram(const std::shared_ptr<ShaderProgram> &shader_program);

		/**
		 * @brief 获取着色器程序。
		 * @return ShaderProgram* 着色器程序
		 */
		ShaderProgram* getShaderProgram() const;

		/**
		 * @brief 设置材质的一维浮点参数。
		 * @param name  参数名称
		 * @param value 参数值
		 */
		void setParam(const std::string& name, float value);
		/**
		 * @brief 设置材质的二维浮点参数。
		 * @param name 参数名称
		 * @param v0   第一个分量
		 * @param v1   第二个分量
		 */
		void setParam(const std::string& name, float v0, float v1);
		/**
		 * @brief 设置材质的三维浮点参数。
		 * @param name  参数名称
		 * @param value vec3 参数值
		 */
		void setParam(const std::string& name, const glm::vec3& value);

		/**
		 * @brief 设置材质的贴图。
		 * @param name  贴图名称
		 * @param texture 贴图
		 */
		void setParam(const std::string& name, const std::shared_ptr<Texture>& texture);

		/// @brief 绑定材质，激活其着色器程序并设置所有参数。
		void bind();

	private:
		std::shared_ptr<ShaderProgram> m_shaderProgram;                                // 着色器程序
		std::unordered_map<std::string, float> m_floatParams;                          // 一维浮点参数表
		std::unordered_map<std::string, std::pair<float, float>> m_float2Params;       // 二维浮点参数表
		std::unordered_map<std::string, glm::vec3> m_float3Params;                     // 三维浮点参数表
		std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;          // 贴图表
	};
}

