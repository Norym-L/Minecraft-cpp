#include "RenderQueue.h"
#include "Mesh.h"
#include "Material.h"
#include "graphics/GraphicsAPI.h"
#include "graphics/ShaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>

namespace mymc
{
void RenderQueue::init()
{
    m_mesh2D = Mesh::CreatePlane();
}

void RenderQueue::submit(const RenderCommand &command)
{
    m_commands.push_back(command);
}

void RenderQueue::submit(const std::vector<RenderCommand>& commands)
{
    for (auto command: commands)
    {
        m_commands.push_back(command);
    }
}

void RenderQueue::submit(const RenderCommand2D &command)
{
    m_commands2D.push_back(command);
}

void RenderQueue::submit(const RenderCommandUI &command)
{
    m_commandsUI.push_back(command);
}

void RenderQueue::draw(
    GraphicsAPI &     graphicsAPI,
    const CameraData &camera_data)
{
    // 3D
    for (auto &[mesh, material, model_matrix]: m_commands)
    {
        if (!material)
        {
            continue;
        }
        GraphicsAPI::BindMaterial(material);
        const auto shader_program = material->getShaderProgram();
        if (!shader_program)
        {
            continue;
        }
        //  临时光照
        shader_program->setUniform("uLight.color", glm::vec3(1.0f));
        shader_program->setUniform("uLight.direction", glm::vec3(-0.5f, -1.0f, -0.3f));
        shader_program->setUniform("uUseTexture", 0);

        shader_program->setUniform("uModel", model_matrix);
        shader_program->setUniform("uView", camera_data.view_matrix);
        shader_program->setUniform("uProjection", camera_data.project_matrix);
        shader_program->setUniform("uCameraPos", camera_data.position);

        //CPU 端计算法线矩阵（模型矩阵的逆转置的左上 3x3）
        glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
        shader_program->setUniform("uNormalMatrix", normal_matrix);

        GraphicsAPI::BindMesh(mesh);
        GraphicsAPI::DrawMesh(mesh);
        GraphicsAPI::UnbindMesh(mesh);
    }

    m_commands.clear();
}
}
