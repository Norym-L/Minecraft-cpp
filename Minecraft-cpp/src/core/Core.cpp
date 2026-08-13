#include "Core.h"

#include <iostream>

#include "entity/components/Camera.h"


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
		entity::Entity* main_camera_entity = nullptr;

		while (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();

			auto now = std::chrono::high_resolution_clock::now();
			auto delta_time = std::chrono::duration<float>(now - m_lastTime).count();
			m_lastTime = now;

			//相机部分
			CameraData camera_data;
			if (!main_camera_entity)
			{
				auto it = std::find_if(
					m_entities.begin(),
					m_entities.end(),
					[](const std::unique_ptr<entity::Entity>& entity)
					{
						return entity->getComponent<entity::Camera>() != nullptr;
					});

				if (it != m_entities.end())
				{
					main_camera_entity = it->get();
				}
			}
			if (main_camera_entity)
			{
				if (auto* camera = main_camera_entity->getComponent<entity::Camera>())
				{
					camera_data = camera->getCameraData();
				}
				m_renderQueue.draw(m_graphicsApi, camera_data);
			}
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
