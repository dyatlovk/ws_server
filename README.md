# WS Server

[![CI/CD Pipeline](https://github.com/dyatlovk/ws_server/workflows/CI/CD%20Pipeline/badge.svg)](https://github.com/dyatlovk/ws_server/actions/workflows/ci.yml)
[![Memory Tests](https://github.com/dyatlovk/ws_server/workflows/Memory%20Tests/badge.svg)](https://github.com/dyatlovk/ws_server/actions/workflows/memory-tests.yml)
[![Code Quality](https://github.com/dyatlovk/ws_server/workflows/Code%20Quality/badge.svg)](https://github.com/dyatlovk/ws_server/actions/workflows/code-quality.yml)

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
- [CI/CD](#cicd)
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

### Middleware

```cpp
// Built-in response middleware for static files and error handling
http::middlewares::response response_middleware(&options);
app.add_middleware(&response_middleware);

// Custom middleware implementation
class CustomMiddleware : public http::middleware_interface {
public:
    void execute(http::request *req, http::response_interface &res) override {
        // Custom middleware logic
    }
};
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

## Architecture

### Core Components

- **HTTP Parser**: RFC-compliant HTTP message parsing
- **Socket Layer**: Abstraction over TCP and Unix domain sockets
- **Epoll Engine**: Event-driven I/O for scalability
- **Router**: URL pattern matching and parameter extraction
- **Middleware Stack**: Pluggable request/response processing
- **Thread Pool**: Asynchronous task execution

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
│   └── utils/             # Utility classes
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

## CI/CD

This project uses GitHub Actions for continuous integration and deployment. The CI/CD pipeline includes:

### Workflows

- **🔄 CI/CD Pipeline** (`ci.yml`): Main build and test workflow
  - Runs on push to `master`/`develop` and pull requests
  - Multi-OS testing (Ubuntu 20.04, 22.04, latest)
  - Debug and Release builds with GCC 10/11
  - Unit tests and HTTP server functionality testing

- **🧠 Memory Tests** (`memory-tests.yml`): Memory analysis and performance testing
  - Valgrind memory leak detection
  - HTTP server load testing
  - Performance benchmarking with Apache Bench

- **🔍 Code Quality** (`code-quality.yml`): Static analysis and security
  - Clang-tidy static analysis
  - Cppcheck analysis with C++20 standard
  - Code formatting verification
  - CodeQL security scanning

- **💪 Stress Tests** (`stress-tests.yml`): Performance and stability testing
  - Apache Bench load testing (light/medium/heavy)
  - Long-running stability tests
  - Concurrent connections testing

- **🎯 Release** (`release.yml`): Automated release management
  - Triggered by git tags (`v*.*.*`)
  - Multi-OS binary builds
  - GitHub release creation with artifacts

### Running CI Locally

Test the CI pipeline locally before pushing:

```bash
# Validate CI setup
./validate_ci.sh

# Run setup guide
./setup_github_actions.sh
```

### Workflow Status

Check the [Actions tab](https://github.com/dyatlovk/ws_server/actions) for workflow status and results.

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

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

- **Issues**: Report bugs and feature requests on [GitHub Issues](https://github.com/dyatlovk/ws_server/issues)
- **Discussions**: Ask questions and get help on [GitHub Discussions](https://github.com/dyatlovk/ws_server/discussions)
- **Documentation**: Additional examples in the `examples/` directory
- **Contributing**: See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines
- **Security**: See [SECURITY.md](SECURITY.md) for security policy and reporting vulnerabilities
- **Community**: Contributions and feedback welcome

---

**Note**: This server is designed for Linux environments and requires epoll support. Windows and macOS support may be added in future versions.
