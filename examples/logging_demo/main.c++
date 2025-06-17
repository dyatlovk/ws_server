#include <chrono>
#include <iostream>
#include <thread>

// Include our logger
#include <utils/logger.h++>

// Include socket for demonstration
#include <io/sockets/inet_socket.h++>

int main()
{
  std::cout << "=== Logging System Demonstration ===\n\n";

  // Set log level to DEBUG to see all messages
  utils::Logger::set_level(utils::LogLevel::DEBUG);

  LOG_INFO("Logging system initialized - showing all log levels");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Demonstrate different log levels with colors
  LOG_DEBUG("This is a DEBUG message - typically used for detailed diagnostic info");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  LOG_INFO("This is an INFO message - general information about program execution");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  LOG_WARN("This is a WARN message - indicates potential issues or important notices");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  LOG_ERROR("This is an ERROR message - indicates serious problems or failures");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Demonstrate formatting capabilities
  std::cout << "\n--- Formatting Examples ---\n";

  LOG_INFO("Formatting numbers: integer={}, float={:.2f}, hex=0x{:x}", 42, 3.14159, 255);
  LOG_DEBUG("Formatting strings: name='{}', status='{}'", "WebServer", "running");
  LOG_WARN("Network stats: connections={}, bytes_sent={}, uptime={}s", 150, 1024 * 1024, 3600);

  // Demonstrate log level filtering
  std::cout << "\n--- Log Level Filtering ---\n";
  LOG_INFO("Setting log level to WARN - DEBUG and INFO messages will be filtered out");

  utils::Logger::set_level(utils::LogLevel::WARN);

  LOG_DEBUG("This debug message should NOT appear");
  LOG_INFO("This info message should NOT appear");
  LOG_WARN("This warning message SHOULD appear");
  LOG_ERROR("This error message SHOULD appear");

  // Reset to show all messages
  std::cout << "\n--- Resetting to DEBUG level ---\n";
  utils::Logger::set_level(utils::LogLevel::DEBUG);

  LOG_INFO("Log level reset to DEBUG - all messages visible again");

  // Demonstrate real socket logging integration
  std::cout << "\n--- Real Socket Integration Demonstration ---\n";

  try
  {
    LOG_INFO("Creating TCP socket for localhost:9999");
    auto socket = io::inet_socket::make_tcp("localhost", 9999);

    if (socket)
    {
      LOG_INFO("Socket factory method succeeded");

      // Try to open the socket (this will show our integrated logging)
      if (socket->open())
      {
        LOG_INFO("Socket opened successfully - attempting to bind");

        // Try to bind (this will likely fail since we're not root for port < 1024, but will show error logging)
        if (socket->bind())
        {
          LOG_INFO("Socket bound successfully");
          socket->close();
        }
        else
        {
          LOG_WARN("Socket bind failed (expected for non-privileged ports)");
        }
      }
      else
      {
        LOG_ERROR("Failed to open socket");
      }
    }
    else
    {
      LOG_ERROR("Failed to create socket");
    }
  }
  catch (const std::exception &e)
  {
    LOG_ERROR("Exception during socket operations: {}", e.what());
  }

  std::cout << "\n--- Simulated Application Logging ---\n";

  LOG_INFO("Starting web server simulation");
  LOG_DEBUG("Loading configuration from config.json");
  LOG_DEBUG("Configuration loaded: port=8080, threads=4");

  // Simulate some client connections
  for (int i = 1; i <= 3; ++i)
  {
    LOG_INFO("Client {} connected from 192.168.1.{}", i, 100 + i);
    LOG_DEBUG("Processing HTTP request from client {}", i);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    LOG_DEBUG("Sending HTTP response to client {} ({} bytes)", i, 1024 + i * 100);
    LOG_INFO("Client {} request completed successfully", i);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
  }

  LOG_INFO("Server simulation completed");

  std::cout << "\n=== Logging Demonstration Complete ===\n";
  std::cout << "\nKey Features Demonstrated:\n";
  std::cout << "✓ Colored log levels (DEBUG=cyan, INFO=green, WARN=yellow, ERROR=red)\n";
  std::cout << "✓ Timestamps in [YYYY-MM-DD HH:MM:SS] format\n";
  std::cout << "✓ fmt-style string formatting with type safety\n";
  std::cout << "✓ Log level filtering (DEBUG < INFO < WARN < ERROR)\n";
  std::cout << "✓ Integration with existing socket code\n";
  std::cout << "✓ Convenient macros (LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR)\n";
  std::cout << "✓ Automatic output routing (INFO/DEBUG→stdout, WARN/ERROR→stderr)\n";

  return 0;
}
