#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "components/Component.h"

namespace mymc
{
	class EntityManager;
}

namespace entity
{
	class Component;

	class Entity
	{
		friend class mymc::EntityManager;
	public:
		Entity(const std::uint64_t id, std::string name);

		virtual ~Entity() = default;

		/**
		 * 获取实体唯一id
		 * @return 唯一id
		 */
		[[nodiscard]] std::uint64_t getId() const;

		/**
		 * 实体更新
		 * @param delta_time 帧时间
		 */
		virtual void update(float delta_time);

		/**
		 * 获取实体名称
		 * @return 实体名称字符串
		 */
		std::string& getName();

		/**
		 * 获取实体坐标
		 * @return vec3坐标
		 */
		[[nodiscard]] const glm::vec3& getPosition() const;

		/**
		 * 设置实体坐标
		 * @param position vec3实体坐标
		 */
		void setPosition(const glm::vec3& position);

		/**
		* 获取实体旋转
		* @return quat四元数
		*/
		[[nodiscard]] const glm::quat& getRotation() const;

		/**
		 * 设置实体旋转
		 * @param rotation quat四元数
		 */
		void setRotation(const glm::quat& rotation);

		/**
		 * 获取实体组件列表
		 * @return 组件列表
		 */
		[[nodiscard]] const std::vector<std::unique_ptr<Component>>& getComponents() const;

		/**
		 * 获取实体组件
		 * @tparam T 组件类型
		 * @return 组件指针，无该类型组件返回nullptr
		 */
		template <typename T>
		T* getComponent()
		{
			for (auto& component : m_components)
			{
				if (auto* casted = dynamic_cast<T*>(component.get()))
				{
					return casted;
				}
			}
			return nullptr;
		}

		/**
		 * 向实体添加组件
		 * @tparam T 组件类型
		 * @param component 组件
		 */
		template <typename T>
		void addComponent(std::unique_ptr<T> component)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must based on class Component");
			m_components.push_back(std::move(component));
		}

		/**
		 * 向实体添加组件（直接构造）
		 * @tparam T 组件类型
		 * @tparam Args 构造参数类型
		 * @param args 构造参数
		 * @return 添加的组件指针
		 */
		template <typename T, typename... Args>
		T* addComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component");

			auto component = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = component.get();
			m_components.push_back(std::move(component));
			return ptr;
		}

		/**
		 * 删除第 n 个特定类型的组件
		 * @tparam T 组件类型
		 * @param index 第几个（从0开始）
		 * @return true 成功 false 失败
		 */
		template <typename T>
		[[nodiscard]] bool removeComponent(std::size_t index)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component");
			size_t count = 0;
			for (auto it = m_components.begin(); it != m_components.end(); ++it)
			{
				if (dynamic_cast<T*>(it->get()))
				{
					if (count == index)
					{
						m_components.erase(it);
						return true;
					}
					++count;
				}
			}
			return false;
		}

	protected:
		std::uint64_t m_id;		// 实体唯一id
		std::string m_name;		// 实体名称
		glm::vec3 m_position;	// 实体坐标
		glm::quat m_rotation;	// 实体旋转（四元数）
		std::atomic<bool> m_isAlive{ true };	// 存活状态

		std::vector<std::unique_ptr<Component>> m_components;	// 实体组件列表
	};
}
