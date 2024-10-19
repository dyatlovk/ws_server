#pragma once

#include <vector>

namespace http
{
  struct stream_interface
  {
  public:
    typedef std::vector<char> buffer;

  public:
    /**
     * Get the size of the stream.
     */
    virtual auto get_size() -> const int = 0;

    /**
     * Write data to the buffer.
     */
    virtual auto write_bytes(const buffer *buf) -> const int = 0;

    virtual auto write_bytes(const buffer buf) -> const int = 0;

    /**
     * Read data from the buffer.
     */
    virtual auto read() -> const buffer * = 0;

    /**
     * Returns contents.
     */
    virtual auto get_contents() -> const char * = 0;

    /**
     * Reset all data in container
     */
    virtual auto reset() -> void = 0;
  };
} // namespace http
