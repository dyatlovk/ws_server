#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include <utils/logger.h++>
#include <utils/thread_pool.h++>

// Demo task that simulates work
void simulate_work(int task_id, int duration_ms) {
    LOG_DEBUG("Task {} starting ({}ms duration)", task_id, duration_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));

    // Simulate occasional failures
    if (task_id % 10 == 7) {
        throw std::runtime_error("Simulated task failure");
    }

    LOG_DEBUG("Task {} completed successfully", task_id);
}

// Demo task that returns a value
int calculate_fibonacci(int n) {
    LOG_DEBUG("Calculating fibonacci({})", n);
    if (n <= 1) return n;

    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate work

    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

void demonstrate_basic_usage() {
    std::cout << "\n=== Basic Thread Pool Usage ===\n";
    LOG_INFO("Demonstrating basic thread pool functionality");

    utils::thread_pool pool(4);

    // Show initial statistics
    auto stats = pool.get_statistics();
    LOG_INFO("Pool started - {} threads, queue size: {}, active: {}",
             stats.thread_count, stats.queue_size, stats.active_tasks);

    // Submit some tasks
    std::vector<std::future<void>> futures;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(50, 200);

    for (int i = 0; i < 10; ++i) {
        int duration = dis(gen);
        futures.push_back(pool.enqueue(simulate_work, i, duration));
    }

    // Show statistics while running
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stats = pool.get_statistics();
    LOG_INFO("During execution - queue size: {}, active: {}, completed: {}, failed: {}",
             stats.queue_size, stats.active_tasks, stats.completed_tasks, stats.failed_tasks);

    // Wait for all tasks
    for (auto& future : futures) {
        try {
            future.get();
        } catch (const std::exception& e) {
            LOG_WARN("Task failed: {}", e.what());
        }
    }

    // Final statistics
    stats = pool.get_statistics();
    LOG_INFO("All tasks completed - total completed: {}, failed: {}",
             stats.completed_tasks, stats.failed_tasks);
}

void demonstrate_return_values() {
    std::cout << "\n=== Tasks with Return Values ===\n";
    LOG_INFO("Demonstrating tasks that return values");

    utils::thread_pool pool(3);

    // Submit fibonacci calculations
    std::vector<std::future<int>> futures;
    for (int i = 1; i <= 8; ++i) {
        futures.push_back(pool.enqueue(calculate_fibonacci, i));
    }

    // Collect results
    for (size_t i = 0; i < futures.size(); ++i) {
        int result = futures[i].get();
        LOG_INFO("fibonacci({}) = {}", i + 1, result);
    }
}

void demonstrate_wait_functionality() {
    std::cout << "\n=== Wait Functionality ===\n";
    LOG_INFO("Demonstrating wait and timeout functionality");

    utils::thread_pool pool(2);

    // Submit long-running tasks
    for (int i = 0; i < 5; ++i) {
        pool.enqueue(simulate_work, i, 300);
    }

    // Try waiting with short timeout
    LOG_INFO("Attempting to wait with 500ms timeout...");
    bool completed = pool.wait_for_all_for(std::chrono::milliseconds(500));
    LOG_INFO("Wait result: {}", completed ? "completed" : "timeout");

    // Wait for all to complete
    LOG_INFO("Waiting for all tasks to complete...");
    pool.wait_for_all();
    LOG_INFO("All tasks completed");
}

void demonstrate_statistics_monitoring() {
    std::cout << "\n=== Statistics and Monitoring ===\n";
    LOG_INFO("Demonstrating real-time statistics monitoring");

    utils::thread_pool pool(3);

    // Submit a batch of tasks
    for (int i = 0; i < 15; ++i) {
        pool.enqueue(simulate_work, i, 100);
    }

    // Monitor progress
    while (pool.is_busy()) {
        auto stats = pool.get_statistics();
        LOG_INFO("Progress - Queue: {}, Active: {}, Completed: {}, Failed: {}",
                 stats.queue_size, stats.active_tasks, stats.completed_tasks, stats.failed_tasks);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    auto final_stats = pool.get_statistics();
    LOG_INFO("Final statistics - Completed: {}, Failed: {}",
             final_stats.completed_tasks, final_stats.failed_tasks);
}

void demonstrate_graceful_shutdown() {
    std::cout << "\n=== Graceful Shutdown ===\n";
    LOG_INFO("Demonstrating graceful shutdown with timeout");

    auto pool = std::make_unique<utils::thread_pool>(2);

    // Submit some long-running tasks
    for (int i = 0; i < 4; ++i) {
        pool->enqueue(simulate_work, i, 200);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Test graceful shutdown
    LOG_INFO("Initiating graceful shutdown...");
    bool shutdown_success = pool->shutdown(std::chrono::seconds(2));
    LOG_INFO("Shutdown result: {}", shutdown_success ? "success" : "timeout");

    pool.reset(); // Clean up
}

int main() {
    std::cout << "=== Thread Pool Improvements Demonstration ===\n";

    // Set debug level to see all logging
    utils::Logger::set_level(utils::LogLevel::DEBUG);

    LOG_INFO("Starting comprehensive thread pool demonstration");

    try {
        demonstrate_basic_usage();
        demonstrate_return_values();
        demonstrate_wait_functionality();
        demonstrate_statistics_monitoring();
        demonstrate_graceful_shutdown();

        LOG_INFO("All demonstrations completed successfully");

    } catch (const std::exception& e) {
        LOG_ERROR("Demonstration failed: {}", e.what());
        return 1;
    }

    std::cout << "\n=== Key Improvements Demonstrated ===\n";
    std::cout << "✓ Comprehensive logging integration\n";
    std::cout << "✓ Exception handling in tasks\n";
    std::cout << "✓ Real-time statistics and monitoring\n";
    std::cout << "✓ Proper wait functionality (wait for executing tasks)\n";
    std::cout << "✓ Timeout support for wait operations\n";
    std::cout << "✓ Graceful shutdown with timeout\n";
    std::cout << "✓ Performance optimizations (no std::bind)\n";
    std::cout << "✓ Thread safety improvements\n";
    std::cout << "✓ Better resource management\n";

    return 0;
}
