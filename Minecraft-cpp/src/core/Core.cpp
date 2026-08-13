#include "Core.h"

#include <iostream>
#include <memory>

#include "entity/components/CameraComponent.h"



namespace mymc
{
	Core& Core::GetInstance()
	{
		static Core instance;
		return instance;
	}

	bool Core::init(const int width, const int height, const std::string& title)
	{
		if (!glfwInit())
		{
			std::cerr << "GLFW 初始化失败" << std::endl;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if (!m_window)
		{
			std::cerr << "创建窗口失败" << std::endl;
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(m_window);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "GLEW 初始化失败" << std::endl;
			glfwDestroyWindow(m_window);
			glfwTerminate();
			return false;
		}

		return true;
	}

	void Core::run()
	{
		m_lastTime = std::chrono::high_resolution_clock::now();

		// ===== 场景搭建（测试）=====
		// 1. 创建相机实体（必须！场景里要有一个带 CameraComponent 的实体）
		auto* cameraEntity = m_entityManager.addEntity<entity::Entity>("main_camera");
		cameraEntity->setPosition(glm::vec3(3.0f, 2.0f, 3.0f));

		// 2. 让相机看向原点（需要 experimental 扩展）
		//    在文件顶部加：#define GLM_ENABLE_EXPERIMENTAL + #include <glm/gtx/quaternion.hpp>
		cameraEntity->setRotation(glm::quatLookAt(
			glm::normalize(-cameraEntity->getPosition()),
			glm::vec3(0.0f, 1.0f, 0.0f)));

		// 3. 挂相机组件
		auto* cameraComponent = cameraEntity->addComponent<entity::CameraComponent>();
		cameraComponent->setOwner(cameraEntity);	// ★ addComponent 不会自动设置 owner，不设会空指针崩溃
		cameraComponent->update(0.0f);				// 用真实位置/旋转刷新 view/project

		m_entityManager.reloadMainCamera();
		auto camera = m_entityManager.getMainCamera();

		auto mesh = Mesh::CreateBox(glm::vec3(1.0f));
		auto material = std::make_shared<Material>();
		material->setShaderProgram(m_graphicsApi.getDefaultShaderProgram());

		m_graphicsApi.init();
		m_renderQueue.init();
		m_graphicsApi.setClearColor(0.4f, 0.6f, 0.9f, 1.0f);
		m_graphicsApi.setViewport(0, 0, 800, 600);

		while (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();

			auto now = std::chrono::high_resolution_clock::now();
			auto delta_time = std::chrono::duration<float>(now - m_lastTime).count();
			m_lastTime = now;

			// 测试开始

			
			RenderCommand command = {
				mesh.get(),
				material.get(),
				glm::mat4(1.0f)
			};
			m_renderQueue.submit(command);

			// 测试结束

			CameraData camera_data;
			//相机部分
			if (camera)
			{
				camera_data = camera->getComponent<entity::CameraComponent>()->getCameraData();
			}
			else
			{
				m_entityManager.reloadMainCamera();
				camera = m_entityManager.getMainCamera();
			}

			m_graphicsApi.clearBuffers();         // 每帧清屏
			m_renderQueue.draw(m_graphicsApi, camera_data);
			glfwSwapBuffers(m_window);            // ★ 关键：交换缓冲上屏
		}
	}

	GraphicsAPI& Core::getGraphicsAPI()
	{
		return m_graphicsApi;
	}

	RenderQueue& Core::getRanderQueue()
	{
		return m_renderQueue;
	}

	EntityManager& Core::getEntityManager()
	{
		return m_entityManager;
	}
}
