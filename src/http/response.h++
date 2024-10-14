#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace http
{
  class response
  {
    static constexpr const char *CRLF = "\r\n";
    static constexpr const char *PROTO = "HTTP/1.1";

  public:
    response(const uint16_t code = 200, const std::string &code_msg = "OK");

    ~response();

    auto add_header(const char *key, const char *val) -> void;

    auto add_common_headers() -> void;

    auto get_headers() -> const std::string & { return this->headers_; }

    auto append_body(std::vector<char> *buf) -> void;

    auto append_body(const char *buf, int size) -> void;

    auto get_body() -> std::string { return std::string(this->body_.data()); }

    auto get_message() -> const std::string &;

    auto static server_error(const std::string &msg = "Server Error") -> std::unique_ptr<response>;

    auto static not_found(const std::string &msg = "Not Found") -> std::unique_ptr<response>;

  private:
    uint16_t code_ = 500;
    std::string code_msg_;
    std::vector<char> body_;
    std::string msg_;
    std::string headers_;
  };
} // namespace http
