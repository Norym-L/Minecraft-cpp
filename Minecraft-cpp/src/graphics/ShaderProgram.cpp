#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"

namespace mymc
{
    ShaderProgram::ShaderProgram(const GLuint shader_rogram_id)
        : m_shaderProgramID(shader_rogram_id)
    {}

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_shaderProgramID);
    }

    void ShaderProgram::bind()
    {
        glUseProgram(m_shaderProgramID);
        m_currentTextureUnit = 0;
    }

    GLint ShaderProgram::getUniformLocation(const std::string &name)
    {
        if (const auto it = m_uniformLocationCache.find(name); it != m_uniformLocationCache.end())
        {
            return static_cast<GLint>(it->second);
        }

        const GLint location = glGetUniformLocation(m_shaderProgramID, name.c_str());
        m_uniformLocationCache[name] = location;
        return location;
    }

    void ShaderProgram::setUniform(const std::string &name, const float value)
    {
        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }
        glUniform1f(location, value);
    }

    void ShaderProgram::setUniform(const std::string &name, const int value)
    {
        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }
        glUniform1i(location, value);
    }

    void ShaderProgram::setUniform(const std::string &name, const float v0, const float v1)
    {
        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }
        glUniform2f(location, v0, v1);
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::mat4 &mat)
    {
        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::vec3 &value)
    {
        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const std::string &name, const glm::vec4 &value)
    {
        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setTexture(const std::string &name, const Texture* texture)
    {
        if (!texture)
        {
            return;
        }

        const auto location = getUniformLocation(name);
        if (location < 0)
        {
            return;
        }

        glActiveTexture(GL_TEXTURE0 + m_currentTextureUnit);
        glBindTexture(GL_TEXTURE_2D, texture->getID());
        glUniform1i(location, m_currentTextureUnit);
        ++m_currentTextureUnit;
    }
}
