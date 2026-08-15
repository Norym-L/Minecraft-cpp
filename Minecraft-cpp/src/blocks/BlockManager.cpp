#include "BlockManager.h"
#include "core/Core.h"

namespace block
{
    BlockManager::BlockManager() = default;

    void BlockManager::init() // TODO 完善注册结构
    {
        const auto mesh = mymc::Mesh::CreateBox(glm::vec3(1.0f));
        const auto material = std::make_shared<mymc::Material>();
        material->setShaderProgram(mymc::Core::GetInstance().getGraphicsAPI().getDefaultShaderProgram());

        registerBlock("testBlovc", mesh, material, nullptr);
    }

    BlockManager& BlockManager::GetInstance()
    {
        static BlockManager instance;
        return instance;
    }

    void BlockManager::update(float delta_time)
    {
        for (auto it = m_blockMap.begin(); it != m_blockMap.end(); ++it)
        {
            const auto block = it->second.get();
            mymc::RenderCommand command = {
                .mesh = block->m_mesh.get(),
                .material = block->m_material.get(),
                .model_matrix = glm::mat4(1.0f)
            };
            mymc::Core::GetInstance().getRanderQueue().submit(command);
        }
    }

    void BlockManager::registerBlock(std::unique_ptr<Block> block)
    {
        ++m_next_id;
        m_blockMap[m_next_id] = std::move(block);
    }
} // block