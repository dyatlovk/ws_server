#pragma once

#include "stream_interface.h++"

namespace http
{
  class stream : public stream_interface
  {
  public:
    stream();

    ~stream();

    auto get_size() -> const int override;

    auto write_bytes(const buffer *buf) -> const int override;

    auto write_bytes(const buffer buf) -> const int override;

    auto read() -> const buffer * override;

    auto get_contents() -> const char * override;

    auto reset() -> void override;

  private:
    buffer buf_;
    int size_;
  };

} // namespace http
