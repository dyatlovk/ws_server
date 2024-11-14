#pragma once

#include <http/server.h++>

namespace server
{
  auto get_options() -> http::server::options
  {
    http::server::options options;
    options.port = 3044;
    options.host = "127.0.0.1";
    options.name = "Black Mesa";
    options.public_dir = "/public";

    return options;
  }
} // namespace server
