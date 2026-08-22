#include "GraphicsAPI.h"
#include "ShaderProgram.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include <cerrno>
#include <iostream>

namespace mymc
{
bool GraphicsAPI::Init()
{
    glEnable(GL_DEPTH_TEST); //启用深度缓测试
    return true;
}

std::shared_ptr<ShaderProgram> GraphicsAPI::createShaderProgram(const std::string &vertex_source,
                                                                const std::string &fragment_source)
{
    ShaderKey key{
        .vertex_source   = vertex_source,
        .fragment_source = fragment_source
    };

    if (const auto it = m_shaderCache.find(key); it != m_shaderCache.end())
    {
        return it->second;
    }


    const GLuint vertex_shader      = glCreateShader(GL_VERTEX_SHADER);
    const char * vertex_shader_cstr = vertex_source.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_cstr, nullptr);
    glCompileShader(vertex_shader);

    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cerr << "ERROR:VERTEX_SHADER_COMPILATION_FAILED: " << infoLog << std::endl;
        glDeleteShader(vertex_shader);
        return nullptr;
    }

    const GLuint fragment_shader      = glCreateShader(GL_FRAGMENT_SHADER);
    const char * fragment_shader_cstr = fragment_source.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_cstr, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info_log[512];
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "ERROR:FRAGMENT_SHADER_COMPILATION_FAILED: " << info_log << std::endl;
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return nullptr;
    }

    GLuint shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader);
    glAttachShader(shader_program_id, fragment_shader);
    glLinkProgram(shader_program_id);

    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info_log[512];
        glGetProgramInfoLog(shader_program_id, 512, nullptr, info_log);
        std::cerr << "ERROR:SHADER_PROGRAM_LINKING_FAILED: " << info_log << std::endl;
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteProgram(shader_program_id);
        return nullptr;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    auto shader_program = std::make_shared<ShaderProgram>(shader_program_id);
    m_shaderCache.emplace(key, shader_program);

    return shader_program;
}

const std::shared_ptr<ShaderProgram> &GraphicsAPI::getDefaultShaderProgram()
{
    //创建默认着色器
    if (!m_defaultShaderProgram)
    {
        const std::string vertexShaderSource   = R"(
				#version 330 core
				layout (location = 0) in vec3 position;
				layout (location = 1) in vec3 color;
				layout (location = 2) in vec2 uv;
				layout (location = 3) in vec3 normal;

				out vec2 vUV;
				out vec3 vNormal;
				out vec3 vFragPos;

				uniform mat4 uModel;
				uniform mat4 uView;
				uniform mat4 uProjection;
				uniform mat3 uNormalMatrix;

				void main()
				{
					vUV = uv;

					vFragPos = vec3(uModel * vec4(position, 1.0));

					//法线矩阵由 CPU 端计算后传入，避免逐顶点求逆
					vNormal = uNormalMatrix * normal;

					gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
				}
			)";
        const std::string fragmentShaderSource = R"(
				#version 330 core

				struct Light
				{
					vec3 color;
					vec3 direction;
				};
				uniform Light uLight;
				uniform vec3 uCameraPos;

				out vec4 FragColor;

				in vec2 vUV;
				in vec3 vNormal;
				in vec3 vFragPos;

				uniform sampler2D uAtlas; // 方块贴图图集：种类与面方向差异已烘焙进顶点 UV
				uniform int uUseTexture;

				void main()
				{
					vec3 norm = normalize(vNormal);

					//漫反射
					vec3 lightDir = normalize(-uLight.direction);
					float diff = max(dot(norm, lightDir), 0.0);
					vec3 diffuse = diff * uLight.color;

					//镜面反射
					vec3 viewDir = normalize(uCameraPos - vFragPos);
					vec3 reflectDir = reflect(-lightDir, norm);
					float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
					float specularStrmymcth = 0.5;
					vec3 specular = specularStrmymcth * spec * uLight.color;

					//环境光
					const float ambientStrmymcth = 0.4;
					vec3 ambient = ambientStrmymcth * uLight.color;

					vec3 result = diffuse + specular + ambient;

					//图集采样：方块种类与面方向差异已烘焙进顶点 UV
					vec4 texColor;
					if (uUseTexture == 1)
					{
						texColor = texture(uAtlas, vUV);
					}
					else
					{
						texColor = vec4(1.0);
					}

					FragColor = texColor * vec4(result, 1.0);
				}
			)";

        m_defaultShaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    }

    return m_defaultShaderProgram;
}

const std::shared_ptr<ShaderProgram> &GraphicsAPI::getDefault2DShaderProgram()
{
    //创建默认2D着色器
    if (!m_default2DShaderProgram)
    {
        const std::string vertexShaderSource   = R"(
				#version 330 core
				layout (location = 0) in vec2 position;

				out vec2 vUV;

				uniform mat4 uModel;
				uniform mat4 uView;
				uniform mat4 uProjection;

				uniform vec2 uPivot;
				uniform vec2 uSize;

				uniform vec2 uUVMin;
				uniform vec2 uUVMax;

				void main()
				{
					vec2 local = (position - uPivot) * uSize;
					vUV = mix(uUVMin, uUVMax, position);

					gl_Position = uProjection * uView * uModel * vec4(local, 0.0, 1.0);
				}
			)";
        const std::string fragmentShaderSource = R"(
				#version 330 core

				in vec2 vUV;

				uniform vec4 uColor;

				uniform sampler2D uTex;

				out vec4 FragColor;

				void main()
				{
					vec4 src = texture(uTex, vUV) * uColor;
					FragColor = src;
				}
			)";

        m_default2DShaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    }

    return m_default2DShaderProgram;
}

const std::shared_ptr<ShaderProgram> &GraphicsAPI::getDefaultUIShaderProgram()
{
    if (!m_defaultUIShaderProgram)
    {
        const std::string vertexShaderSource   = R"(
				#version 330 core
				layout (location = 0) in vec2 position;
				layout (location = 1) in vec4 color;
				layout (location = 2) in vec2 uv;

				out vec2 vUV;
				out vec4 vColor;

				uniform mat4 uProjection;

				void main()
				{
					vUV = uv;
					vColor = color;

					gl_Position = uProjection * vec4(position, 0.0, 1.0);
				}
			)";
        const std::string fragmentShaderSource = R"(
				#version 330 core

				in vec2 vUV;
				in vec4 vColor;

				uniform sampler2D uTex;
				uniform int uUseTexture;

				out vec4 FragColor;

				void main()
				{
					vec4 src = (uUseTexture != 0) ? texture(uTex, vUV) * vColor : vColor;
					FragColor = src;
				}
			)";

        m_defaultUIShaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    }

    return m_defaultUIShaderProgram;
}

GLuint GraphicsAPI::CreateVertexBuffer(const std::vector<float> &vertices)
{
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return VBO;
}

GLuint GraphicsAPI::CreateIndexBuffer(const std::vector<uint32_t> &indices)
{
    GLuint EBO = 0;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)),
                 indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return EBO;
}

void GraphicsAPI::SetClearColor(const float r, const float g, const float b, const float a)
{
    glClearColor(r, g, b, a);
}

void GraphicsAPI::ClearBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const Rect &GraphicsAPI::getViewport() const
{
    return m_viewport;
}

void GraphicsAPI::setViewport(const int x, const int y, const int width, const int height)
{
    glViewport(x, y, width, height);
    m_viewport.x      = x;
    m_viewport.y      = y;
    m_viewport.width  = width;
    m_viewport.height = height;
}

void GraphicsAPI::SetDepthTestEnabled(const bool enabled)
{
    if (enabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

void GraphicsAPI::SetBlendMode(const BlendMode mode)
{
    switch (mode)
    {
        case BlendMode::DISABLE:
            glDisable(GL_BLEND);
            break;
        case BlendMode::ALPHA:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::ADDITIVE:
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case BlendMode::MULTIPLY:
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        default:
            glDisable(GL_BLEND);
            break;
    }
}

void GraphicsAPI::BindShaderProgram(ShaderProgram *shader_program)
{
    if (shader_program)
    {
        shader_program->bind();
    }
}

void GraphicsAPI::BindMaterial(Material *material)
{
    if (material)
    {
        material->bind();
    }
}

void GraphicsAPI::BindMesh(const Mesh *mesh)
{
    if (mesh)
    {
        mesh->bind();
    }
}

void GraphicsAPI::UnbindMesh(const Mesh *mesh)
{
    if (mesh)
    {
        Mesh::Unbind();
    }
}

void GraphicsAPI::DrawMesh(const Mesh *mesh)
{
    if (mesh)
    {
        mesh->draw();
    }
}
}
