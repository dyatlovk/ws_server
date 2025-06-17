# Logging System Documentation

## Overview

A comprehensive logging system has been added to the WebServer project. The logging system provides colored, timestamped output with multiple log levels and is built on top of the high-performance `fmt` library for efficient string formatting.

## Features

- **Colored Output**: Different log levels have distinct colors for easy visual identification
- **Timestamps**: Each log message includes a precise timestamp in `[YYYY-MM-DD HH:MM:SS]` format
- **Multiple Log Levels**: DEBUG, INFO, WARN, ERROR with configurable filtering
- **Type-Safe Formatting**: Uses `fmt` library for compile-time format string validation
- **Output Routing**: Automatically routes INFO/DEBUG to stdout and WARN/ERROR to stderr
- **Easy Integration**: Simple macros for convenient usage throughout the codebase
- **Performance**: Zero-cost when log level filtering excludes messages

## Usage

### Basic Logging

```cpp
#include <utils/logger.h++>

int main() {
    LOG_DEBUG("Debug message with value: {}", 42);
    LOG_INFO("Application started successfully");
    LOG_WARN("Configuration file not found, using defaults");
    LOG_ERROR("Failed to connect to database: {}", error_message);
    return 0;
}
```

### Log Level Configuration

```cpp
// Set minimum log level (messages below this level will be filtered out)
utils::Logger::set_level(utils::LogLevel::INFO);  // Hide DEBUG messages
utils::Logger::set_level(utils::LogLevel::WARN);  // Hide DEBUG and INFO messages
utils::Logger::set_level(utils::LogLevel::ERROR); // Hide everything except ERROR

// Get current log level
auto current_level = utils::Logger::get_level();
```

### Advanced Formatting

The logging system supports all `fmt` formatting features:

```cpp
LOG_INFO("User {} has {} unread messages", username, count);
LOG_DEBUG("Server stats: CPU={:.1f}%, Memory={:.2f}GB", cpu_usage, memory_gb);
LOG_WARN("Connection timeout after {}ms (retries: {})", timeout, retry_count);
LOG_ERROR("Invalid request: status={}, reason='{}'", status_code, reason);
```

## Log Levels

| Level | Color  | Description | Output Stream |
|-------|--------|-------------|---------------|
| DEBUG | Cyan   | Detailed diagnostic information | stdout |
| INFO  | Green  | General information about program execution | stdout |
| WARN  | Yellow | Warning conditions that should be noted | stderr |
| ERROR | Red    | Error conditions that indicate problems | stderr |

## Integration Examples

The logging system is already integrated into the socket subsystem. Here are examples from `inet_socket.c++`:

```cpp
// Constructor validation
LOG_DEBUG("Creating inet_socket for {}:{}", host ? host : "NULL", port);
if (!host || port <= 0 || port > 65535) {
    LOG_ERROR("Invalid parameters: host={}, port={}", host ? host : "NULL", port);
    // ...
}

// Socket operations
LOG_INFO("Opening socket for {}:{} (domain={}, type={})", host_, port_, domain_, type_);
if (fd == -1) {
    LOG_ERROR("Failed to create socket: {}", std::strerror(errno));
    // ...
}

// Connection handling
LOG_INFO("Accepted connection from client (fd={})", peer);
LOG_DEBUG("Reading from connection fd={}, bufSize={}", conn, bufSize);
```

## File Structure

```
src/utils/
├── logger.h++     # Header file with Logger class and macros
└── logger.c++     # Implementation file with static variable definitions
```

## Implementation Details

### Logger Class

The `utils::Logger` class is a static utility class with the following key methods:

- `set_level(LogLevel level)`: Set minimum log level
- `get_level()`: Get current minimum log level
- `debug()`, `info()`, `warn()`, `error()`: Log message functions

### Convenience Macros

- `LOG_DEBUG(...)`: Log debug message
- `LOG_INFO(...)`: Log info message
- `LOG_WARN(...)`: Log warning message
- `LOG_ERROR(...)`: Log error message

### Thread Safety

The current implementation uses `fmt::print()` which is thread-safe for output, but the log level setting is not protected by mutexes. For multi-threaded applications, consider setting the log level once during initialization.

## Performance Considerations

- **Compile-time optimization**: When log level filtering excludes a message, the formatting overhead is avoided
- **Zero-allocation formatting**: The `fmt` library provides efficient string formatting
- **Minimal overhead**: Timestamp generation and color formatting are only performed for messages that will be displayed

## Examples

### Running the Demo

A comprehensive demonstration of the logging system is available:

```bash
cd build/examples/logging_demo
./logging_demo
```

This demo shows:
- All log levels with colors
- String formatting capabilities
- Log level filtering
- Real integration with socket operations
- Simulated application logging scenarios

### Sample Output

```
[2025-06-16 23:34:32] [INFO ] Application started successfully
[2025-06-16 23:34:32] [DEBUG] Loading configuration from config.json
[2025-06-16 23:34:32] [WARN ] Using default port 8080
[2025-06-16 23:34:32] [ERROR] Database connection failed: timeout
```

## Best Practices

1. **Use appropriate log levels**:
   - DEBUG: Detailed diagnostic info for development
   - INFO: General application flow and important events
   - WARN: Recoverable issues that should be noted
   - ERROR: Serious problems that need attention

2. **Include context in messages**:
   ```cpp
   LOG_ERROR("Failed to process request: client={}, path='{}', error='{}'",
             client_id, request_path, error_msg);
   ```

3. **Use structured logging**:
   ```cpp
   LOG_INFO("Request processed: method={}, path='{}', status={}, duration={}ms",
            method, path, status_code, duration);
   ```

4. **Be mindful of sensitive data**:
   ```cpp
   LOG_DEBUG("Processing login for user: {}", username);  // OK
   LOG_DEBUG("Password: {}", password);                   // NEVER DO THIS
   ```

5. **Set appropriate log levels for different environments**:
   ```cpp
   #ifdef DEBUG
       utils::Logger::set_level(utils::LogLevel::DEBUG);
   #else
       utils::Logger::set_level(utils::LogLevel::INFO);
   #endif
   ```

## Future Enhancements

Possible future improvements to the logging system:

- **Log rotation**: Automatic log file rotation based on size/time
- **Multiple outputs**: Simultaneous logging to file and console
- **Structured logging**: JSON output format option
- **Performance metrics**: Built-in timing and profiling support
- **Remote logging**: Network-based log aggregation
- **Thread safety**: Mutex protection for log level changes
