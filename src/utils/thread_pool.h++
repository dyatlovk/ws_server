#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utils/logger.h++>
#include <vector>

namespace utils
{
  /**
   * @brief A high-performance, feature-rich thread pool with logging and monitoring
   */
  class thread_pool
  {
  public:
    struct statistics
    {
      std::size_t thread_count = 0;
      std::size_t queue_size = 0;
      std::size_t active_tasks = 0;
      std::size_t completed_tasks = 0;
      std::size_t failed_tasks = 0;
      bool is_running = false;
    };

    explicit thread_pool(uint32_t threadCount = std::thread::hardware_concurrency());
    ~thread_pool();

    // Disable copy and move to prevent resource management issues
    thread_pool(const thread_pool &) = delete;
    thread_pool &operator=(const thread_pool &) = delete;
    thread_pool(thread_pool &&) = delete;
    thread_pool &operator=(thread_pool &&) = delete;

    template<typename F, typename... Args>
    auto enqueue(F &&f, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>>;

    // Wait for all tasks to complete (both queued and executing)
    void wait_for_all();

    // Wait with timeout
    template<typename Rep, typename Period>
    bool wait_for_all_for(const std::chrono::duration<Rep, Period> &timeout);

    // Graceful shutdown with timeout
    template<typename Rep, typename Period>
    bool shutdown(const std::chrono::duration<Rep, Period> &timeout);

    // Get current statistics
    statistics get_statistics() const;

    // Check if pool is busy (has queued or executing tasks)
    bool is_busy() const;

    // Get number of worker threads
    std::size_t size() const { return workers.size(); }

    const std::vector<std::thread> &get_workers() const { return workers; }

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    mutable std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable wait_condition;

    std::atomic<bool> stop{false};
    std::atomic<std::size_t> active_tasks{0};
    std::atomic<std::size_t> completed_tasks{0};
    std::atomic<std::size_t> failed_tasks{0};

    void worker_thread(std::size_t worker_id);
  };

  template<typename F, typename... Args>
  auto thread_pool::enqueue(F &&f, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>>
  {
    using return_type = std::invoke_result_t<F, Args...>;

    // Use lambda instead of std::bind for better performance
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        [f = std::forward<F>(f), ... args = std::forward<Args>(args)]() mutable -> return_type { return f(args...); });

    auto result = task->get_future();

    {
      std::unique_lock<std::mutex> lock(queueMutex);

      if (stop.load())
      {
        LOG_WARN("Attempt to enqueue task on stopped thread pool");
        throw std::runtime_error("Enqueue called on a stopped ThreadPool");
      }

      tasks.emplace([task]() { (*task)(); });
      LOG_DEBUG("Task enqueued, queue size: {}", tasks.size());
    }

    condition.notify_one();
    return result;
  }

  template<typename Rep, typename Period>
  bool thread_pool::wait_for_all_for(const std::chrono::duration<Rep, Period> &timeout)
  {
    std::unique_lock<std::mutex> lock(queueMutex);

    LOG_DEBUG("Waiting for all tasks to complete (timeout: {}ms)",
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());

    bool completed =
        wait_condition.wait_for(lock, timeout, [this]() { return tasks.empty() && active_tasks.load() == 0; });

    if (completed)
    {
      LOG_DEBUG("All tasks completed successfully");
    }
    else
    {
      LOG_WARN("Wait timeout reached - {} queued tasks, {} active tasks", tasks.size(), active_tasks.load());
    }

    return completed;
  }

  template<typename Rep, typename Period>
  bool thread_pool::shutdown(const std::chrono::duration<Rep, Period> &timeout)
  {
    LOG_INFO("Initiating thread pool shutdown with {}ms timeout",
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count());

    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop.store(true);
    }

    condition.notify_all();

    auto shutdown_start = std::chrono::steady_clock::now();
    bool all_joined = true;

    for (auto &worker : workers)
    {
      if (worker.joinable())
      {
        auto remaining_time = timeout - (std::chrono::steady_clock::now() - shutdown_start);
        if (remaining_time <= std::chrono::duration<Rep, Period>::zero())
        {
          LOG_ERROR("Shutdown timeout reached, some threads may not have joined cleanly");
          all_joined = false;
          break;
        }
        worker.join();
      }
    }

    if (all_joined)
    {
      LOG_INFO("Thread pool shutdown completed successfully - {} tasks completed, {} failed", completed_tasks.load(),
          failed_tasks.load());
    }

    return all_joined;
  }
} // namespace utils
