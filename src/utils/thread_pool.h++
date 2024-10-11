#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace utils
{
  /**
   * @brief A fixed size pool of threads.
   */
  class thread_pool
  {
  public:
    explicit thread_pool(uint32_t threadCount = std::thread::hardware_concurrency());
    ~thread_pool();

    template<typename F, typename... Args>
    auto enqueue(F &&f, Args &&...args);

    void wait();

    const std::vector<std::thread> &get_workers() const { return workers; }

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
  };

  template<typename F, typename... Args>
  auto thread_pool::enqueue(F &&f, Args &&...args)
  {
    using return_type = typename std::invoke_result_t<F, Args...>;

    auto task =
        std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto result = task->get_future();

    {
      std::unique_lock<std::mutex> lock(queueMutex);

      if (stop) throw std::runtime_error("Enqueue called on a stopped ThreadPool");

      tasks.emplace([task]() { (*task)(); });
    }

    condition.notify_one();
    return result;
  }
} // namespace utils
