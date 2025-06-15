# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- GitHub Actions CI/CD pipeline
- Comprehensive test automation
- Memory leak detection with Valgrind
- Static analysis with clang-tidy and cppcheck
- Performance benchmarking automation
- Multi-OS build testing (Ubuntu 20.04, 22.04, latest)

### Changed
- Enhanced README with CI/CD documentation
- Improved .gitignore for better artifact handling
- Updated build system for GitHub Actions compatibility

### Fixed
- Socket handling improvements in `inet_socket.c++`
- HTTP parser memory management fixes
- Request parsing segmentation fault resolution
- Buffer safety improvements

## [1.0.0] - 2024-XX-XX

### Added
- Initial release of WS Server
- HTTP/1.1 protocol implementation
- Epoll-based I/O for high performance
- Flexible routing system with parameter extraction
- Static file serving capabilities
- JSON request/response handling
- Thread pool for async processing
- TCP and Unix domain socket support
- MVC architecture support
- Comprehensive test suite

### Core Features
- **HTTP Server**: Full HTTP/1.1 implementation with keep-alive
- **Routing**: URL routing with regex support and parameter extraction
- **Middleware**: Modular request/response processing
- **I/O**: Efficient epoll-based event handling
- **Sockets**: TCP and Unix domain socket abstractions
- **JSON**: Native JSON support for APIs
- **Static Files**: Built-in static file server with MIME detection
- **Testing**: Complete unit test coverage

### Examples
- Basic HTTP server example
- MVC web application template
- Frontend integration examples

### Dependencies
- C++20 compatible compiler (GCC 10+, Clang 14+)
- CMake 3.26+
- fmt library for formatting
- Reflex for regular expressions
- MiniJSON for JSON processing
