#pragma once

#include <minijson/json.h>
#include <string>

#include "response_interface.h++"

namespace http
{
  class response final : public response_interface
  {
  public:
    static constexpr const char *CRLF = "\r\n";
    constexpr static const char *PROTO_PREFIX = "HTTP/";
    constexpr static const char *PROTO_DEFAULT = "HTTP/1.1";

  public:
    response(int code, const char *reason);

    ~response();

    auto get_status_code() -> int override;

    auto with_status(int code, const char *reason = "") -> response * override;

    auto get_reason_phrase() -> const char * override { return reason_; }

    auto get_proto_ver() -> const char * override { return proto_v_.c_str(); }

    /**
     * Version without prefix (HTTP/)
     */
    auto with_proto_ver(const char *ver) -> void override;

    auto get_headers() -> key_value override { return headers_; }

    auto has_header(const char *key) -> bool override;

    auto with_header(const char *key, const char *val) -> void override;

    auto with_added_header(const char *key, const char *val) -> void override;

    auto get_header(const char *key) -> header * override;

    auto get_body() -> stream_interface::buffer override;

    auto with_body(const stream_interface::buffer body) -> void override;
    auto with_body(const stream_interface::buffer *body) -> void override;
    auto with_body(const char *data) -> void override;

    auto get_message() -> const char * override;

  public:
    auto with_view(const char *p) -> void;

    auto with_redirect(const char *location, const int code = 303, const char *phrase = "See Other") -> void override;

    auto with_json(const miniJson::Json *data) -> void;

  private:
    int code_;
    std::string msg_;
    key_value headers_;
    const char *reason_;
    std::string proto_v_;
  };
} // namespace http
