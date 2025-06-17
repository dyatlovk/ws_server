# WS Server

A modern, high-performance HTTP web server library written in C++20, designed for building scalable web applications and APIs.

## Table of Contents

- [Features](#features)
- [Quick Start](#quick-start)
- [Examples](#examples)
- [Installation](#installation)
- [API Reference](#api-reference)
- [Advanced Usage](#advanced-usage)
- [Architecture](#architecture)
- [Development](#development)
- [Troubleshooting](#troubleshooting)
- [License](#license)
- [Support](#support)

## Features

- **HTTP/1.1 Support**: Full HTTP/1.1 protocol implementation with keep-alive connections
- **Modern C++20**: Leverages the latest C++ features for performance and safety
- **Epoll-based I/O**: Efficient event-driven I/O using Linux epoll for high concurrency
- **Routing System**: Flexible URL routing with parameter extraction and regex support
- **Middleware Support**: Modular middleware architecture for request/response processing
- **Static File Serving**: Built-in static file server with MIME type detection
- **JSON Support**: Native JSON request/response handling
- **Thread Pool**: Asynchronous request processing with configurable thread pool
- **Socket Abstraction**: TCP and Unix domain socket support
- **MVC Architecture**: Model-View-Controller pattern support for structured applications
- **Comprehensive Logging**: Colored, timestamped logging system with multiple levels and fmt-style formatting
- **Comprehensive Testing**: Full test suite with unit tests and benchmarks

## Quick Start

### Basic HTTP Server
```cpp
#include <http/server.h++>
#include <http/options.h++>

int main() {
    auto options = http::options({3044, "127.0.0.1", "My Server", "/public"});
    http::server app(&options);

    http::router router;
    router.add("/", http::request::methods::Get,
        [](const http::request *req, http::response *res) {
            res->with_body("Hello, World!");
        });

    app.with_routers(&router);
    return app.listen();
}
```

## Examples

The project includes several comprehensive examples:

* **[Basic Server](./examples/basic/)** - Minimal HTTP server setup
* **[HTTP Server](./examples/http_server/)** - Full-featured HTTP server with routing, JSON API, and static files
* **[MVC Application](./examples/mvc/)** - Complete MVC web application with frontend/backend separation
* **[Logging Demo](./examples/logging_demo/)** - Comprehensive demonstration of the logging system features

## Installation

### Requirements
- **Git** - Version control
- **CMake 3.26+** - Build system (as required by CMakeLists.txt)
- **C++20 Compiler** - GCC 11+ or Clang 12+
- **Linux** - Currently supports Linux with epoll

### Build Instructions

1. **Clone the repository:**
```bash
git clone git@github.com:dyatlovk/ws_server.git
cd ws_server
```

2. **Configure and build (Debug):**
```bash
cmake --preset=makefile-x86_64-linux-debug
cmake --build --preset=debug-build-linux -j$(nproc)
```

3. **Configure and build (Release):**
```bash
cmake --preset=makefile-x86_64-linux-release
cmake --build --preset=release-build-linux -j$(nproc)
```

### Build Options

| Option               | Debug         | Release      | Description                    |
| -------------------- | ------------- | ------------ | ------------------------------ |
| CMAKE_BUILD_TYPE     | Debug         | MinSizeRel   | Build optimization level       |
| SRV_BUILD_EXAMPLES   | On            | On           | Build example applications     |
| SRV_BUILD_BENCHMARKS | Off           | On           | Build performance benchmarks  |
| SRV_BUILD_TESTS      | On            | Off          | Build unit test suite         |

### Running Tests

```bash
# Build with tests enabled (debug mode)
cmake --preset=makefile-x86_64-linux-debug
cmake --build --preset=debug-build-linux -j$(nproc)

# Run the test suite
./build/makefile-x86_64-linux-debug/tests/server_test
```

### Running Benchmarks

```bash
# Build with benchmarks enabled (release mode)
cmake --preset=makefile-x86_64-linux-release
cmake --build --preset=release-build-linux -j$(nproc)

# Run benchmarks
./build/makefile-x86_64-linux-release/benchmarks/response/benchmark_response
./build/makefile-x86_64-linux-release/benchmarks/router/benchmark_router
```

### Running Examples

```bash
# Examples are built with both debug and release configurations
# Run basic example
./build/makefile-x86_64-linux-debug/examples/basic/basic_example

# Run HTTP server example
./build/makefile-x86_64-linux-debug/examples/http_server/http_server

# Run MVC example
./build/makefile-x86_64-linux-debug/examples/mvc/mvc

# Run logging system demonstration
./build/makefile-x86_64-linux-debug/examples/logging_demo/logging_demo
```

### Quick Verification

After building, verify everything works:

```bash
# 1. Verify tests pass
./build/makefile-x86_64-linux-debug/tests/server_test --gtest_filter="*headers*"

# 2. Run a simple server test (in background, then test with curl)
./build/makefile-x86_64-linux-debug/examples/basic/basic_example &
SERVER_PID=$!
sleep 1
curl http://127.0.0.1:3044  # Should return "index"
kill $SERVER_PID

# 3. Check benchmark runs without errors
./build/makefile-x86_64-linux-release/benchmarks/response/benchmark_response

# 4. Test logging system
./build/makefile-x86_64-linux-debug/examples/logging_demo/logging_demo
```

## API Reference

### Server Configuration

```cpp
// Server options
http::options options({
    3044,               // Port
    "127.0.0.1",       // Host
    "My Server",       // Server name
    "/public"          // Static files directory
});

http::server app(&options);
```

### Routing

```cpp
http::router router;

// Simple route
router.add("/", http::request::methods::Get, handler);

// Multiple HTTP methods
router.add("/api/data", {http::request::methods::Get, http::request::methods::Post}, handler);

// Route parameters with regex
router.add("/user/\\d+", http::request::methods::Get, user_handler);
router.add("/blog/\\D+", http::request::methods::Get, blog_handler);
```

### Request Handling

```cpp
auto handler = [](http::request *req, http::response *res) {
    // Access request method
    auto method = req->req.method;

    // Access URI and parameters
    auto uri = req->req.uri;
    auto params = req->req.params; // Extracted from regex routes

    // HTTP version
    auto version = req->req.http_ver;
};
```

### Response Methods

```cpp
// Text response
res->with_body("Hello, World!");

// JSON response
miniJson::Json json = miniJson::Json::_object{{"key", "value"}};
res->with_json(&json);

// File response
res->with_view("/index.html");

// Redirect
res->with_redirect("/new-location");

// Headers
res->with_header("Content-Type", "application/json");
res->with_added_header("X-Custom", "value");

// Status codes
res->with_status(404, "Not Found");
```

### Logging System

The server includes a comprehensive logging system with colored output, timestamps, and multiple log levels:

```cpp
#include <utils/logger.h++>

// Basic logging with different levels
LOG_DEBUG("Detailed diagnostic information: {}", debug_info);
LOG_INFO("General application flow: {}", status);
LOG_WARN("Warning condition: {}", warning_msg);
LOG_ERROR("Error occurred: {}", error_msg);

// Configure log level filtering
utils::Logger::set_level(utils::LogLevel::INFO);  // Hide DEBUG messages
utils::Logger::set_level(utils::LogLevel::WARN);  // Hide DEBUG and INFO
utils::Logger::set_level(utils::LogLevel::ERROR); // Only show ERROR

// Advanced formatting with fmt-style syntax
LOG_INFO("User {} has {} unread messages", username, count);
LOG_DEBUG("Server stats: CPU={:.1f}%, Memory={:.2f}GB", cpu_usage, memory_gb);
LOG_ERROR("Database connection failed: host={}, port={}, timeout={}ms",
          db_host, db_port, timeout);
```

#### Log Levels and Colors

| Level | Color  | Description | Output Stream |
|-------|--------|-------------|---------------|
| DEBUG | Cyan   | Detailed diagnostic information | stdout |
| INFO  | Green  | General information about program execution | stdout |
| WARN  | Yellow | Warning conditions that should be noted | stderr |
| ERROR | Red    | Error conditions that indicate problems | stderr |

#### Integration with HTTP Server

The logging system is already integrated throughout the HTTP server components:

```cpp
// Server startup and shutdown
LOG_INFO("Server starting on {}:{}", host, port);
LOG_ERROR("Failed to initialize epoll: {}", e.what());
LOG_INFO("Server is shutting down");

// Socket operations (automatically logged)
LOG_DEBUG("Creating inet_socket for {}:{}", host, port);
LOG_INFO("Opening socket for {}:{} (domain={}, type={})", host, port, domain, type);
LOG_ERROR("Failed to create socket: {}", std::strerror(errno));

// Connection handling
LOG_INFO("Accepted connection from client (fd={})", peer);
LOG_DEBUG("Reading from connection fd={}, bufSize={}", conn, bufSize);
```

## Advanced Usage

### MVC Architecture

The server supports Model-View-Controller patterns for larger applications:

```cpp
// Controller class
class BlogController {
public:
    auto list(http::request *req, http::response *res) -> http::response {
        res->with_json(&blog_list);
        return *res;
    }

    auto entry(http::request *req, http::response *res) -> http::response {
        auto params = req->req.params;
        if (!params.empty()) {
            int id = std::atoi(params[0].c_str());
            // Load blog entry by ID
        }
        return *res;
    }
};

// Usage
BlogController blog;
router.add("/blog", method::Get,
    std::bind(&BlogController::list, &blog, std::placeholders::_1, std::placeholders::_2));
```

### Static File Serving

The server automatically serves static files from the configured public directory:

- Automatic MIME type detection
- Efficient file streaming
- Support for common web assets (HTML, CSS, JS, images)
- Configurable document root

### Performance Features

- **Epoll I/O Multiplexing**: Handle thousands of concurrent connections
- **Thread Pool**: Configurable worker threads for request processing
- **Zero-Copy Operations**: Efficient memory management
- **Connection Keep-Alive**: HTTP/1.1 persistent connections

### Logging and Debugging

The server provides comprehensive logging capabilities for development and production:

```cpp
// Development logging - show all messages
#ifdef DEBUG
    utils::Logger::set_level(utils::LogLevel::DEBUG);
#else
    utils::Logger::set_level(utils::LogLevel::INFO);
#endif

// Production logging with structured messages
LOG_INFO("Request processed: method={}, path='{}', status={}, duration={}ms",
         method, path, status_code, duration);

// Error handling with context
try {
    // ... server operations
} catch (const std::exception& e) {
    LOG_ERROR("Server error: {}", e.what());
}
```

#### Best Practices for Logging

1. **Use appropriate log levels**:
   - `DEBUG`: Detailed diagnostic info for development
   - `INFO`: General application flow and important events
   - `WARN`: Recoverable issues that should be noted
   - `ERROR`: Serious problems that need attention

2. **Include context in messages**:
   ```cpp
   LOG_ERROR("Failed to process request: client={}, path='{}', error='{}'",
             client_id, request_path, error_msg);
   ```

3. **Be mindful of sensitive data** - never log passwords or tokens

4. **Use structured logging for metrics**:
   ```cpp
   LOG_INFO("Performance metrics: requests={}, avg_time={}ms, errors={}",
            request_count, avg_response_time, error_count);
   ```

## Architecture

### Core Components

- **HTTP Parser**: RFC-compliant HTTP message parsing
- **Socket Layer**: Abstraction over TCP and Unix domain sockets
- **Epoll Engine**: Event-driven I/O for scalability
- **Router**: URL pattern matching and parameter extraction
- **Middleware Stack**: Pluggable request/response processing
- **Thread Pool**: Asynchronous task execution
- **Logging System**: Colored, timestamped logging with multiple levels and fmt-style formatting

### Dependencies

- **fmt**: Modern C++ formatting library
- **MiniJSON**: Lightweight JSON parsing and generation
- **Reflex**: Regular expression engine for routing

## Development

### Project Structure

```
├── src/                    # Core library source code
│   ├── http/              # HTTP protocol implementation
│   ├── io/                # I/O abstractions (sockets, epoll)
│   ├── stl/               # STL extensions
│   └── utils/             # Utility classes (logging, thread pool)
├── examples/              # Example applications
├── tests/                 # Unit test suite
├── benchmarks/            # Performance benchmarks
└── vendor/                # Third-party dependencies
```

### Contributing

1. **Code Style**: Follow modern C++20 conventions
2. **Testing**: Add tests for new features
3. **Documentation**: Update README and code comments
4. **Performance**: Consider performance implications

### Complete Development Workflow

```bash
# 1. Setup development environment
git clone git@github.com:dyatlovk/ws_server.git
cd ws_server

# 2. Configure for development (debug + tests)
cmake --preset=makefile-x86_64-linux-debug

# 3. Build everything
cmake --build --preset=debug-build-linux -j$(nproc)

# 4. Run tests to ensure everything works
./build/makefile-x86_64-linux-debug/tests/server_test

# 5. Make your changes...

# 6. Rebuild and test
cmake --build --preset=debug-build-linux -j$(nproc)
./build/makefile-x86_64-linux-debug/tests/server_test

# 7. Test performance impact (optional)
cmake --preset=makefile-x86_64-linux-release
cmake --build --preset=release-build-linux -j$(nproc)
./build/makefile-x86_64-linux-release/benchmarks/response/benchmark_response

# 8. Test examples still work
./build/makefile-x86_64-linux-debug/examples/basic/basic_example
```

### Testing

The project includes comprehensive tests covering:

- HTTP parsing and generation
- Socket operations
- Routing functionality
- Request/response handling
- Error conditions

### Benchmarking

Performance benchmarks are available for:

- Response generation speed
- Router matching performance
- Memory usage patterns
- Concurrency handling

## Troubleshooting

### Build Issues

**CMake version mismatch:**
```bash
# Ensure you have CMake 3.26+
cmake --version

# On Ubuntu/Debian, you might need to install from Kitware's repository
# if your distribution's CMake is too old
```

**Missing dependencies:**
```bash
# Ensure you have a C++20 compatible compiler
g++ --version  # Should be GCC 11+
clang++ --version  # Should be Clang 12+

# On Ubuntu/Debian:
sudo apt update
sudo apt install build-essential cmake git
```

**Build preset not found:**
```bash
# List available presets
cmake --list-presets

# If presets are not working, try manual configuration
mkdir -p build/manual
cd build/manual
cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DSRV_BUILD_TESTS=ON
make -j$(nproc)
```

### Runtime Issues

**Permission denied when running executables:**
```bash
# Make sure executables have execute permissions
chmod +x ./build/makefile-x86_64-linux-debug/tests/server_test
```

**Port already in use:**
```bash
# Check if port is already in use
sudo netstat -tulpn | grep :3044

# Kill process using the port
sudo lsof -ti:3044 | xargs sudo kill -9
```

**Library linking errors:**
```bash
# Ensure all dependencies are built
make clean
cmake --build --preset=debug-build-linux -j$(nproc)
```

### Testing Issues

**Tests failing:**
```bash
# Run specific test categories
./build/makefile-x86_64-linux-debug/tests/server_test --gtest_filter="*HttpParser*"

# Run in verbose mode for debugging
./build/makefile-x86_64-linux-debug/tests/server_test --gtest_verbose
```

**Performance issues:**
```bash
# Run in release mode for better performance
cmake --preset=makefile-x86_64-linux-release
cmake --build --preset=release-build-linux -j$(nproc)
./build/makefile-x86_64-linux-release/examples/basic/basic_example
```

### Logging Issues

**Too verbose logging in production:**
```bash
# Set appropriate log level to reduce output
utils::Logger::set_level(utils::LogLevel::WARN);  # Only warnings and errors
utils::Logger::set_level(utils::LogLevel::ERROR); # Only errors
```

**Missing log output:**
```bash
# Ensure log level allows your messages
utils::Logger::set_level(utils::LogLevel::DEBUG);  # Show all messages

# Check if you're using the correct macros
LOG_INFO("This message will appear");  # Correct
fmt::println("This bypasses logging system");  # Bypasses logging
```

**Log colors not showing:**
```bash
# Ensure terminal supports ANSI colors
echo $TERM  # Should show color-capable terminal like xterm-256color

# Test color support
./build/makefile-x86_64-linux-debug/examples/logging_demo/logging_demo
```

## License

This project is open source. Please check the repository for license details.

## Support

- **Issues**: Report bugs and feature requests on GitHub
- **Documentation**: Additional examples in the `examples/` directory
- **Community**: Contributions and feedback welcome

---

**Note**: This server is designed for Linux environments and requires epoll support. Windows and macOS support may be added in future versions.
