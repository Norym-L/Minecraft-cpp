#pragma once

#include <chrono>
#include <thread>

#include "EntityManager.h"
#include "mymc.h"
#include "GLFW/glfw3.h"

namespace mymc
{
	class Core
	{
	public:
		Core(const Core&) = delete;
		Core operator=(Core& other) = delete;

		/**
		 * 获取单例
		 * @return Core对象单例
		 */
		static Core& GetInstance();

		/**
		 * 初始化核心
		 * @param width 窗口宽度
		 * @param height 窗口高度
		 * @param title 窗口标题
		 * @return true 成功 false 失败
		 */
		[[nodiscard]] bool init(const int width, const int height, const std::string& title);

		/**
		 * 主循环运行
		 */
		void run();

		/**
		 * 获取图形api
		 * @return 图形api引用
		 */
		GraphicsAPI& getGraphicsAPI();

		/**
		 * 获取渲染队列
		 * @return 渲染队列引用
		 */
		RenderQueue& getRanderQueue();

		/**
		 * 获取实体管理器
		 * @return 实体管理器引用
		 */
		EntityManager& getEntityManager();

	private:
		Core() = default;
		~Core() = default;

		GLFWwindow* m_window = nullptr;	// 窗口指针
		std::chrono::steady_clock::time_point m_lastTime;                 // 上一帧时间点（用于计算 deltaTime）
		GraphicsAPI m_graphicsApi;	// 图形api
		RenderQueue m_renderQueue;	// 渲染队列
		EntityManager m_entityManager; // 实体管理器
	};
}