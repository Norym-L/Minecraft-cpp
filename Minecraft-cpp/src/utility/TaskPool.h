/**
 * @file TaskPool.h
 * @brief 通用线程池：固定数量工作线程消费任务队列，用于把耗时任务移出主/渲染线程。
 */
#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace utility
{
/// @brief 简单线程池：提交 std::function 任务，由固定数量的工作线程并行执行。
class TaskPool
{
public:
    /**
     * @brief 构造并启动线程池。
     * @param thread_count 工作线程数
     */
    explicit TaskPool(size_t thread_count);

    /// @brief 析构：停止接受新任务并等待所有工作线程退出。
    ~TaskPool();

    TaskPool(const TaskPool &) = delete;

    TaskPool &operator=(const TaskPool &) = delete;

    /**
     * @brief 提交一个任务。若线程池已停止则直接忽略。
     * @param task 任务函数
     */
    void submit(std::function<void()> task);

    /// @brief 工作线程数量。
    [[nodiscard]] size_t threadCount() const { return m_threads.size(); }

    /// @brief 停止接受新任务并等待所有工作线程退出（幂等，析构时自动调用）。
    void stop();

private:
    void workerLoop();

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_stopping{false};
};
} // namespace utility