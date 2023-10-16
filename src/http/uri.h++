#pragma once

#include <string>

namespace http
{
  struct Uri
  {
    std::string scheme;
    std::string host;
    uint16_t port;
    std::string path = "/";
    std::string query;
    std::string fragment;
  };

  struct uri
  {
  public:
    typedef std::string_view::value_type val_type;

    uri();

    ~uri();

    /**
     *   foo://example.com:8042/over/there?name=ferret#nose
     *   \_/   \______________/\_________/ \_________/ \__/
     *    |           |            |            |        |
     * scheme     authority       path        query   fragment
     *    |   _____________________|__
     *   / \ /                        \
     *   urn:example:animal:ferret:nose
     */
    auto parse(const std::string &raw) -> Uri;

    // RFC 3986/3.1
    // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    auto parse_scheme(const std::string &input) -> const uint8_t;

    // RFC 3986/3
    auto parse_hier(const std::string &input) -> const uint8_t;

    // RFC 3986/3.2
    auto parse_authority(const std::string &input) -> const uint8_t;

    // RFC 3986/3.4
    auto parse_query(const std::string &input) -> const uint8_t;

    // RFC 3986/3.5
    auto parse_fragment(const std::string &input) -> const uint8_t;

    // RFC 3986 Section 3.2.2
    auto parse_host(const std::string &input) -> const uint8_t;

    // RFC 3986 Section 3.3
    auto parse_port(const std::string &input) -> const uint8_t;

    // if host as domain name
    auto parse_regname(const std::string &input) -> const uint8_t;

    // RFC 3986 Section 3.3
    auto parse_path(const std::string &input) -> const uint8_t;

  private:
    static auto is_alpha(const val_type &view) -> const bool;
    static auto is_digit(const val_type &view) -> const bool;
    static auto is_scheme(const val_type &view) -> const bool;

  private:
    constexpr static val_type exclamation = '!';
    constexpr static val_type parenthesis_open = '(';
    constexpr static val_type parenthesis_close = ')';
    constexpr static val_type less_than = '<';
    constexpr static val_type greater_than = '>';
    constexpr static val_type at = '@';
    constexpr static val_type comma = ',';
    constexpr static val_type semicolon = ';';
    constexpr static val_type colon = ':';
    constexpr static val_type backslash = '\\';
    constexpr static val_type doublequotes = '"';
    constexpr static val_type slash = '/';
    constexpr static val_type bracket_sq_open = '[';
    constexpr static val_type bracket_sq_close = ']';
    constexpr static val_type question = '?';
    constexpr static val_type equal = '=';
    constexpr static val_type bracket_open = '{';
    constexpr static val_type bracket_close = '}';
    constexpr static val_type minus = '-';
    constexpr static val_type plus = '+';
    constexpr static val_type dot = '.';
    constexpr static val_type underscore = '_';
    constexpr static val_type tilde = '~';
    constexpr static val_type asterisk = '*';
    constexpr static val_type dollar = '$';
    constexpr static val_type number_sign = '#';

  private:
    Uri uri_;
  };
} // namespace http
