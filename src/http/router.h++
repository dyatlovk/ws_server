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
    using str = std::string;
    using req = http::request;
    using res = http::response;
    using tokens_map = std::vector<str>;
    using methods_map = std::vector<req::methods>;
    using params_map = std::vector<str>;

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
    auto add(const char *url, req::methods method, const handlers &&handler) -> void;
    auto add(const char *url, methods_map methods, const handlers &&handler) -> void;
    auto add(const char *url, methods_map methods, const handlers *handler) -> void;

    auto find(const char *url) -> route *;

    auto is_method_allowed(const route *route, const req::methods method) -> bool;

    auto get_size() -> const int { return routes_.size(); }

    auto get_routers() -> map { return this->routes_; }

    auto match(const char *url) -> route *;

    auto reset() -> void;

    auto shutdown() -> void;

  private:
    auto get_tokens(const char *path) -> tokens_map;

  public:
    struct route
    {
      const char *url;
      handlers handler;
      methods_map methods;
      params_map params;
    };

  private:
    map routes_;
  };
} // namespace http
