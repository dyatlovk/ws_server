#pragma once

#include "message_interface.h++"

namespace http
{
  struct response_interface : public message_interface
  {
  public:
    /**
     * Gets the response status code.
     * RFC 7231 Section 6
     */
    virtual auto get_status_code() -> int = 0;

    /**
     * Return an instance with the specified status code and, optionally, reason phrase
     */
    virtual auto with_status(int code, const char *reason = "") -> response_interface * = 0;

    /**
     * Gets the response reason phrase associated with the status code.
     */
    virtual auto get_reason_phrase() -> const char * = 0;

    /**
     * Return message builded message
     */
    virtual auto get_message() -> const char * = 0;

    virtual auto with_redirect(const char *location, const int code, const char *reason) -> void = 0;
  };
} // namespace http
