#pragma once

#include <vector>

#include "stream_interface.h++"

namespace http
{
  struct message_interface
  {
  public:
    struct header;
    // typedef std::vector<char> stream;
    typedef std::vector<header> key_value;

  public:
    /**
     * Retrieves the HTTP protocol version. (1.0, 1.1, etc.)
     */
    virtual auto get_proto_ver() -> const char * = 0;

    /**
     * Return an instance with the specified HTTP protocol version
     */
    virtual auto with_proto_ver(const char *ver) -> void = 0;

    /*
     * Retrieves all headers value.
     */
    virtual auto get_headers() -> key_value = 0;

    /**
     * Check if header exist.
     */
    virtual auto has_header(const char *key) -> bool = 0;

    /**
     * Return an instance with the provided value replacing the specified header.
     */
    virtual auto with_header(const char *key, const char *val) -> void = 0;

    /**
     * Return an instance with the specified header appended with the given value.
     * All keys are unique. Duplicate keys will be replacing with a new value.
     */
    virtual auto with_added_header(const char *key, const char *val) -> void = 0;

    /**
     * Retrieves a message header value by name.
     */
    virtual auto get_header(const char *key) -> header * = 0;

    /**
     * Gets the body of the message.
     */
    virtual auto get_body() -> stream_interface::buffer = 0;

    /**
     * Return an instance with the specified message body.
     */
    virtual auto with_body(const stream_interface::buffer body) -> void = 0;
    virtual auto with_body(const stream_interface::buffer *body) -> void = 0;
    virtual auto with_body(const char *body) -> void = 0;

  public:
    struct header
    {
      const char *key;
      const char *value;
    };
  };
} // namespace http
