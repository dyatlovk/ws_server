#pragma once

#include "request.h++"
#include "response_interface.h++"

namespace http
{
  struct middleware
  {
  public:
    virtual auto execute(http::request *req, response_interface &response) -> void = 0;
  };
} // namespace http
