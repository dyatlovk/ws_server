#pragma once

#include "../middleware_interface.h++"
#include "../response_interface.h++"
#include "../router.h++"

namespace http::middlewares
{
  class response final : public middleware
  {
  public:
    response();

    ~response();

    auto execute(http::request *req, response_interface &response) -> void override;

  public:
    auto set_router(const http::router *router) -> void { this->router_ = const_cast<http::router *>(router); }

  private:
    auto is_file_exist(const char *p) -> bool;

  private:
    http::router *router_;
  };
} // namespace http::middlewares
