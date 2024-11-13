# About

Web server for C++.

## Examples

* [Simple http](./examples/http_server/)
* [MVC](./examples/mvc/)

# Install

## Requirements
* Git
* Cmake (3.20+)
* C++20

### Install
```bash
    git clone git@github.com:dyatlovk/ws_server.git
    cd ./ws_server
```
Debug version:
```bash
    cmake --build --preset=debug-build-linux -jN
    make
```

Release version:
```bash
    cmake --build --preset=release-build-linux -jN
    make
```

Where **N** is a number of threads to build your project. For max performance on build you should use this simple formula: number of CPU threads + 1. For example. If you have CPU with 16 threads your flag will be -j17.

### CMake options:
| Option               | Debug         | Release    | Description           |
| -------------------- | ------------- | -----------| ----------------------|
| CMAKE_BUILD_TYPE     | Debug         | MinSizeRel | Build type            |
| SRV_BUILD_EXAMPLES   | On            | On         | Build all examples    |
| SRV_BUILD_BENCHMARKS | Off           | On         | Build with benchmarks |
| SRV_BUILD_TESTS      | On            | Off        | Build with tests      |
