#include "response.h++"

#include <cstring>
#include <filesystem>
#include <fstream>

#include "../mime.h++"
#include "http/request.h++"
#include "http/response.h++"
#include "http/router.h++"
#include "http/stream.h++"

namespace http::middlewares
{
  response::response()
      : mime_("")
  {
  }

  response::~response()
  {
    mime_.clear();
  }

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
      response.with_status(200, "OK");
      http::mime mime;
      auto ext = std::filesystem::path(req->req.uri.c_str()).extension().string().erase(0, 1);
      auto file_mime = mime.get_ext(ext);
      mime_ = file_mime.mime.append(";charset=utf-8");
      response.with_added_header("Content-Type", mime_.c_str());
      const auto buffer = load_file(req->req.uri.c_str());
      response.with_body(buffer);
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
    const auto static_dir = std::filesystem::current_path().string() + "/public";
    const std::string path = static_dir + p;

    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
  }

  auto response::load_file(const char *p) -> http::stream::buffer
  {
    const auto static_dir = std::filesystem::current_path().string() + "/public";
    const std::string path = static_dir + p;
    const auto is_regular_file = std::filesystem::is_regular_file(path);
    const auto is_file_exist = std::filesystem::exists(path);

    if (!is_regular_file || !is_file_exist)
    {
      return {};
    }

    std::ifstream file(path, std::ios::binary);
    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    http::stream::buffer stream_buf;
    for (const auto &c : buf.str())
    {
      stream_buf.push_back(c);
    }

    return stream_buf;
  }
} // namespace http::middlewares
