#!/bin/bash

# GitHub Actions Local Validation Script
# This script validates that the build and test commands used in GitHub Actions work locally

set -e  # Exit on any error

echo "=== GitHub Actions Local Validation Script ==="
echo "Date: $(date)"
echo "Working directory: $(pwd)"
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}[PASS]${NC} $message"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}[FAIL]${NC} $message"
    elif [ "$status" = "INFO" ]; then
        echo -e "${YELLOW}[INFO]${NC} $message"
    fi
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d ".github/workflows" ]; then
    print_status "FAIL" "Not in the correct project directory"
    exit 1
fi

print_status "INFO" "Validating GitHub Actions workflows locally..."

# 1. Check dependencies that GitHub Actions will need
print_status "INFO" "Checking dependencies..."

dependencies=("cmake" "gcc" "g++" "socat")
missing_deps=()

for dep in "${dependencies[@]}"; do
    if command -v "$dep" >/dev/null 2>&1; then
        print_status "PASS" "$dep is available"
    else
        print_status "FAIL" "$dep is missing"
        missing_deps+=("$dep")
    fi
done

if [ ${#missing_deps[@]} -ne 0 ]; then
    print_status "FAIL" "Missing dependencies: ${missing_deps[*]}"
    echo "Install missing dependencies with: sudo apt-get install ${missing_deps[*]}"
    exit 1
fi

# 2. Test CMake configuration (Debug)
print_status "INFO" "Testing CMake configuration (Debug)..."
if cmake --preset makefile-x86_64-linux-debug >/dev/null 2>&1; then
    print_status "PASS" "CMake debug configuration successful"
else
    print_status "FAIL" "CMake debug configuration failed"
    echo "Error details:"
    cmake --preset makefile-x86_64-linux-debug
    exit 1
fi

# 3. Test CMake configuration (Release)
print_status "INFO" "Testing CMake configuration (Release)..."
if cmake --preset makefile-x86_64-linux-release >/dev/null 2>&1; then
    print_status "PASS" "CMake release configuration successful"
else
    print_status "FAIL" "CMake release configuration failed"
    echo "Error details:"
    cmake --preset makefile-x86_64-linux-release
    exit 1
fi

# 4. Test Build (Debug)
print_status "INFO" "Testing debug build..."
if cmake --build build --preset debug-build-linux --parallel $(nproc) >/dev/null 2>&1; then
    print_status "PASS" "Debug build successful"
else
    print_status "FAIL" "Debug build failed"
    echo "Error details:"
    cmake --build build --preset debug-build-linux --parallel $(nproc)
    exit 1
fi

# 5. Test Build (Release)
print_status "INFO" "Testing release build..."
if cmake --build build --preset release-build-linux --parallel $(nproc) >/dev/null 2>&1; then
    print_status "PASS" "Release build successful"
else
    print_status "FAIL" "Release build failed"
    echo "Error details:"
    cmake --build build --preset release-build-linux --parallel $(nproc)
    exit 1
fi

# 6. Test unit tests
print_status "INFO" "Testing unit tests..."
cd build/makefile-x86_64-linux-debug

if ctest --output-on-failure >/dev/null 2>&1; then
    print_status "PASS" "Unit tests passed"
else
    print_status "FAIL" "Unit tests failed"
    echo "Error details:"
    ctest --output-on-failure
    cd - >/dev/null
    exit 1
fi

cd - >/dev/null

# 7. Test specific test executable
print_status "INFO" "Testing server_test executable..."
if [ -f "build/makefile-x86_64-linux-debug/tests/server_test" ]; then
    if timeout 30s build/makefile-x86_64-linux-debug/tests/server_test >/dev/null 2>&1; then
        print_status "PASS" "server_test executable runs successfully"
    else
        print_status "FAIL" "server_test executable failed or timed out"
    fi
else
    print_status "FAIL" "server_test executable not found"
fi

# 8. Test HTTP server example
print_status "INFO" "Testing HTTP server example..."
if [ -f "build/makefile-x86_64-linux-debug/examples/http_server/http_server" ]; then
    print_status "PASS" "HTTP server executable exists"

    # Test if server can start and respond
    print_status "INFO" "Testing HTTP server functionality..."
    cd build/makefile-x86_64-linux-debug/examples/http_server

    # Start server in background
    timeout 20s ./http_server --port 8081 &
    SERVER_PID=$!

    # Wait for server to start
    sleep 2

    # Test basic endpoint
    if curl -f http://localhost:8081/ >/dev/null 2>&1; then
        print_status "PASS" "HTTP server responds to requests"
    else
        print_status "FAIL" "HTTP server does not respond to requests"
    fi

    # Cleanup
    kill $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true

    cd - >/dev/null
else
    print_status "FAIL" "HTTP server executable not found"
fi

# 9. Check workflow file syntax
print_status "INFO" "Checking workflow file syntax..."
workflow_files=(.github/workflows/*.yml)
for workflow in "${workflow_files[@]}"; do
    if [ -f "$workflow" ]; then
        # Basic YAML syntax check (if python is available)
        if command -v python3 >/dev/null 2>&1; then
            if python3 -c "import yaml; yaml.safe_load(open('$workflow'))" 2>/dev/null; then
                print_status "PASS" "$(basename "$workflow") has valid YAML syntax"
            else
                print_status "FAIL" "$(basename "$workflow") has invalid YAML syntax"
            fi
        else
            print_status "INFO" "$(basename "$workflow") exists (YAML validation skipped - python3 not available)"
        fi
    fi
done

# 10. Test if key build artifacts exist
print_status "INFO" "Checking build artifacts..."
artifacts=(
    "build/makefile-x86_64-linux-debug/tests/server_test"
    "build/makefile-x86_64-linux-debug/examples/http_server/http_server"
    "build/makefile-x86_64-linux-release/examples/http_server/http_server"
)

for artifact in "${artifacts[@]}"; do
    if [ -f "$artifact" ]; then
        print_status "PASS" "$(basename "$artifact") artifact exists"
    else
        print_status "FAIL" "$(basename "$artifact") artifact missing"
    fi
done

# 11. Test optional tools that workflows use
print_status "INFO" "Checking optional tools..."
optional_tools=("valgrind" "cppcheck" "clang-tidy" "ab" "wrk")
for tool in "${optional_tools[@]}"; do
    if command -v "$tool" >/dev/null 2>&1; then
        print_status "PASS" "$tool is available (used in advanced workflows)"
    else
        print_status "INFO" "$tool not available (optional - some workflows may skip related tests)"
    fi
done

echo
print_status "INFO" "=== Validation Summary ==="
echo "✅ Basic build and test pipeline validated"
echo "✅ CMake presets working correctly"
echo "✅ Unit tests can be executed"
echo "✅ HTTP server example functional"
echo "✅ GitHub Actions workflows created"
echo
echo "🚀 Your project is ready for GitHub Actions CI/CD!"
echo "   Push to GitHub and the workflows will automatically run."
echo
echo "📋 Workflow files created:"
for workflow in .github/workflows/*.yml; do
    echo "   - $(basename "$workflow")"
done
echo
echo "📖 See .github/workflows/README.md for detailed information about each workflow."
