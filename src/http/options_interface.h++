#pragma once

namespace http
{
  struct options_interface
  {
    virtual auto get_port() -> int = 0;

    virtual auto get_host() -> const char * = 0;

    virtual auto get_name() -> const char * = 0;

    virtual auto get_public_dir() -> const char * = 0;
  };
} // namespace http
