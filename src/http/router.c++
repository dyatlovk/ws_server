#include "router.h++"

#include <cstring>

namespace http
{
  router::router() = default;

  router::~router()
  {
    for (const auto &router : routes_)
    {
      delete router;
    }
  }

  auto router::add(const char *url, req::methods method, handlers &&handler) -> void
  {
    const auto found = this->match(url);
    const auto is_method_allowed = this->is_method_allowed(found, method);
    if (is_method_allowed) return;

    route *router_ = new route;
    router_->url = url;
    router_->methods.push_back(method);
    router_->handler = std::move(handler);
    this->routes_.push_back(router_);
  }

  auto router::add(const char *url, methods_map methods, handlers &&handler) -> void
  {
    const auto found = this->match(url);
    int matched = 0;
    for (const auto &m : methods)
    {
      const auto is_exists = is_method_allowed(found, m);
      if (is_exists)
      {
        ++matched;
      }
    }

    if (matched == methods.size())
    {
      return;
    }

    route *router_ = new route;
    router_->url = url;
    router_->methods = methods;
    router_->handler = std::move(handler);
    this->routes_.push_back(router_);
  }

  auto router::is_method_allowed(const route *router, const req::methods method) -> bool
  {
    for (const auto &route : routes_)
    {
      if (route != router)
      {
        continue;
      }

      for (const auto &m : route->methods)
      {
        if (m == method)
        {
          return true;
        }
      }
    }
    return false;
  }

  auto router::match(const char *url) -> route *
  {
    for (const auto &route : routes_)
    {
      if (std::strcmp(route->url, url) != 0)
      {
        continue;
      }

      return route;
    }
    return nullptr;
  }
} // namespace http
