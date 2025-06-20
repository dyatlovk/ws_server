#pragma once

#include <stl/string/ws_string.h++>
#include <vector>

namespace http
{
  struct request
  {
    enum class methods
    {
      Get,
      Post,
      Patch,
      Put,
      Options,
      Delete,
      Trace,
      Connect,
    };

    using params_map = std::vector<std::string>;

  public:
    struct http_request
    {
      methods method;
      std::string uri;
      float http_ver;
      params_map params;
    } req;

    request()
        : input_("")
    {
    }

    ~request()
    {
      input_.clear();
      req.params.clear();
    }

    inline auto parse(const std::string &input) -> const http_request *
    {
      std::string meth;
      std::string uri;
      std::string version_raw;
      int spaces_count = 1;
      const size_t input_size = input.size();

      for (size_t i = 0; i < input_size; i++)
      {
        char current_char = input.at(i);

        if (current_char == ' ')
        {
          spaces_count++;
          continue; // Skip the space character itself
        }

        if (spaces_count > 3) break;

        if (spaces_count == 1)
        {
          meth += current_char;
        }
        else if (spaces_count == 2)
        {
          uri += current_char;
        }
        else if (spaces_count == 3)
        {
          version_raw += current_char;
        }
      }

      req.method = get_method(meth);
      req.uri = get_uri(uri);
      req.http_ver = get_version(version_raw);

      if (!validate_request(&req)) return nullptr;

      return &req;
    }

    inline static auto validate_request(const http_request *req) -> bool
    {
      return req->http_ver > 0 && !req->uri.empty();
    }

    inline static auto get_method(const std::string &m) -> const methods
    {
      auto str = ws_stl::to_upper(ws_stl::trim_string(m));

      if (str == "GET") return methods::Get;
      if (str == "POST") return methods::Post;
      if (str == "PATCH") return methods::Patch;
      if (str == "PUT") return methods::Put;
      if (str == "OPTIONS") return methods::Options;
      if (str == "DELETE") return methods::Delete;
      if (str == "TRACE") return methods::Trace;
      if (str == "CONNECT") return methods::Connect;

      return methods::Get;
    }

    inline static auto get_uri(const std::string &raw) -> const std::string { return ws_stl::trim_string(raw); }

    inline static auto get_version(const std::string &input) -> float
    {
      auto kv = ws_stl::split_string(input, '/');
      if (kv.size() == 0) return 0.0;

      try
      {
        return std::stof(kv.begin()->second);
      }
      catch (std::invalid_argument &e)
      {
      }

      return 0;
    }

    inline static auto method_string(methods m) -> const std::string
    {
      switch (m)
      {
      case request::methods::Get: return "GET"; break;
      case methods::Post: return "POST"; break;
      case methods::Patch: return "PATCH"; break;
      case methods::Put: return "PUT"; break;
      case methods::Options: return "OPTIONS"; break;
      case methods::Delete: return "DELETE"; break;
      case methods::Trace: return "TRACE"; break;
      case methods::Connect: return "CONNECT"; break;
      default: return "GET"; break;
      }
    }

  private:
    std::string input_;
  };

} // namespace http
