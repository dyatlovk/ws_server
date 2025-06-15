#!/bin/bash

# GitHub Actions Setup Guide
# This scecho "2. Create GitHub repository and push:"
echo "   # Repository already configured for dyatlovk/ws_server"
echo "   git add ."
echo "   git commit -m 'Initial commit with GitHub Actions'"
echo "   git push origin master"helps you set up and use the GitHub Actions workflows

set -e

echo "🚀 GitHub Actions Setup Guide for ws_server"
echo "============================================="
echo

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_step() {
    echo -e "${BLUE}📋 $1${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ️  $1${NC}"
}

# Step 1: Verify workflows exist
print_step "Step 1: Verifying GitHub Actions workflows..."
if [ -d ".github/workflows" ] && [ "$(ls .github/workflows/*.yml 2>/dev/null | wc -l)" -gt 0 ]; then
    print_success "GitHub Actions workflows found:"
    ls .github/workflows/*.yml | sed 's|.github/workflows/||' | sed 's|.yml||' | while read workflow; do
        echo "  • $workflow"
    done
else
    echo "❌ No GitHub Actions workflows found!"
    exit 1
fi
echo

# Step 2: Show repository setup instructions
print_step "Step 2: Repository Setup Instructions"
echo "To enable these workflows in your GitHub repository:"
echo
echo "1. Initialize git repository (if not already done):"
echo "   git init"
echo "   git add ."
echo "   git commit -m 'Initial commit with GitHub Actions'"
echo
echo "2. Create GitHub repository and push:"
echo "   # Create repository on GitHub, then:"
echo "   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git"
echo "   git branch -M master"
echo "   git push -u origin master"
echo
echo "3. The workflows will automatically run on:"
echo "   • Push to master/develop branches"
echo "   • Pull requests"
echo "   • Manual dispatch (no scheduled runs)"
echo

# Step 3: Workflow descriptions
print_step "Step 3: Workflow Descriptions"
echo
echo "📊 ci.yml - Main CI/CD Pipeline"
echo "   • Runs on every push and PR"
echo "   • Tests on Ubuntu 20.04, 22.04, latest"
echo "   • Both Debug and Release builds"
echo "   • Unit tests and HTTP server testing"
echo
echo "🧠 memory-tests.yml - Memory Analysis"
echo "   • Runs daily at 2 AM UTC"
echo "   • Valgrind memory leak detection"
echo "   • Performance benchmarking"
echo
echo "🔍 code-quality.yml - Static Analysis"
echo "   • Runs on push/PR"
echo "   • Clang-tidy, cppcheck analysis"
echo "   • Code formatting checks"
echo "   • CodeQL security scanning"
echo
echo "💪 stress-tests.yml - Load Testing"
echo "   • Runs weekly on Sundays"
echo "   • Apache Bench and wrk testing"
echo "   • 10-minute stability tests"
echo "   • Concurrent connection testing"
echo
echo "🎯 release.yml - Release Management"
echo "   • Triggers on git tags (v*.*.*)'"
echo "   • Creates GitHub releases"
echo "   • Builds and packages binaries"
echo

# Step 4: Usage examples
print_step "Step 4: Usage Examples"
echo
echo "🏷️  Creating a release:"
echo "   git tag v1.0.0"
echo "   git push origin v1.0.0"
echo "   # This automatically triggers the release workflow"
echo
echo "🔧 Manual workflow dispatch:"
echo "   • Go to GitHub Actions tab"
echo "   • Select workflow"
echo "   • Click 'Run workflow'"
echo
echo "📊 Monitoring workflows:"
echo "   • Check GitHub Actions tab for status"
echo "   • Download artifacts for detailed logs"
echo "   • View test results and performance reports"
echo

# Step 5: Status badges
print_step "Step 5: Adding Status Badges to README"
echo
echo "Add these badges to your README.md:"
echo
echo '```markdown'
echo '![CI/CD Pipeline](https://github.com/dyatlovk/ws_server/workflows/CI/CD%20Pipeline/badge.svg)'
echo '![Memory Tests](https://github.com/dyatlovk/ws_server/workflows/Memory%20Tests/badge.svg)'
echo '![Code Quality](https://github.com/dyatlovk/ws_server/workflows/Code%20Quality/badge.svg)'
echo '```'
echo
echo "Badges are already configured in the README.md file."
echo

# Step 6: Customization
print_step "Step 6: Customization Options"
echo
echo "🔧 Common customizations:"
echo "   • Update .github/dependabot.yml with maintainer usernames"
echo "   • Modify test timeouts in workflow files"
echo "   • Adjust load testing parameters"
echo "   • Configure notification settings"
echo "   • Add deployment steps to release workflow"
echo

# Step 7: Troubleshooting
print_step "Step 7: Troubleshooting"
echo
echo "🐛 If workflows fail:"
echo "   1. Check the Actions tab for error details"
echo "   2. Run 'validate_ci.sh' locally to debug"
echo "   3. Ensure all dependencies are properly specified"
echo "   4. Check CMake presets are working"
echo "   5. Verify socat is available (required for tests)"
echo

# Step 8: Local testing
print_step "Step 8: Local Testing"
echo
if [ -f "validate_ci.sh" ]; then
    echo "🧪 Test workflows locally:"
    echo "   ./validate_ci.sh"
    echo
    print_info "Running local validation now..."
    if timeout 60s ./validate_ci.sh >/dev/null 2>&1; then
        print_success "Local validation completed successfully!"
    else
        echo "⚠️  Local validation had issues. Check manually with:"
        echo "   ./validate_ci.sh"
    fi
else
    echo "⚠️  validate_ci.sh not found. Create it to test workflows locally."
fi
echo

print_step "🎉 Setup Complete!"
echo
echo "Your GitHub Actions workflows are ready to use!"
echo "Push this repository to GitHub to activate the CI/CD pipeline."
echo
echo "📚 For detailed information, see:"
echo "   • .github/workflows/README.md"
echo "   • GITHUB_ACTIONS_SUMMARY.md"
echo
echo "🌟 Happy coding!"
