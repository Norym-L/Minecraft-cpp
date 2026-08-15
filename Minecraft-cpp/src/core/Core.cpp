#include "Core.h"

#include <iostream>
#include <memory>

#include "entity/Player.h"
#include "entity/components/CameraComponent.h"

// 回调函数
namespace
{
    //键盘按键回调函数
    void keyCallback(GLFWwindow* window, const int key, int, const int action, int)
    {
        auto &inputManager = mymc::Core::GetInstance().getInputManager();
        if (action == GLFW_PRESS)
        {
            inputManager.setKeyPressed(key, true);
            inputManager.setKeyWasPressed(key, true);
        }
        else if (action == GLFW_RELEASE)
        {
            inputManager.setKeyPressed(key, false);
            inputManager.setKeyWasReleased(key, true);
        }
    }

    //鼠标按键回调函数
    void mouseButtonCallback(GLFWwindow* window, const int button, const int action, int mods)
    {
        auto &inputManager = mymc::Core::GetInstance().getInputManager();
        if (action == GLFW_PRESS)
        {
            inputManager.setMouseButtonPressed(button, true);
            inputManager.setMouseButtonWasPressed(button, true);
        }
        else if (action == GLFW_RELEASE)
        {
            inputManager.setMouseButtonPressed(button, false);
            inputManager.setMouseButtonWasReleased(button, true);
        }
    }

    //鼠标指针回调函数
    void cursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
    {
        auto &inputManager = mymc::Core::GetInstance().getInputManager();

        inputManager.setMousePositionOld(inputManager.getMousePositionCurrent());

        const glm::vec2 currentPos(static_cast<float>(xpos), static_cast<float>(ypos));
        inputManager.setMousePositionCurrent(currentPos);

        inputManager.setMousePositionChanged(true);
    }

    //窗口尺寸回调函数
    void windowSizeCallback(GLFWwindow* window, const int width, const int height)
    {
        auto &core = mymc::Core::GetInstance();
        core.getGraphicsAPI().setViewport(0, 0, width, height);
        if (const auto entity = core.getEntityManager().getMainCamera())
        {
            entity->getComponent<entity::CameraComponent>()->setWindowSize(width, height);
        }
    }
}

namespace mymc
{
    Core &Core::GetInstance()
    {
        static Core instance;
        return instance;
    }

    bool Core::init(const int width, const int height, const std::string &title)
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

        //glfw设置窗口回调
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPositionCallback);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); //启用鼠标原始输入

        glfwMakeContextCurrent(m_window);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "GLEW 初始化失败" << std::endl;
            glfwDestroyWindow(m_window);
            glfwTerminate();
            return false;
        }

        GraphicsAPI::Init();
        m_graphicsAPI.setViewport(0, 0, width, height);
        GraphicsAPI::SetClearColor(0.4f, 0.6f, 0.9f, 1.0f);
        m_renderQueue.init();
        m_blockManager.init(); // 注册测试方块（需在 GL 上下文就绪后执行）

        setCursorEnabled(false); // 默认禁用光标

        return true;
    }

    void Core::run()
    {
        m_lastTime = std::chrono::steady_clock::now();

        // ===== 场景搭建（测试）=====
        // 1. 创建相机实体（必须！场景里要有一个带 CameraComponent 的实体）
        auto* player = m_entityManager.addEntity<entity::Player>("Norym");
        player->setPosition(glm::vec3(3.0f, 2.0f, 3.0f));

        // 2. 让相机看向原点（需要 experimental 扩展）
        //    在文件顶部加：#define GLM_ENABLE_EXPERIMENTAL + #include <glm/gtx/quaternion.hpp>
        player->setRotation(glm::quatLookAt(
                                            glm::normalize(-player->getPosition()),
                                            glm::vec3(0.0f, 1.0f, 0.0f)));


        m_entityManager.reloadMainCamera();
        auto camera = m_entityManager.getMainCamera();


        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();

            if (m_inputManager.wasKeyReleased(GLFW_KEY_ESCAPE))
            {
                setCursorEnabled(!m_cursorEnabled);
            }

            auto now = std::chrono::steady_clock::now();
            const auto delta_time = std::chrono::duration<float>(now - m_lastTime).count();
            m_lastTime = now;

            // 测试开始

            // 更新实体
            m_entityManager.update(delta_time);
            // 更新方块
            m_blockManager.update(delta_time);
            // 测试结束

            CameraData camera_data{};
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

            m_inputManager.clearStates(); // 每帧清理按键状态
            GraphicsAPI::ClearBuffers();  // 每帧清屏
            m_renderQueue.draw(m_graphicsAPI, camera_data);
            glfwSwapBuffers(m_window); // ★ 关键：交换缓冲上屏
        }
    }

    void Core::setCursorEnabled(const bool enabled)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        m_cursorEnabled = enabled;
        m_inputManager.setMousePositionChangedEventEnable(!enabled);
    }

    GraphicsAPI &Core::getGraphicsAPI()
    {
        return m_graphicsAPI;
    }

    RenderQueue &Core::getRanderQueue()
    {
        return m_renderQueue;
    }

    entity::EntityManager &Core::getEntityManager()
    {
        return m_entityManager;
    }

    block::BlockManager& Core::getBlockManager()
    {
        return m_blockManager;
    }

    InputManager &Core::getInputManager()
    {
        return m_inputManager;
    }
}
