#include "router.h++"

#include <cstring>
#include <regex>

namespace http
{
  router::router() = default;

  router::~router() = default;

  auto router::add(const char *url, req::methods method, handlers &&handler) -> void
  {
    const auto found = this->find(url);
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
    const auto found = this->find(url);
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

  auto router::find(const char *url) -> route *
  {
    for (const auto &route : routes_)
    {
      // full complaring
      if (std::strcmp(route->url, url) != 0)
      {
        continue;
      }

      return route;
    }
    return nullptr;
  }

  auto router::match(const char *url) -> route *
  {
    map candidates;
    for (const auto &route : routes_)
    {
      // full complaring
      if (std::strcmp(route->url, url) == 0)
      {
        return route;
      }

      // comparing with regex
      std::string s(url);
      std::smatch m;
      std::regex exp(route->url);
      const auto tokens_original = this->get_tokens(url);
      const auto tokens_router = this->get_tokens(route->url);
      if (tokens_original.size() != tokens_router.size()) continue;

      while (std::regex_search(s, m, exp))
      {
        candidates.push_back(route);
        s = m.suffix().str();
      }
    }

    if (candidates.size() == 1)
    {
      const auto found = candidates.at(0);
      for (const auto &candidate : candidates)
      {
        const auto token_route = this->get_tokens(candidate->url);
        const auto token_original = this->get_tokens(url);
        int i = 0;
        for (const auto &token : token_original)
        {
          if (token != token_route.at(i))
          {
            found->params.push_back(token);
          }
          ++i;
        }
      }
      return found;
    }

    return nullptr;
  }

  auto router::get_tokens(const str &path) -> tokens_map
  {
    tokens_map map;
    std::string s(path);
    std::smatch m;
    std::regex exp(R"([^/]+)", std::regex_constants::ECMAScript);
    while (std::regex_search(s, m, exp))
    {
      map.push_back(m[0]);
      s = m.suffix().str();
    }
    return map;
  }

  auto router::reset() -> void
  {
    for (const auto &router : routes_)
    {
      router->params.clear();
    }
  }

  auto router::shutdown() -> void
  {
    for (const auto &router : routes_)
    {
      delete router;
    }
  }
} // namespace http
