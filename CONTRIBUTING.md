# Contributing to WS Server

Thank you for your interest in contributing to WS Server! This document provides guidelines for contributing to the project.

## Development Setup

### Prerequisites

- **C++ Compiler**: GCC 10+ or Clang 14+
- **CMake**: Version 3.26 or higher
- **Build Tools**: make, ninja-build
- **Dependencies**: socat, libfmt-dev, pkg-config
- **Optional**: valgrind (for memory testing), clang-tidy, cppcheck

### Setting Up Development Environment

1. **Clone the repository:**
   ```bash
   git clone https://github.com/dyatlovk/ws_server.git
   cd ws_server
   ```

2. **Install dependencies (Ubuntu/Debian):**
   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential cmake ninja-build socat libfmt-dev pkg-config
   ```

3. **Configure and build:**
   ```bash
   cmake --preset makefile-x86_64-linux-debug
   cmake --build build --preset debug-build-linux --parallel $(nproc)
   ```

4. **Run tests:**
   ```bash
   cd build/makefile-x86_64-linux-debug
   ctest --output-on-failure
   ```

## Making Changes

### Code Style

- Follow modern C++20 conventions
- Use meaningful variable and function names
- Include comprehensive documentation for public APIs
- Add unit tests for new functionality

### Workflow

1. **Create a feature branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes and test locally:**
   ```bash
   cmake --build build --preset debug-build-linux
   ./build/makefile-x86_64-linux-debug/tests/server_test
   ```

3. **Run the validation script:**
   ```bash
   ./validate_ci.sh
   ```

4. **Commit your changes:**
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   ```

5. **Push and create a pull request:**
   ```bash
   git push origin feature/your-feature-name
   ```

## Pull Request Guidelines

### Before Submitting

- [ ] All tests pass locally
- [ ] Code follows project style guidelines
- [ ] New features include appropriate tests
- [ ] Documentation is updated if needed
- [ ] Commit messages follow conventional format

### PR Requirements

- **Clear Description**: Explain what changes you made and why
- **Test Coverage**: Include tests for new functionality
- **Documentation**: Update README or API docs if needed
- **Breaking Changes**: Clearly mark any breaking changes

### CI/CD Checks

Your PR will automatically run through our CI/CD pipeline:

- ✅ **Build Tests**: Multi-OS builds (Ubuntu 20.04, 22.04, latest)
- ✅ **Unit Tests**: Complete test suite execution
- ✅ **Memory Tests**: Valgrind memory leak detection
- ✅ **Code Quality**: Static analysis and formatting checks
- ✅ **HTTP Server Tests**: Functional endpoint testing

## Testing

### Running Tests Locally

```bash
# Run all tests
cd build/makefile-x86_64-linux-debug
ctest --output-on-failure

# Run specific test categories
./tests/server_test

# Test HTTP server functionality
cd examples/http_server
./http_server --port 8080 &
curl http://localhost:8080/
kill %1
```

### Adding New Tests

1. **Unit Tests**: Add to appropriate `tests/` subdirectory
2. **Integration Tests**: Add to `examples/` or main test suite
3. **Benchmarks**: Add to `benchmarks/` directory

### Test Structure

```cpp
#include "testSuite.h"

void test_your_feature() {
    // Arrange
    YourClass instance;

    // Act
    auto result = instance.yourMethod();

    // Assert
    assert(result == expected_value);
}
```

## Documentation

### API Documentation

- Document all public classes and methods
- Include usage examples
- Explain parameters and return values
- Note any exceptions or special cases

### README Updates

- Update feature list for new capabilities
- Add new examples if applicable
- Update build instructions if dependencies change

## Code Review Process

1. **Automated Checks**: CI/CD pipeline runs automatically
2. **Peer Review**: At least one maintainer review required
3. **Testing**: Reviewer tests functionality if needed
4. **Approval**: Maintainer approves and merges

## Release Process

### Version Numbering

We follow [Semantic Versioning](https://semver.org/):
- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes

### Creating Releases

1. **Update version numbers** in relevant files
2. **Update CHANGELOG.md** with release notes
3. **Create git tag:**
   ```bash
   git tag v1.2.3
   git push origin v1.2.3
   ```
4. **Automated release** will be created by GitHub Actions

## Getting Help

- **Issues**: Create a GitHub issue for bugs or feature requests
- **Discussions**: Use GitHub Discussions for questions
- **Documentation**: Check existing docs and examples
- **Code Review**: Ask for feedback in your PR

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Help others learn and grow
- Follow professional communication standards

Thank you for contributing to WS Server! 🚀
