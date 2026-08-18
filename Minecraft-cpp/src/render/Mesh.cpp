#include "Mesh.h"

#include <array>
#include <cmath>
#include <memory>
#include <vector>

#include "MeshBuilder.h"
#include "graphics/GraphicsAPI.h"
#include "graphics/VertexLayout.h"

namespace mymc
{
Mesh::Mesh(const VertexLayout &layout, const std::vector<float> &vertices, const std::vector<uint32_t> &indices)
{
    m_vertexLayout = layout;

    // 保存 CPU 端副本（供合并使用）
    m_verticesCPU = vertices;
    m_indicesCPU = indices;

    m_VBO = GraphicsAPI::CreateVertexBuffer(vertices);
    m_EBO = GraphicsAPI::CreateIndexBuffer(indices);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    for (const auto &[index, size, type, offset]: m_vertexLayout.elements)
    {
        glVertexAttribPointer(index, static_cast<GLint>(size), type, GL_FALSE,
                              static_cast<GLint>(m_vertexLayout.stride),
                              reinterpret_cast<void*>(static_cast<uintptr_t>(offset)));
        glEnableVertexAttribArray(index);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_vertexCount = vertices.size() * sizeof(float) / m_vertexLayout.stride;
    m_indexCount = indices.size();
}

Mesh::Mesh(const VertexLayout &layout, const std::vector<float> &vertices)
{
    m_vertexLayout = layout;

    // 保存 CPU 端副本（供合并使用）
    m_verticesCPU = vertices;

    m_VBO = GraphicsAPI::CreateVertexBuffer(vertices);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    for (const auto &[index, size, type, offset]: m_vertexLayout.elements)
    {
        glVertexAttribPointer(index, static_cast<GLint>(size), type, GL_FALSE,
                              static_cast<GLint>(m_vertexLayout.stride),
                              reinterpret_cast<void*>(static_cast<uintptr_t>(offset)));
        glEnableVertexAttribArray(index);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexCount = vertices.size() * sizeof(float) / m_vertexLayout.stride;
}

Mesh::~Mesh()
{
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
    }
    if (m_VBO != 0)
    {
        glDeleteBuffers(1, &m_VBO);
    }
    if (m_EBO != 0)
    {
        glDeleteBuffers(1, &m_EBO);
    }
}

void Mesh::bind() const
{
    glBindVertexArray(m_VAO);
}

void Mesh::Unbind()
{
    glBindVertexArray(0);
}

void Mesh::draw() const
{
    if (m_indexCount > 0)
    {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
    }
}

void Mesh::DrawIndexedRange(const uint32_t start_index, const uint32_t index_count)
{
    if (index_count == 0)
    {
        return;
    }
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(index_count),
                   GL_UNSIGNED_INT,
                   reinterpret_cast<void*>(static_cast<size_t>(start_index) * sizeof(uint32_t)));
}

void Mesh::updateDynamic(const std::vector<float> &vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_vertexCount = vertices.size() * sizeof(float) / m_vertexLayout.stride;
}

void Mesh::updateDynamic(const std::vector<float> &vertices, const std::vector<uint32_t> &indices)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_vertexCount = vertices.size() * sizeof(float) / m_vertexLayout.stride;

    if (m_EBO == 0)
    {
        //首次创建 EBO：保存句柄并绑定进 VAO，供 DrawIndexedRange 使用
        m_EBO = GraphicsAPI::CreateIndexBuffer(indices);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBindVertexArray(0);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)),
                     indices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    m_indexCount = indices.size();
}

std::shared_ptr<Mesh> Mesh::CreateBox(const glm::vec3 &extents)
{
    const glm::vec3 half = extents * 0.5f;
    std::vector vertices = {
        // positions               // colors		   // UV            //法线
        half.x, half.y, half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.y, 0.0f, 0.0f, 1.0f, // 正面
        -half.x, half.y, half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, 0.0f, 0.0f, 1.0f,
        -half.x, -half.y, half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        half.x, -half.y, half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, 0.0f, 1.0f,

        half.x, half.y, -half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.z, 0.0f, 1.0f, 0.0f, // 顶面
        -half.x, half.y, -half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.z, 0.0f, 1.0f, 0.0f,
        -half.x, half.y, half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        half.x, half.y, half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, 1.0f, 0.0f,

        half.x, half.y, -half.z, 1.0f, 0.0f, 0.0f, extents.z, extents.y, 1.0f, 0.0f, 0.0f, // 右面
        half.x, half.y, half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, 1.0f, 0.0f, 0.0f,
        half.x, -half.y, half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.z, 0.0f, 1.0f, 0.0f, 0.0f,

        -half.x, half.y, half.z, 1.0f, 0.0f, 0.0f, extents.z, extents.y, -1.0f, 0.0f, 0.0f, // 左面
        -half.x, half.y, -half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, -1.0f, 0.0f, 0.0f,
        -half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -half.x, -half.y, half.z, 1.0f, 1.0f, 0.0f, extents.z, 0.0f, -1.0f, 0.0f, 0.0f,

        half.x, -half.y, half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.z, 0.0f, -1.0f, 0.0f, // 底面
        -half.x, -half.y, half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.z, 0.0f, -1.0f, 0.0f,
        -half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, -1.0f, 0.0f,

        -half.x, half.y, -half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.y, 0.0f, 0.0f, -1.0f, // 后面
        half.x, half.y, -half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, 0.0f, 0.0f, -1.0f,
        half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, 0.0f, -1.0f,
        };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,       // 前面
        4, 5, 6, 4, 6, 7,       // 上面
        8, 9, 10, 8, 10, 11,    // 右面
        12, 13, 14, 12, 14, 15, // 左面
        16, 17, 18, 16, 18, 19, // 底面
        20, 21, 22, 20, 22, 23  // 后面
        };

    VertexLayout vertexLayout;

    // 位置
    vertexLayout.elements.push_back({
        .index = VertexElement::PositionIndex, .size = 3, .type = GL_FLOAT, .offset = 0
        });
    // 颜色
    vertexLayout.elements.push_back({
        .index = VertexElement::ColorIndex, .size = 3, .type = GL_FLOAT,
        .offset = sizeof(float) * 3
        });
    // UV
    vertexLayout.elements.push_back({
        .index = VertexElement::UVIndex, .size = 2, .type = GL_FLOAT,
        .offset = sizeof(float) * 6
        });
    // 法线
    vertexLayout.elements.push_back({
        .index = VertexElement::NormalIndex, .size = 3, .type = GL_FLOAT,
        .offset = sizeof(float) * 8
        });
    vertexLayout.stride = sizeof(float) * 11;

    return std::make_shared<Mesh>(vertexLayout, vertices, indices);
}

std::shared_ptr<Mesh> Mesh::CreateBlockByFace(const std::array<bool, 6> &faces)
{
    constexpr auto extents = glm::vec3(1.0f);
    constexpr glm::vec3 half = extents * 0.5f;

    // 每面4个顶点，每个顶点11个float（3位置+3颜色+2UV+3法线）
    static const std::vector<float> vertices_base = {
        half.x, half.y, half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.y, 0.0f, 0.0f, 1.0f, // 正面
        -half.x, half.y, half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, 0.0f, 0.0f, 1.0f,
        -half.x, -half.y, half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        half.x, -half.y, half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, 0.0f, 1.0f,

        half.x, half.y, -half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.z, 0.0f, 1.0f, 0.0f, // 顶面
        -half.x, half.y, -half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.z, 0.0f, 1.0f, 0.0f,
        -half.x, half.y, half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        half.x, half.y, half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, 1.0f, 0.0f,

        half.x, half.y, -half.z, 1.0f, 0.0f, 0.0f, extents.z, extents.y, 1.0f, 0.0f, 0.0f, // 右面
        half.x, half.y, half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, 1.0f, 0.0f, 0.0f,
        half.x, -half.y, half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.z, 0.0f, 1.0f, 0.0f, 0.0f,

        -half.x, half.y, half.z, 1.0f, 0.0f, 0.0f, extents.z, extents.y, -1.0f, 0.0f, 0.0f, // 左面
        -half.x, half.y, -half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, -1.0f, 0.0f, 0.0f,
        -half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -half.x, -half.y, half.z, 1.0f, 1.0f, 0.0f, extents.z, 0.0f, -1.0f, 0.0f, 0.0f,

        half.x, -half.y, half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.z, 0.0f, -1.0f, 0.0f, // 底面
        -half.x, -half.y, half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.z, 0.0f, -1.0f, 0.0f,
        -half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, -1.0f, 0.0f,

        -half.x, half.y, -half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.y, 0.0f, 0.0f, -1.0f, // 后面
        half.x, half.y, -half.z, 0.0f, 1.0f, 0.0f, 0.0f, extents.y, 0.0f, 0.0f, -1.0f,
        half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f, 0.0f, 0.0f, -1.0f,
        };

    constexpr int FACE_VERTEX_COUNT = 4;
    constexpr int FLOAT_PER_VERTEX = 11;
    constexpr int FLOAT_PER_FACE = FACE_VERTEX_COUNT * FLOAT_PER_VERTEX;

    // 面顺序: x-, x+, y-, y+, z-, z+
    static const int faceOffsets[6] = {3, 2, 4, 1, 5, 0};

    std::vector<float> vertices;
    vertices.reserve(6 * FLOAT_PER_FACE); // 最多6个面

    // 按照 faces 数组顺序添加面
    for (int i = 0; i < 6; ++i)
    {
        if (faces[i])
        {
            const int start = faceOffsets[i] * FLOAT_PER_FACE;
            const int end = start + FLOAT_PER_FACE;
            vertices.insert(vertices.end(),
                            vertices_base.begin() + start,
                            vertices_base.begin() + end);
        }
    }

    vertices.shrink_to_fit();

    // 构造索引
    const int vertexCount = static_cast<int>(vertices.size() / FLOAT_PER_VERTEX);
    std::vector<unsigned int> indices;
    indices.reserve(vertexCount * 6); // 每个顶点6个索引（2个三角形）

    for (int i = 0; i < vertexCount; i += 4)
    {
        // 每个面的两个三角形
        indices.push_back(i + 0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 0);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }

    VertexLayout vertexLayout;
    vertexLayout.elements.push_back({
        .index = VertexElement::PositionIndex,
        .size = 3,
        .type = GL_FLOAT,
        .offset = 0
        });
    vertexLayout.elements.push_back({
        .index = VertexElement::ColorIndex,
        .size = 3,
        .type = GL_FLOAT,
        .offset = sizeof(float) * 3
        });
    vertexLayout.elements.push_back({
        .index = VertexElement::UVIndex,
        .size = 2,
        .type = GL_FLOAT,
        .offset = sizeof(float) * 6
        });
    vertexLayout.elements.push_back({
        .index = VertexElement::NormalIndex,
        .size = 3,
        .type = GL_FLOAT,
        .offset = sizeof(float) * 8
        });
    vertexLayout.stride = sizeof(float) * FLOAT_PER_VERTEX;

    return std::make_shared<Mesh>(vertexLayout, vertices, indices);
}

std::shared_ptr<Mesh> Mesh::MergeMeshes(
    const std::vector<std::pair<std::shared_ptr<Mesh>, glm::vec3>>& meshes)
{
    MeshBuilder builder;
    for (const auto& [mesh, offset] : meshes)
    {
        if (mesh)
        {
            builder.addMesh(mesh->getVertices(), mesh->getIndices(),
                           mesh->getVertexLayout(), offset);
        }
    }
    return builder.build();
}

std::shared_ptr<Mesh> Mesh::MergeMeshesDeduplicated(
    const std::vector<std::pair<std::shared_ptr<Mesh>, glm::vec3>>& meshes)
{
    MeshBuilder builder;
    for (const auto& [mesh, offset] : meshes)
    {
        if (mesh)
        {
            builder.addMeshDeduplicated(mesh->getVertices(), mesh->getIndices(),
                                       mesh->getVertexLayout(), offset);
        }
    }
    return builder.build();
}

#if 0
std::shared_ptr<Mesh> Mesh::CreateSphere(const float radius, const int sectors, const int stacks)
{
    std::vector<float> vertices((stacks + 1) * (sectors + 1) * 8);
    for (int i = 0; i <= stacks; ++i)
    {
        constexpr float PI = 3.14159265358979323846f;
        const float stack_angle = PI / 2.0f - static_cast<float>(i) * (PI / static_cast<float>(stacks));
        const float xy = radius * cosf(stack_angle);
        const float z = radius * sinf(stack_angle);

        for (int j = 0; j <= sectors; ++j)
        {
            const float sector_angle = static_cast<float>(j) * (2.0f * PI / static_cast<float>(sectors));

            const float x = xy * cosf(sector_angle);
            const float y = xy * sinf(sector_angle);

            const size_t vertexStart = (i * (sectors + 1) + j) * 8;

            // 位置
            vertices[vertexStart] = x;
            vertices[vertexStart + 1] = y;
            vertices[vertexStart + 2] = z;

            // 法线（归一化位置向量）
            const float lmymcth = sqrtf(x * x + y * y + z * z);
            vertices[vertexStart + 3] = x / lmymcth;
            vertices[vertexStart + 4] = y / lmymcth;
            vertices[vertexStart + 5] = z / lmymcth;

            // UV
            vertices[vertexStart + 6] = static_cast<float>(j) / static_cast<float>(sectors);
            vertices[vertexStart + 7] = static_cast<float>(i) / static_cast<float>(stacks);
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; ++i)
    {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != stacks - 1)
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    VertexLayout vertexLayout;

    // 位置
    vertexLayout.elements.push_back({
        .index = VertexElement::PositionIndex, .size = 3, .type = GL_FLOAT, .offset = 0
        });
    // 法线
    vertexLayout.elements.push_back({
        .index = VertexElement::NormalIndex, .size = 3, .type = GL_FLOAT,
        .offset = sizeof(float) * 3
        });
    // UV
    vertexLayout.elements.push_back({
        .index = VertexElement::UVIndex, .size = 2, .type = GL_FLOAT,
        .offset = sizeof(float) * 6
        });
    vertexLayout.stride = sizeof(float) * 8;

    auto result = std::make_shared<Mesh>(vertexLayout, vertices, indices);

    return result;
}
#endif

std::shared_ptr<Mesh> Mesh::CreatePlane()
{
    std::vector vertices =
        {
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
        };

    std::vector<uint32_t> indices =
        {
        0, 1, 2,
        0, 2, 3
        };

    VertexLayout vertexLayout;

    //位置
    vertexLayout.elements.push_back({
        .index = VertexElement::PositionIndex,
        .size = 2,
        .type = GL_FLOAT,
        .offset = 0
        });
    vertexLayout.stride = sizeof(float) * 2;

    auto result = std::make_shared<Mesh>(vertexLayout, vertices, indices);

    return result;
}

} // namespace mymc
