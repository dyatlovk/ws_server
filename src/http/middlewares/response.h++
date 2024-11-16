#pragma once

#include "../middleware_interface.h++"
#include "../options_interface.h++"
#include "../response_interface.h++"
#include "../router.h++"
#include "../stream.h++"

namespace http::middlewares
{
  class response final : public middleware
  {
  public:
    response(const options_interface *option);

    ~response();

    auto execute(http::request *req, response_interface &response) -> void override;

  public:
    auto set_router(const http::router *router) -> void { this->router_ = const_cast<http::router *>(router); }

  private:
    auto is_file_exist(const char *p) -> bool;

    auto load_file(const char *p) -> http::stream::buffer;

  private:
    options_interface *options_;
    http::router *router_;
    std::string mime_;
  };
} // namespace http::middlewares
