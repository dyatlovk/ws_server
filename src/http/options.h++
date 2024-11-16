#pragma once

#include <filesystem>

#include "options_interface.h++"

namespace http
{
  class options : public options_interface
  {
  private:
    struct data;

  public:
    options(const data options)
        : data_(options)
    {
      const auto path = std::filesystem::current_path().string() + options.public_dir;
      data_.public_dir = path.c_str();
    }

    ~options() = default;

    auto get_port() -> int override { return data_.port; }

    auto get_host() -> const char * override { return data_.host; }

    auto get_name() -> const char * override { return data_.name; }

    auto get_public_dir() -> const char * override { return data_.public_dir.c_str(); }

  private:
    struct data
    {
      int port;
      const char *host;
      const char *name;
      std::string public_dir;
    } data_;
  };
} // namespace http
