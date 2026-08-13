#include "Material.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"

#include <nlohmann/json.hpp>

namespace mymc
{
	void Material::setShaderProgram(std::shared_ptr<ShaderProgram> shader_program)
	{
		m_shaderProgram = shader_program;
	}

	ShaderProgram* Material::getShaderProgram()
	{
		return m_shaderProgram.get();
	}

	void Material::setParam(const std::string& name, float value)
	{
		m_floatParams[name] = value;
	}

	void Material::setParam(const std::string& name, float v0, float v1)
	{
		m_float2Params[name] = { v0, v1 };
	}

	void Material::setParam(const std::string& name, const glm::vec3& value)
	{
		m_float3Params[name] = value;
	}

	void Material::setParam(const std::string& name, const std::shared_ptr<Texture>& texture)
	{
		m_textures[name] = texture;
	}

	void Material::bind()
	{
		if (!m_shaderProgram)
		{
			return;
		}

		m_shaderProgram->bind();

		//通知着色器是否使用贴图（无贴图时采样器不绑定，避免残留）
		m_shaderProgram->setUniform("uUseTexture", m_textures.empty() ? 0 : 1);

		for (auto& param : m_floatParams)
		{
			m_shaderProgram->setUniform(param.first, param.second);
		}

		for (auto& param : m_float2Params)
		{
			m_shaderProgram->setUniform(param.first, param.second.first, param.second.second);
		}
		
		for (auto& param : m_float3Params)
		{
			m_shaderProgram->setUniform(param.first, param.second);
		}

		for (auto& param : m_textures)
		{
			m_shaderProgram->setTexture(param.first, param.second.get());
		}
	}
}