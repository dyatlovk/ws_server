#pragma once

#include <algorithm>
#include <string>

#include "fmt/core.h"
#include "stl/string/ws_string.h++"

namespace http
{
  struct request
  {
  public:
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

    struct http_request
    {
      methods method;
      std::string uri;
      float http_ver;
    } http_req_;

    request(const std::string &input)
        : input_(input)
    {
    }

    ~request() = default;

    inline auto parse() -> const http_request *
    {
      std::string meth;
      std::string uri;
      std::string version_raw;
      for (int i = 0; i < input_.size(); i++)
      {
        static int spaces_count = 1;
        char ch[1] = {input_.at(i)};
        if (input_.at(i) == ' ')
        {
          spaces_count++;
        }

        if (spaces_count > 3) break;

        if (spaces_count == 1)
        {
          meth.append(ch);
        }

        if (spaces_count == 2)
        {
          uri.append(ch);
        }

        if (spaces_count == 3)
        {
          version_raw.append(ch);
        }
      }

      http_req_.method = get_method(meth);
      http_req_.uri = get_uri(uri);
      http_req_.http_ver = get_version(version_raw);
      return &http_req_;
    }

    inline static auto validate_request(const http_request *req) -> bool
    {
      return req->http_ver > 0 && !req->uri.empty();
    }

    inline static auto get_method(const std::string &m) -> const methods
    {
      auto str = ws_stl::trim_string(m);
      char *chr = const_cast<char *>(str.c_str());
      while (*chr)
        *(chr++) = toupper((unsigned char)*chr);

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

  private:
    std::string input_;
  };

} // namespace http
