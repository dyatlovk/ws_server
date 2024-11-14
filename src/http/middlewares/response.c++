#include "response.h++"

#include <cstring>
#include <filesystem>

#include "../mime.h++"
#include "http/request.h++"
#include "http/response.h++"
#include "http/router.h++"

namespace http::middlewares
{
  response::response() {}

  response::~response() {}

  auto response::execute(http::request *req, response_interface &response) -> void
  {
    // validate request
    if (!req)
    {
      const char *phrase = "Server error";
      response.with_status(500, phrase);
      response.with_added_header("Content-Type", "text/html;charset=utf-8");
      response.with_added_header("Content-Length", std::to_string(std::strlen(phrase)).c_str());
      response.with_body(phrase);
      return;
    }

    // serve static files
    const auto file = this->is_file_exist(req->req.uri.c_str());
    if (file)
    {
      auto ext = std::filesystem::path(req->req.uri.c_str()).extension().string().erase(0, 1);
      http::mime mime;
      auto file_mime = mime.get_ext(ext);
      response.with_status(200, "OK");
      response.with_added_header("Content-Type", file_mime.mime.append(";charset=utf-8").c_str());
      response.with_body("Not Found");
      return;
    }

    // handle routers
    auto router = router_->match(req->req.uri.c_str());
    if (!router)
    {
      response.with_status(404, "Not Found");
      response.with_added_header("Content-Type", "text/html;charset=utf-8");
      response.with_body("Not Found");
      return;
    }

    // methods
    const auto is_method_allowed = router_->is_method_allowed(router, req->req.method);
    if (!is_method_allowed)
    {
      response.with_status(405, "Not Allowed");
      response.with_added_header("Content-Type", "text/html;charset=utf-8");
      return;
    }

    req->req.params = router->params;
    response.with_added_header("Content-Type", "text/html;charset=utf-8");
    const auto headers = response.get_headers();
    router->handler(const_cast<http::request *>(req), static_cast<http::response *>(&response));
  }

  auto response::is_file_exist(const char *p) -> bool
  {
    return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
  }
} // namespace http::middlewares
