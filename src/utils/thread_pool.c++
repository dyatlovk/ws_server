#include "thread_pool.h++"

namespace utils
{
  thread_pool::thread_pool(uint32_t threadCount)
  {
    LOG_INFO("Initializing thread pool with {} threads", threadCount);

    workers.reserve(threadCount);

    for (std::size_t i = 0; i < threadCount; ++i)
    {
      workers.emplace_back(&thread_pool::worker_thread, this, i);
    }

    LOG_INFO("Thread pool started successfully with {} worker threads", workers.size());
  }

  thread_pool::~thread_pool()
  {
    LOG_DEBUG("Thread pool destructor called");

    // Use default 30 second timeout for destructor
    if (!shutdown(std::chrono::seconds(30)))
    {
      LOG_ERROR("Thread pool destructor: forced shutdown due to timeout");
    }
  }

  void thread_pool::worker_thread(std::size_t worker_id)
  {
    LOG_DEBUG("Worker thread {} started", worker_id);

    while (true)
    {
      std::function<void()> task;

      {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this] { return stop.load() || !tasks.empty(); });

        if (stop.load() && tasks.empty())
        {
          LOG_DEBUG("Worker thread {} shutting down", worker_id);
          return;
        }

        task = std::move(tasks.front());
        tasks.pop();
        active_tasks.fetch_add(1);

        LOG_DEBUG("Worker thread {} picked up task, {} tasks remaining in queue", worker_id, tasks.size());
      }

      // Execute task with exception handling
      try
      {
        task();
        completed_tasks.fetch_add(1);
        LOG_DEBUG("Worker thread {} completed task successfully", worker_id);
      }
      catch (const std::exception &e)
      {
        failed_tasks.fetch_add(1);
        LOG_ERROR("Worker thread {} task failed with exception: {}", worker_id, e.what());
      }
      catch (...)
      {
        failed_tasks.fetch_add(1);
        LOG_ERROR("Worker thread {} task failed with unknown exception", worker_id);
      }

      // Notify waiting threads that a task completed
      active_tasks.fetch_sub(1);

      {
        std::lock_guard<std::mutex> lock(queueMutex);
        wait_condition.notify_all();
      }
    }
  }

  void thread_pool::wait_for_all()
  {
    LOG_DEBUG("Waiting for all tasks to complete (no timeout)");

    std::unique_lock<std::mutex> lock(queueMutex);
    wait_condition.wait(lock, [this]() { return tasks.empty() && active_tasks.load() == 0; });

    LOG_DEBUG("All tasks completed");
  }

  thread_pool::statistics thread_pool::get_statistics() const
  {
    std::lock_guard<std::mutex> lock(queueMutex);

    return statistics{.thread_count = workers.size(),
        .queue_size = tasks.size(),
        .active_tasks = active_tasks.load(),
        .completed_tasks = completed_tasks.load(),
        .failed_tasks = failed_tasks.load(),
        .is_running = !stop.load()};
  }

  bool thread_pool::is_busy() const
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    return !tasks.empty() || active_tasks.load() > 0;
  }

} // namespace utils
