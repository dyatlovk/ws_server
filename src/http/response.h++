#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace http
{
  class response
  {
    static constexpr const char *CRLF = "\r\n";

  public:
    response(const uint16_t code = 200);

    ~response();

    auto add_header(const char *key, const char *val) -> void;

    auto get_headers() -> const std::string & { return this->headers_; }

    auto append_body(std::vector<char> *buf) -> void;

    auto append_body(const char *buf, int size) -> void;

    auto get_body() -> std::string { return std::string(this->body_.data()); }

    auto get_message() -> const std::string &;

  private:
    uint16_t code_ = 500;
    std::vector<char> body_;
    std::string msg_;
    std::string headers_;
  };
} // namespace http
