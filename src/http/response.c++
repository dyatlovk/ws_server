#include "response.h++"

#include <cstring>

namespace http
{
  response::response(const uint16_t code)
      : code_(code)
      , body_{}
      , headers_("")
      , msg_()
  {
  }

  response::~response()
  {
    body_.clear();
    headers_.clear();
  }

  auto response::add_header(const char *key, const char *val) -> void
  {
    this->headers_.append(key);
    this->headers_.append(":");
    this->headers_.append(val);
    this->headers_.append(CRLF);
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
    std::string http = "HTTP/1.1 200 OK";
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
