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

void RenderQueue::submit(const std::vector<RenderCommand> &commands)
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

void RenderQueue::draw(GraphicsAPI &graphicsAPI, const CameraData &camera_data)
{
    // 3D
    // 帧内常量：对所有命令都相同，每个 shader 程序每帧只上传一次
    const glm::vec3      light_color(1.0f);
    const glm::vec3      light_direction(-0.5f, -1.0f, -0.3f);
    const ShaderProgram *current_program = nullptr; // 已上传过帧内常量的 shader 程序

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

        // 帧内常量仅在 shader 程序第一次出现时设置（uniform 是程序状态，不会因后续 glUseProgram 丢失）
        if (shader_program != current_program)
        {
            shader_program->setUniform("uLight.color", light_color);
            shader_program->setUniform("uLight.direction", light_direction);
            shader_program->setUniform("uView", camera_data.view_matrix);
            shader_program->setUniform("uProjection", camera_data.project_matrix);
            shader_program->setUniform("uCameraPos", camera_data.position);
            // 模型矩阵为纯平移时法线矩阵 = 单位阵；必须上传，否则 GLSL 默认会得到"零矩阵"导致法线全零
            shader_program->setUniform("uNormalMatrix", glm::mat3(1.0f));
            current_program = shader_program;
        }

        // 每个命令独有的模型矩阵
        shader_program->setUniform("uModel", model_matrix);

        GraphicsAPI::BindMesh(mesh);
        GraphicsAPI::DrawMesh(mesh);
        GraphicsAPI::UnbindMesh(mesh);
    }

    m_commands.clear();
}
}
