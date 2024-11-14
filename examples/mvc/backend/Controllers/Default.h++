#pragma once

#include <http/request.h++>
#include <http/response.h++>

#include "fmt/core.h"

namespace Controllers
{
  class Default
  {
    using request = http::request;
    using response = ::http::response;

  public:
    inline Default() = default;

    inline ~Default() = default;

    inline auto index(request *req, response *res) -> response
    {
      res->with_added_header("Server", "Server Name");
      res->with_added_header("Content-Type", "text/html;charset=utf-8");
      res->with_view("/index.html");
      return *res;
    }

    inline auto about(request *req, response *res) -> response
    {
      res->with_added_header("Server", "Server Name");
      res->with_added_header("Content-Type", "text/html;charset=utf-8");
      res->with_view("/about.html");
      const auto params = req->req.params;
      for (const auto &c : params)
      {
        fmt::println("{}", c);
      }
      return *res;
    }

    inline auto faq(request *req, response *res) -> response
    {
      res->with_added_header("Server", "Server Name");
      res->with_added_header("Content-Type", "text/html;charset=utf-8");
      res->with_view("/faq.html");
      return *res;
    }
  };

} // namespace Controllers
