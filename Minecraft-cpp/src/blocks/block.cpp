#include "block.h"

#include <utility>

namespace block
{
Block::Block(
    const std::uint64_t id,
    std::string name,
    const std::shared_ptr<mymc::Mesh> &mesh,
    const std::shared_ptr<mymc::Material> &material,
    const std::shared_ptr<mymc::Texture> &texture)
    : m_id(id), m_name(std::move(name)), m_mesh(mesh), m_material(material)
{
    m_textures.fill(texture);
}
}
