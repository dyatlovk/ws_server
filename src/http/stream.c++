#include "stream.h++"

namespace http
{
  stream::stream()
      : buf_()
      , size_(0)
  {
  }

  stream::~stream()
  {
    this->reset();
  }

  auto stream::get_size() -> const int
  {
    return this->size_;
  }

  auto stream::write_bytes(const buffer *buf) -> const int
  {
    this->buf_.reserve(buf->size() + 1);
    this->buf_ = *buf;
    this->buf_.push_back('\0');
    this->size_ = buf->size();
    return this->size_;
  }

  auto stream::write_bytes(const buffer buf) -> const int
  {
    this->buf_.reserve(buf.size() + 1);
    this->buf_ = buf;
    this->buf_.push_back({'\0'});
    this->size_ = buf.size();
    return this->size_;
  }

  auto stream::read() -> const buffer *
  {
    return &buf_;
  }

  auto stream::get_contents() -> const char *
  {
    if (buf_.empty()) return "";

    return buf_.data();
  }

  auto stream::reset() -> void
  {
    this->buf_.clear();
    this->size_ = 0;
  }
} // namespace http
