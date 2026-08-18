#include "MeshBuilder.h"

#include <array>

#include "Mesh.h"

namespace mymc
{
void MeshBuilder::addMesh(const std::vector<float>& vertices,
                          const std::vector<uint32_t>& indices,
                          const VertexLayout& layout,
                          const glm::vec3& offset)
{
    if (vertices.empty())
    {
        return;
    }

    // 首次添加时设置布局
    if (!m_layoutSet)
    {
        m_layout = layout;
        m_floatsPerVertex = layout.stride / sizeof(float);
        m_layoutSet = true;
    }

    // 偏移量为零时直接追加，无需逐顶点处理
    const bool needOffset = (offset.x != 0.0f || offset.y != 0.0f || offset.z != 0.0f);
    const uint32_t vertexOffset = static_cast<uint32_t>(m_vertices.size() / m_floatsPerVertex);

    if (!needOffset)
    {
        // 无偏移：直接拼接顶点数据
        m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

        // 索引加上偏移后追加
        m_indices.reserve(m_indices.size() + indices.size());
        for (const auto& idx : indices)
        {
            m_indices.push_back(idx + vertexOffset);
        }
    }
    else
    {
        // 有偏移：逐顶点处理，只偏移 position 分量
        const size_t vertexCount = vertices.size() / m_floatsPerVertex;
        m_vertices.reserve(m_vertices.size() + vertices.size());

        for (size_t v = 0; v < vertexCount; ++v)
        {
            const size_t base = v * m_floatsPerVertex;

            // 复制整个顶点
            m_vertices.insert(m_vertices.end(),
                              vertices.begin() + base,
                              vertices.begin() + base + m_floatsPerVertex);

            // 偏移 position（假设 position 是前 3 个 float）
            m_vertices[m_vertices.size() - m_floatsPerVertex + 0] += offset.x;
            m_vertices[m_vertices.size() - m_floatsPerVertex + 1] += offset.y;
            m_vertices[m_vertices.size() - m_floatsPerVertex + 2] += offset.z;
        }

        // 索引加上偏移后追加
        m_indices.reserve(m_indices.size() + indices.size());
        for (const auto& idx : indices)
        {
            m_indices.push_back(idx + vertexOffset);
        }
    }
}

void MeshBuilder::addMeshDeduplicated(const std::vector<float>& vertices,
                                      const std::vector<uint32_t>& indices,
                                      const VertexLayout& layout,
                                      const glm::vec3& offset)
{
    if (vertices.empty())
    {
        return;
    }

    // 首次添加时设置布局
    if (!m_layoutSet)
    {
        m_layout = layout;
        m_floatsPerVertex = layout.stride / sizeof(float);
        m_layoutSet = true;
    }

    // 定位 position / normal / UV 在顶点中的偏移（float 单位）
    size_t posOffset = 0;
    size_t normalOffset = 0;
    size_t uvOffset = 0;
    for (const auto& elem : layout.elements)
    {
        if (elem.index == VertexElement::PositionIndex)
        {
            posOffset = elem.offset / sizeof(float);
        }
        else if (elem.index == VertexElement::NormalIndex)
        {
            normalOffset = elem.offset / sizeof(float);
        }
        else if (elem.index == VertexElement::UVIndex)
        {
            uvOffset = elem.offset / sizeof(float);
        }
    }

    const size_t vertexCount = vertices.size() / m_floatsPerVertex;
    const uint32_t baseIndex = static_cast<uint32_t>(m_vertices.size() / m_floatsPerVertex);

    // 去重哈希表：相同 key 的顶点只保留一份
    std::unordered_map<VertexKey, uint32_t, VertexKeyHash> vertexMap;
    vertexMap.reserve(vertexCount);

    // 为每个索引找到（或创建）对应的合并后顶点索引
    std::vector<uint32_t> indexRemap;
    indexRemap.reserve(indices.size());

    for (const auto& origIdx : indices)
    {
        // 提取原始顶点的 key（先临时偏移 position）
        VertexKey key = extractVertexKey(vertices, origIdx, m_floatsPerVertex,
                                         posOffset, normalOffset, uvOffset);

        // 应用位置偏移到 key 中
        key.position += offset;

        auto it = vertexMap.find(key);
        if (it != vertexMap.end())
        {
            // 已存在：复用
            indexRemap.push_back(it->second);
        }
        else
        {
            // 新顶点：追加到输出（带偏移）
            const uint32_t newIndex = baseIndex + static_cast<uint32_t>(vertexMap.size());
            vertexMap[key] = newIndex;
            indexRemap.push_back(newIndex);

            // 复制顶点数据并偏移 position
            const size_t srcBase = origIdx * m_floatsPerVertex;
            m_vertices.insert(m_vertices.end(),
                              vertices.begin() + srcBase,
                              vertices.begin() + srcBase + m_floatsPerVertex);

            // 偏移 position
            const size_t dstBase = m_vertices.size() - m_floatsPerVertex;
            m_vertices[dstBase + posOffset + 0] += offset.x;
            m_vertices[dstBase + posOffset + 1] += offset.y;
            m_vertices[dstBase + posOffset + 2] += offset.z;
        }
    }

    // 追加重映射后的索引
    m_indices.insert(m_indices.end(), indexRemap.begin(), indexRemap.end());
}

std::shared_ptr<Mesh> MeshBuilder::build()
{
    if (m_vertices.empty())
    {
        return nullptr;
    }

    auto mesh = std::make_shared<Mesh>(m_layout, m_vertices, m_indices);
    return mesh;
}

void MeshBuilder::clear()
{
    m_vertices.clear();
    m_indices.clear();
    m_layout = {};
    m_floatsPerVertex = 0;
    m_layoutSet = false;
}

MeshBuilder::VertexKey MeshBuilder::extractVertexKey(const std::vector<float>& vertexData,
                                                     const size_t vertexIndex,
                                                     const size_t floatsPerVertex,
                                                     const size_t posOffset,
                                                     const size_t normalOffset,
                                                     const size_t uvOffset)
{
    const size_t base = vertexIndex * floatsPerVertex;
    VertexKey key;
    key.position = {vertexData[base + posOffset], vertexData[base + posOffset + 1], vertexData[base + posOffset + 2]};
    key.normal = {vertexData[base + normalOffset], vertexData[base + normalOffset + 1], vertexData[base + normalOffset + 2]};
    key.uv = {vertexData[base + uvOffset], vertexData[base + uvOffset + 1]};
    return key;
}

void MeshBuilder::ensureLayout()
{
    if (m_layoutSet)
    {
        return;
    }

    // 默认布局：position(3) + color(3) + UV(2) + normal(3) = 11 floats
    m_layout.elements.push_back({
        .index = VertexElement::PositionIndex,
        .size = 3,
        .type = GL_FLOAT,
        .offset = 0
    });
    m_layout.elements.push_back({
        .index = VertexElement::ColorIndex,
        .size = 3,
        .type = GL_FLOAT,
        .offset = sizeof(float) * 3
    });
    m_layout.elements.push_back({
        .index = VertexElement::UVIndex,
        .size = 2,
        .type = GL_FLOAT,
        .offset = sizeof(float) * 6
    });
    m_layout.elements.push_back({
        .index = VertexElement::NormalIndex,
        .size = 3,
        .type = GL_FLOAT,
        .offset = sizeof(float) * 8
    });
    m_layout.stride = sizeof(float) * 11;
    m_floatsPerVertex = 11;
    m_layoutSet = true;
}

void MeshBuilder::addGreedyQuad(const int x, const int y, const int z,
                                 const int size1, const int size2,
                                 const Face face)
{
    ensureLayout();

    glm::vec3 normal(0.0f);
    std::array<std::array<float, 3>, 4> positions{};

    // 方块坐标是中心点，顶点偏移 ±0.5
    const float cx = static_cast<float>(x) - 0.5f;
    const float cy = static_cast<float>(y) - 0.5f;
    const float cz = static_cast<float>(z) - 0.5f;
    const float w = static_cast<float>(size1);
    const float h = static_cast<float>(size2);

    // 所有面的顶点顺序：与原始 CreateBox 一致
    switch (face)
    {
    case Face::Y_PLUS: // 顶面
        normal = {0.0f, 1.0f, 0.0f};
        positions = {{
            {{cx + w, cy + 1.0f, cz + h}},  // 0
            {{cx,     cy + 1.0f, cz + h}},  // 1
            {{cx,     cy + 1.0f, cz}},       // 2
            {{cx + w, cy + 1.0f, cz}}        // 3
        }};
        break;

    case Face::Y_MINUS: // 底面
        normal = {0.0f, -1.0f, 0.0f};
        positions = {{
            {{cx + w, cy, cz}},       // 0
            {{cx,     cy, cz}},       // 1
            {{cx,     cy, cz + h}},   // 2
            {{cx + w, cy, cz + h}}    // 3
        }};
        break;

    case Face::Z_MINUS: // 前面 (z-)
        normal = {0.0f, 0.0f, -1.0f};
        positions = {{
            {{cx,     cy,     cz}},   // 0
            {{cx + w, cy,     cz}},   // 1
            {{cx + w, cy + h, cz}},   // 2
            {{cx,     cy + h, cz}}    // 3
        }};
        break;

    case Face::Z_PLUS: // 后面 (z+)
        normal = {0.0f, 0.0f, 1.0f};
        positions = {{
            {{cx + w, cy,     cz + 1.0f}},  // 0
            {{cx,     cy,     cz + 1.0f}},  // 1
            {{cx,     cy + h, cz + 1.0f}},  // 2
            {{cx + w, cy + h, cz + 1.0f}}   // 3
        }};
        break;

    case Face::X_PLUS: // 右面 (x+)
        normal = {1.0f, 0.0f, 0.0f};
        positions = {{
            {{cx + 1.0f, cy,     cz + w}},  // 0
            {{cx + 1.0f, cy,     cz}},       // 1
            {{cx + 1.0f, cy + h, cz}},       // 2
            {{cx + 1.0f, cy + h, cz + w}}    // 3
        }};
        break;

    case Face::X_MINUS: // 左面 (x-)
        normal = {-1.0f, 0.0f, 0.0f};
        positions = {{
            {{cx, cy,     cz}},       // 0
            {{cx, cy,     cz + w}},   // 1
            {{cx, cy + h, cz + w}},   // 2
            {{cx, cy + h, cz}}        // 3
        }};
        break;
    }

    // UV 坐标（与原始 CreateBox 一致）
    const std::array<std::array<float, 2>, 4> uvs = {{
        {{w, h}},   // 0
        {{0, h}},   // 1
        {{0, 0}},   // 2
        {{w, 0}}    // 3
    }};

    const uint32_t base = static_cast<uint32_t>(getVertexCount());

    for (int i = 0; i < 4; ++i)
    {
        m_vertices.push_back(positions[i][0]);
        m_vertices.push_back(positions[i][1]);
        m_vertices.push_back(positions[i][2]);
        m_vertices.push_back(1.0f);
        m_vertices.push_back(1.0f);
        m_vertices.push_back(1.0f);
        m_vertices.push_back(uvs[i][0]);
        m_vertices.push_back(uvs[i][1]);
        m_vertices.push_back(normal.x);
        m_vertices.push_back(normal.y);
        m_vertices.push_back(normal.z);
    }

    // CCW 三角形
    m_indices.push_back(base + 0);
    m_indices.push_back(base + 1);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 0);
    m_indices.push_back(base + 2);
    m_indices.push_back(base + 3);
}
} // namespace mymc
