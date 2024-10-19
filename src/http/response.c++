#include "response.h++"

#include <cstring> // strlen
#include <filesystem>
#include <fstream>

#include "stream.h++"

namespace http
{
  stream stream_;

  response::response(int code, const char *reason)
      : code_(code)
      , reason_(reason)
      , proto_v_(PROTO_DEFAULT)
      , headers_()
      , msg_("")
  {
    with_proto_ver("1.1");
  }

  response::~response()
  {
    this->code_ = 0;
    this->headers_.clear();
    this->proto_v_ = PROTO_DEFAULT;
    this->msg_.clear();
  }

  auto response::get_status_code() -> int
  {
    if (code_ < 100 || code_ > 599) return 500;

    return code_;
  }

  auto response::with_status(int code, const char *reason) -> response *
  {
    code_ = code;
    reason_ = reason;

    return this;
  }

  auto response::with_proto_ver(const char *ver) -> void
  {
    std::string version;
    version.reserve(strlen(PROTO_PREFIX) + strlen(ver));
    version.append(PROTO_PREFIX);
    version.append(ver);
    this->proto_v_ = version;
  }

  auto response::has_header(const char *key) -> bool
  {
    const auto found = this->get_header(key);
    if (!found) return false;

    return found->key == key;
  }

  auto response::with_header(const char *key, const char *val) -> void
  {
    this->headers_.clear();
    this->headers_.push_back({key, val});
  }

  auto response::with_added_header(const char *key, const char *val) -> void
  {
    const auto found = this->get_header(key);
    if (found)
    {
      int it = 0;
      for (const auto &header : headers_)
      {
        if (header.key != found->key)
        {
          continue;
        }
        headers_.at(it) = *found;
        ++it;
      }
      return;
    }

    this->headers_.push_back({key, val});
  }

  auto response::get_header(const char *key) -> header *
  {
    if (std::strlen(key) == 0) return nullptr;
    for (const auto &item : headers_)
    {
      if (item.key != key)
      {
        continue;
      }

      return const_cast<header *>(&item);
    }

    return nullptr;
  }

  auto response::with_body(stream_interface::buffer body) -> void
  {
    stream_.write_bytes(body);
  }

  auto response::with_body(stream_interface::buffer *body) -> void
  {
    stream_.write_bytes(body);
  }

  auto response::get_body() -> stream_interface::buffer
  {
    return *stream_.read();
  }

  auto response::get_message() -> const char *
  {
    std::string http = proto_v_;
    http += " ";
    http += std::to_string(code_) + " " + reason_;
    http += CRLF;

    with_added_header("Content-Length", std::to_string(stream_.get_size()).c_str());

    for (const auto &header : headers_)
    {
      http += header.key;
      http += ":";
      http += header.value;
      http += CRLF;
    }

    http += CRLF;
    for (const auto &item : *stream_.read())
    {
      http += item;
    }

    int i{0};
    while (i < http.size())
    {
      msg_.push_back(http[i++]);
    }

    return this->msg_.c_str();
  }

  auto response::with_view(const char *p) -> void
  {
    const auto static_dir = std::filesystem::current_path().string() + "/public";
    const std::string path = static_dir + p;
    const auto is_regular_file = std::filesystem::is_regular_file(path);
    const auto is_file_exist = std::filesystem::exists(path);

    if (!is_regular_file || !is_file_exist)
    {
      return;
    }

    std::ifstream file(path, std::ios::binary);
    std::stringstream buf;
    buf << file.rdbuf();
    file.close();

    http::stream::buffer stream_buf;
    for (const auto &c : buf.str())
    {
      stream_buf.push_back(c);
    }

    this->with_body(&stream_buf);
  }
} // namespace http
