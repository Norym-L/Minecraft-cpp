#include "Material.h"
#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"

#include <nlohmann/json.hpp>

namespace mymc
{
    void Material::setShaderProgram(const std::shared_ptr<ShaderProgram> &shader_program)
    {
        m_shaderProgram = shader_program;
    }

    ShaderProgram* Material::getShaderProgram() const
    {
        return m_shaderProgram.get();
    }

    void Material::setParam(const std::string &name, const float value)
    {
        m_floatParams[name] = value;
    }

    void Material::setParam(const std::string &name, float v0, float v1)
    {
        m_float2Params[name] = {v0, v1};
    }

    void Material::setParam(const std::string &name, const glm::vec3 &value)
    {
        m_float3Params[name] = value;
    }

    void Material::setParam(const std::string &name, const std::shared_ptr<Texture> &texture)
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

        for (auto &[fst, snd]: m_floatParams)
        {
            m_shaderProgram->setUniform(fst, snd);
        }

        for (auto &[fst, snd]: m_float2Params)
        {
            m_shaderProgram->setUniform(fst, snd.first, snd.second);
        }

        for (auto &[fst, snd]: m_float3Params)
        {
            m_shaderProgram->setUniform(fst, snd);
        }

        for (auto &[fst, snd]: m_textures)
        {
            m_shaderProgram->setTexture(fst, snd.get());
        }
    }
}
