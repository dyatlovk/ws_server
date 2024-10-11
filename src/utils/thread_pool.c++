#include "thread_pool.h++"

namespace utils
{
  thread_pool::thread_pool(uint32_t threadCount)
  {
    workers.reserve(threadCount);

    for (std::size_t i = 0; i < threadCount; ++i)
    {
      workers.emplace_back(
          [this]
          {
            while (true)
            {
              std::function<void()> task;

              {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return stop || !tasks.empty(); });

                if (stop && tasks.empty()) return;

                task = std::move(tasks.front());
                tasks.pop();
              }

              task();
            }
          });
    }
  }

  thread_pool::~thread_pool()
  {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop = true;
    }

    condition.notify_all();

    for (auto &worker : workers)
      worker.join();
  }

  void thread_pool::wait()
  {
    std::unique_lock<std::mutex> lock(queueMutex);

    condition.wait(lock, [this]() { return tasks.empty(); });
  }
} // namespace utils
