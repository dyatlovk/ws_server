# GitHub Actions Configuration

This directory contains GitHub Actions workflows for automated testing and CI/CD.

## Workflows

### 1. `ci.yml` - Main CI/CD Pipeline
- **Triggers**: Push to master/develop, Pull requests, Manual dispatch
- **Runs on**: Ubuntu 20.04, 22.04, latest
- **Build types**: Debug and Release
- **Features**:
  - Multi-OS testing
  - Unit test execution
  - HTTP server functionality testing
  - Artifact uploads
  - Build verification

### 2. `memory-tests.yml` - Memory and Performance Testing
- **Triggers**: Push to master, Pull requests, Manual dispatch
- **Features**:
  - Valgrind memory leak detection
  - HTTP server memory testing under load
  - Performance benchmarking with Apache Bench
  - Memory report artifacts

### 3. `code-quality.yml` - Static Analysis and Code Quality
- **Triggers**: Push to master/develop, Pull requests, Manual dispatch
- **Features**:
  - Clang-tidy static analysis
  - Cppcheck analysis
  - Code formatting verification
  - CodeQL security scanning
  - Build with warnings as errors

### 4. `stress-tests.yml` - Stress and Load Testing
- **Triggers**: Push to master, Manual dispatch
- **Features**:
  - Apache Bench load testing (light/medium/heavy)
  - wrk HTTP benchmarking
  - Long-running stability tests (10 minutes)
  - Concurrent connections testing
  - Multiple endpoint testing

### 5. `release.yml` - Release Management
- **Triggers**: Git tags (v*.*.*), Manual dispatch
- **Features**:
  - Multi-OS release builds
  - Automated GitHub release creation
  - Binary artifact packaging
  - Release performance benchmarking
  - Test verification before release

## Workflow Status

You can check the status of these workflows in the Actions tab of your GitHub repository. Add these badges to your README.md:

```markdown
![CI/CD](https://github.com/dyatlovk/ws_server/workflows/CI/CD%20Pipeline/badge.svg)
![Memory Tests](https://github.com/dyatlovk/ws_server/workflows/Memory%20Tests/badge.svg)
![Code Quality](https://github.com/dyatlovk/ws_server/workflows/Code%20Quality/badge.svg)
```

## Environment Requirements

All workflows require:
- Ubuntu Linux (20.04, 22.04, or latest)
- CMake 3.20+
- GCC 10+ or Clang 14+
- Dependencies: socat, libfmt-dev, pkg-config

## Artifacts

Workflows generate various artifacts:
- **Test results**: CTest logs and test outputs
- **Build artifacts**: Compiled binaries for releases
- **Memory reports**: Valgrind output and memory analysis
- **Performance reports**: Benchmark results and stress test logs
- **Static analysis**: Code quality reports

## Customization

To customize these workflows for your repository:

1. Update repository-specific paths and names
2. Modify compiler versions and build configurations
3. Adjust test parameters and timeouts
4. Configure notification settings
5. Set up secrets for external services if needed

## Troubleshooting

Common issues and solutions:

- **Build failures**: Check CMake presets and dependency versions
- **Test timeouts**: Adjust timeout values in workflow files
- **Memory test failures**: Review Valgrind suppressions
- **Performance variations**: Results may vary between different runner instances
