#include "TaskPool.h"

namespace utility
{
TaskPool::TaskPool(const size_t thread_count)
{
    m_threads.reserve(thread_count);
    for (size_t i = 0; i < thread_count; ++i)
    {
        m_threads.emplace_back(&TaskPool::workerLoop, this);
    }
}

TaskPool::~TaskPool()
{
    stop();
}

void TaskPool::submit(std::function<void()> task)
{
    if (m_stopping.load(std::memory_order_relaxed))
    {
        return; // 已停止：丢弃新任务
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(task));
    }
    m_cv.notify_one();
}

void TaskPool::stop()
{
    m_stopping.store(true, std::memory_order_relaxed);
    m_cv.notify_all();
    for (auto &t: m_threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    m_threads.clear();
}

void TaskPool::workerLoop()
{
    for (;;)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this] { return m_stopping.load(std::memory_order_relaxed) || !m_tasks.empty(); });
            if (m_tasks.empty())
            {
                return; // 已停止且队列清空
            }
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}
} // namespace utility