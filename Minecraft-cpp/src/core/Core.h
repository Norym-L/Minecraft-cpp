#pragma once

#include <chrono>
#include <thread>

#include "mymc.h"
#include "GLFW/glfw3.h"
#include "blocks/BlockManager.h"
#include "entity/EntityManager.h"


namespace mymc
{
	class Core
	{
	private:
		Core() = default;
		~Core() = default;

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
		[[nodiscard]] bool init(int width, int height, const std::string& title);

		/**
		 * 主循环运行
		 */
		void run();

		/**
		 * 设置鼠标是否启用（同时禁用光标移动事件）
		 * @param enabled true 启用鼠标 false 禁用鼠标
		 */
		void setCursorEnabled(bool enabled);

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
		entity::EntityManager& getEntityManager();

		/**
		 * 获取方块管理器
		 * @return 方块管理器引用
		 */
		block::BlockManager& getBlockManager();

		/**
		 * 获取按键输入管理器
		 * @return 按键输入管理器引用
		 */
		InputManager& getInputManager();

	private:
		GLFWwindow* m_window = nullptr;	// 窗口指针
		std::chrono::steady_clock::time_point m_lastTime;                 // 上一帧时间点（用于计算 deltaTime）
		bool m_cursorEnabled = false; // 光标是否启用
		GraphicsAPI m_graphicsAPI;	// 图形api
		RenderQueue m_renderQueue;	// 渲染队列
		InputManager m_inputManager; // 按键输入管理器
		entity::EntityManager m_entityManager; // 实体管理器
		block::BlockManager m_blockManager; // 方块管理器
	};
}
