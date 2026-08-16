#pragma once

#include <mutex>

#include "Common.h"
#include "Component.h"

namespace entity
{
class CameraComponent : public Component
{
public:
    void init() override;

    void update(float delta_time) override;

    /**
		 * 获取视图矩阵
		 * @return mat4视图矩阵
		 */
    [[nodiscard]] glm::mat4 getViewMatrix() const;

    /**
		 * 获取投影矩阵
		 * @param aspect 宽高比
		 * @return mat4投影矩阵
		 */
    [[nodiscard]] glm::mat4 getProjectMatrix(float aspect) const;

    /**
		 * 获取相机数据
		 * @return 相机数据
		 */
    [[nodiscard]] mymc::CameraData &getCameraData();

    /**
		 * 设置相机渲染窗口的尺寸
		 * @param width 宽度
		 * @param height 高度
		 */
    void setWindowSize(int width, int height);

private:
    std::mutex       m_cameraDataMutex;       // 锁相机数据
    mymc::CameraData m_cameraData;            // 相机数据
    float            m_fov          = 60.0f;  // 垂直渲染角度
    float            m_nearPlane    = 0.1f;   // 最近渲染距离
    float            m_farPlane     = 100.0f; // 最远渲染距离
    int              m_windowWidth  = 800;    // 窗口宽度
    int              m_windowHeight = 600;    // 窗口高度
};
}
