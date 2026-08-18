#include "block.h"

#include <utility>

namespace block
{
Block::Block(
    const std::uint64_t id,
    std::string name,
    const std::shared_ptr<mymc::Material> &material,
    const std::array<std::shared_ptr<mymc::Texture>, 6> &texture)
    : m_id(id), m_name(std::move(name)), m_material(material), m_textures(texture)
{}

}
