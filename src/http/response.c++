#include "response.h++"

#include <cstring>

namespace http
{
  response::response(const uint16_t code, const std::string &code_msg)
      : code_(code)
      , body_{}
      , headers_("")
      , msg_()
      , code_msg_(code_msg)
  {
  }

  response::~response()
  {
    body_.clear();
    headers_.clear();
  }

  auto response::server_error(const std::string &msg) -> std::unique_ptr<response>
  {
    auto r = std::make_unique<response>(500);
    r->add_header("Server", "WS");
    r->add_header("Content-Type", "text/html;charset=utf-8");
    r->add_header("Content-Length", std::to_string(msg.size()).c_str());
    return r;
  }

  auto response::not_found(const std::string &msg) -> std::unique_ptr<response>
  {
    auto r = std::make_unique<response>(404);
    r->add_header("Server", "WS");
    r->add_header("Content-Type", "text/html;charset=utf-8");
    r->add_header("Content-Length", std::to_string(msg.size()).c_str());
    return r;
  }

  auto response::add_header(const char *key, const char *val) -> void
  {
    this->headers_.append(key);
    this->headers_.append(":");
    this->headers_.append(val);
    this->headers_.append(CRLF);
  }

  auto response::add_common_headers() -> void
  {
    add_header("Server", "WS");
  }

  auto response::append_body(std::vector<char> *buf) -> void
  {
    int i{0};
    while (i < buf->size())
    {
      const char *ch = buf[i++].data();
      body_.push_back(*ch);
    }
  }

  auto response::append_body(const char *buf, int size) -> void
  {
    int i{0};
    while (i < size)
    {
      body_.push_back(buf[i++]);
    }
  }

  auto response::get_message() -> const std::string &
  {
    std::string http = PROTO;
    http += " ";
    http += std::to_string(code_) + " " + code_msg_;
    http += CRLF;
    http += headers_;
    http += CRLF;
    for (const auto &item : body_)
    {
      http += item;
    }

    int i{0};
    while (i < http.size())
    {
      msg_.push_back(http[i++]);
    }

    return this->msg_;
  }
} // namespace http
