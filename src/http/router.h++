#pragma once

#include <functional>
#include <http/request.h++>
#include <http/response.h++>
#include <vector>

namespace http
{
  class router
  {
  private:
    using req = http::request;
    using res = http::response;
    using methods_map = std::vector<req::methods>;

  public:
    struct route;
    using map = std::vector<route *>;
    using handlers = std::function<void(req *, res *)>;

  public:
    router();

    ~router();

    /**
     * Add handler to container.
     * Endpoint is unique. The same url with different methods will be make a different instances
     */
    auto add(const char *url, req::methods method, handlers &&handler) -> void;
    auto add(const char *url, methods_map methods, handlers &&handler) -> void;

    auto match(const char *url) -> route *;

    auto is_method_allowed(const route *route, const req::methods method) -> bool;

    auto get_size() -> const int { return routes_.size(); }

    auto get_routers() -> map { return this->routes_; }

  public:
    struct route
    {
      const char *url;
      handlers handler;
      methods_map methods;
    };

  private:
    map routes_;
  };
} // namespace http
