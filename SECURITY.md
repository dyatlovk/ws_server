# Security Policy

## Supported Versions

We actively support the following versions of WS Server with security updates:

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

We take security vulnerabilities seriously. If you discover a security vulnerability, please report it responsibly.

### How to Report

**Please do NOT report security vulnerabilities through public GitHub issues.**

Instead, please report them via one of the following methods:

1. **GitHub Security Advisories** (Preferred)
   - Go to the [Security tab](https://github.com/dyatlovk/ws_server/security/advisories)
   - Click "Report a vulnerability"
   - Fill out the form with details

2. **Email** (Alternative)
   - Send an email to: dyatlovk@users.noreply.github.com
   - Include "WS Server Security" in the subject line

### What to Include

Please include the following information in your report:

- **Description**: A clear description of the vulnerability
- **Steps to Reproduce**: Detailed steps to reproduce the issue
- **Impact**: Potential impact and attack scenarios
- **Affected Versions**: Which versions are affected
- **Code Sample**: Minimal code sample demonstrating the issue (if applicable)
- **Environment**: OS, compiler version, and other relevant details

### Response Timeline

- **Acknowledgment**: We will acknowledge receipt within 48 hours
- **Initial Assessment**: We will provide an initial assessment within 5 business days
- **Status Updates**: We will provide regular updates every 5-7 days
- **Resolution**: We aim to resolve critical vulnerabilities within 30 days

### Disclosure Policy

- We follow a coordinated disclosure approach
- We will work with you to understand and validate the vulnerability
- We will develop and test a fix before public disclosure
- We will provide credit to reporters (unless they prefer to remain anonymous)
- Public disclosure will occur after a fix is available and sufficient time has been given for users to update

### Security Best Practices

When using WS Server in production:

1. **Keep Updated**: Always use the latest stable version
2. **Input Validation**: Validate all user inputs in your application
3. **HTTPS**: Use HTTPS for production deployments
4. **Access Control**: Implement proper authentication and authorization
5. **Resource Limits**: Set appropriate limits for request sizes and connection counts
6. **Monitoring**: Monitor your application for unusual activity
7. **Dependencies**: Keep all dependencies up to date

### Known Security Considerations

- **Buffer Management**: WS Server handles buffers carefully, but always validate input sizes in your application
- **File Serving**: When serving static files, ensure proper path validation to prevent directory traversal
- **JSON Parsing**: Large JSON payloads may consume significant memory
- **Connection Limits**: Set appropriate limits to prevent resource exhaustion

### Security Testing

Our CI/CD pipeline includes:

- **Static Analysis**: Clang-tidy and cppcheck for code quality
- **Memory Safety**: Valgrind testing for memory leaks and errors
- **CodeQL**: GitHub's semantic code analysis for security vulnerabilities
- **Dependency Scanning**: Automated scanning of third-party dependencies

### Contact Information

For non-security related issues, please use the normal GitHub issue tracker.

For security-related questions or concerns, please contact:
- GitHub Security Advisories: [Security tab](https://github.com/dyatlovk/ws_server/security)
- Email: dyatlovk@users.noreply.github.com

Thank you for helping to keep WS Server secure!
