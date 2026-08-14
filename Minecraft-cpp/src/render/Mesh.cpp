#include "Mesh.h"

#include <memory>
#include <vector>

#include "Core/Core.h"
#include "graphics/GraphicsAPI.h"
#include "graphics/VertexLayout.h"

namespace mymc
{
	Mesh::Mesh(const VertexLayout layout, const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
		m_vertexLayout = layout;

		auto& graphicsAPI = Core::getInstance().getGraphicsAPI();

		m_VBO = graphicsAPI.createVertexBuffer(vertices);
		m_EBO = graphicsAPI.createIndexBuffer(indices);

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		for (auto& element : m_vertexLayout.elements)
		{
			glVertexAttribPointer(element.index, element.size, element.type, GL_FALSE, m_vertexLayout.stride,
								  (void*)(uintptr_t)element.offset);
			glEnableVertexAttribArray(element.index);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_vertexCount = (vertices.size() * sizeof(float)) / m_vertexLayout.stride;
		m_indexCount = indices.size();
	}

	Mesh::Mesh(const VertexLayout layout, const std::vector<float>& vertices)
	{
		m_vertexLayout = layout;

		auto& graphicsAPI = Core::getInstance().getGraphicsAPI();

		m_VBO = graphicsAPI.createVertexBuffer(vertices);

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		for (auto& element : m_vertexLayout.elements)
		{
			glVertexAttribPointer(element.index, element.size, element.type, GL_FALSE, m_vertexLayout.stride,
								  (void*)(uintptr_t)element.offset);
			glEnableVertexAttribArray(element.index);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_vertexCount = (vertices.size() * sizeof(float)) / m_vertexLayout.stride;
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

	void Mesh::bind()
	{
		glBindVertexArray(m_VAO);
	}

	void Mesh::unbind()
	{
		glBindVertexArray(0);
	}

	void Mesh::draw()
	{
		if (m_indexCount > 0)
		{
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
		}
	}

	void Mesh::drawIndexedRange(uint32_t start_index, uint32_t index_count)
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

	void Mesh::updateDynamic(const std::vector<float>& vertices)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_vertexCount = (vertices.size() * sizeof(float)) / m_vertexLayout.stride;
	}

	void Mesh::updateDynamic(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_vertexCount = (vertices.size() * sizeof(float)) / m_vertexLayout.stride;

		if (m_EBO == 0)
		{
			//首次创建 EBO：保存句柄并绑定进 VAO，供 DrawIndexedRange 使用
			m_EBO = Core::getInstance().getGraphicsAPI().createIndexBuffer(indices);
			glBindVertexArray(m_VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBindVertexArray(0);
		}
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		m_indexCount = indices.size();
	}

	std::shared_ptr<Mesh> Mesh::CreateBox(const glm::vec3& extents)
	{
		const glm::vec3 half = extents * 0.5f;
		std::vector<float> vertices = {
			// positions               // colors		   // UV
			// //法线
			half.x,	 half.y,  half.z,  1.0f, 0.0f, 0.0f, extents.x, extents.y, 0.0f,  0.0f,	 1.0f, // 正面
			-half.x, half.y,  half.z,  0.0f, 1.0f, 0.0f, 0.0f,		extents.y, 0.0f,  0.0f,	 1.0f,
			-half.x, -half.y, half.z,  0.0f, 0.0f, 1.0f, 0.0f,		0.0f,	   0.0f,  0.0f,	 1.0f,
			half.x,	 -half.y, half.z,  1.0f, 1.0f, 0.0f, extents.x, 0.0f,	   0.0f,  0.0f,	 1.0f,

			half.x,	 half.y,  -half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.z, 0.0f,  1.0f,	 0.0f, // 顶面
			-half.x, half.y,  -half.z, 0.0f, 1.0f, 0.0f, 0.0f,		extents.z, 0.0f,  1.0f,	 0.0f,
			-half.x, half.y,  half.z,  0.0f, 0.0f, 1.0f, 0.0f,		0.0f,	   0.0f,  1.0f,	 0.0f,
			half.x,	 half.y,  half.z,  1.0f, 1.0f, 0.0f, extents.x, 0.0f,	   0.0f,  1.0f,	 0.0f,

			half.x,	 half.y,  -half.z, 1.0f, 0.0f, 0.0f, extents.z, extents.y, 1.0f,  0.0f,	 0.0f, // 右面
			half.x,	 half.y,  half.z,  0.0f, 1.0f, 0.0f, 0.0f,		extents.y, 1.0f,  0.0f,	 0.0f,
			half.x,	 -half.y, half.z,  0.0f, 0.0f, 1.0f, 0.0f,		0.0f,	   1.0f,  0.0f,	 0.0f,
			half.x,	 -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.z, 0.0f,	   1.0f,  0.0f,	 0.0f,

			-half.x, half.y,  half.z,  1.0f, 0.0f, 0.0f, extents.z, extents.y, -1.0f, 0.0f,	 0.0f, // 左面
			-half.x, half.y,  -half.z, 0.0f, 1.0f, 0.0f, 0.0f,		extents.y, -1.0f, 0.0f,	 0.0f,
			-half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f,		0.0f,	   -1.0f, 0.0f,	 0.0f,
			-half.x, -half.y, half.z,  1.0f, 1.0f, 0.0f, extents.z, 0.0f,	   -1.0f, 0.0f,	 0.0f,

			half.x,	 -half.y, half.z,  1.0f, 0.0f, 0.0f, extents.x, extents.z, 0.0f,  -1.0f, 0.0f, // 底面
			-half.x, -half.y, half.z,  0.0f, 1.0f, 0.0f, 0.0f,		extents.z, 0.0f,  -1.0f, 0.0f,
			-half.x, -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f,		0.0f,	   0.0f,  -1.0f, 0.0f,
			half.x,	 -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f,	   0.0f,  -1.0f, 0.0f,

			-half.x, half.y,  -half.z, 1.0f, 0.0f, 0.0f, extents.x, extents.y, 0.0f,  0.0f,	 -1.0f, // 后面
			half.x,	 half.y,  -half.z, 0.0f, 1.0f, 0.0f, 0.0f,		extents.y, 0.0f,  0.0f,	 -1.0f,
			half.x,	 -half.y, -half.z, 0.0f, 0.0f, 1.0f, 0.0f,		0.0f,	   0.0f,  0.0f,	 -1.0f,
			-half.x, -half.y, -half.z, 1.0f, 1.0f, 0.0f, extents.x, 0.0f,	   0.0f,  0.0f,	 -1.0f,
		};

		std::vector<unsigned int> indices = {
			0,	1,	2,	0,	2,	3,	// 前面
			4,	5,	6,	4,	6,	7,	// 上面
			8,	9,	10, 8,	10, 11, // 右面
			12, 13, 14, 12, 14, 15, // 左面
			16, 17, 18, 16, 18, 19, // 底面
			20, 21, 22, 20, 22, 23	// 后面
		};

		mymc::VertexLayout vertexLayout;

		// 位置
		vertexLayout.elements.push_back({ VertexElement::PositionIndex, 3, GL_FLOAT, 0 });
		// 颜色
		vertexLayout.elements.push_back({ VertexElement::ColorIndex, 3, GL_FLOAT, sizeof(float) * 3 });
		// UV
		vertexLayout.elements.push_back({ VertexElement::UVIndex, 2, GL_FLOAT, sizeof(float) * 6 });
		// 法线
		vertexLayout.elements.push_back({ VertexElement::NormalIndex, 3, GL_FLOAT, sizeof(float) * 8 });
		vertexLayout.stride = sizeof(float) * 11;

		return std::make_shared<mymc::Mesh>(vertexLayout, vertices, indices);
	}

	std::shared_ptr<Mesh> Mesh::CreateSphere(float radius, int sectors, int stacks)
	{
		const float PI = 3.14159265358979323846f;

		std::vector<float> vertices((stacks + 1) * (sectors + 1) * 8);
		for (int i = 0; i <= stacks; ++i)
		{
			float stackAngle = PI / 2.0f - static_cast<float>(i) * (PI / static_cast<float>(stacks));
			float xy = radius * cosf(stackAngle);
			float z = radius * sinf(stackAngle);

			for (int j = 0; j <= sectors; ++j)
			{
				float sectorAngle = static_cast<float>(j) * (2.0f * PI / static_cast<float>(sectors));

				float x = xy * cosf(sectorAngle);
				float y = xy * sinf(sectorAngle);

				size_t vertexStart = (i * (sectors + 1) + j) * 8;

				// 位置
				vertices[vertexStart] = x;
				vertices[vertexStart + 1] = y;
				vertices[vertexStart + 2] = z;

				// 法线（归一化位置向量）
				float lmymcth = sqrtf(x * x + y * y + z * z);
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

				if (i != (stacks - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		mymc::VertexLayout vertexLayout;

		// 位置
		vertexLayout.elements.push_back({ VertexElement::PositionIndex, 3, GL_FLOAT, 0 });
		// 法线
		vertexLayout.elements.push_back({ VertexElement::NormalIndex, 3, GL_FLOAT, sizeof(float) * 3 });
		// UV
		vertexLayout.elements.push_back({ VertexElement::UVIndex, 2, GL_FLOAT, sizeof(float) * 6 });
		vertexLayout.stride = sizeof(float) * 8;

		auto result = std::make_shared<mymc::Mesh>(vertexLayout, vertices, indices);

		return result;
	}

	std::shared_ptr<Mesh> Mesh::CreatePlane()
	{
		std::vector<float> vertices =
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

		mymc::VertexLayout vertexLayout;

		//位置
		vertexLayout.elements.push_back({
			VertexElement::PositionIndex,
			2,
			GL_FLOAT,
			0 });
		vertexLayout.stride = sizeof(float) * 2;

		auto result = std::make_shared<mymc::Mesh>(vertexLayout, vertices, indices);

		return result;
	}
} // namespace mymc