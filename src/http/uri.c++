#include "uri.h++"

#include "../stl/string/ws_string.h++"

namespace http
{
  uri::uri()
      : uri_({"", "", 0, "/", "", ""})
  {
  }

  uri::~uri() = default;

  auto uri::parse(const std::string &raw) -> Uri
  {
    // https://localhost:443/path?query#fragment
    if (raw.empty()) return uri_;
    auto input = ws_stl::trim_string(raw);

    uint8_t cursor = parse_scheme(input);
    auto semicl = input.find_first_of(colon);
    if (semicl > 0) input = input.substr(semicl + 1);

    cursor = parse_hier(input);
    if (input.size() <= cursor) return uri_;

    cursor += parse_query(input.substr(cursor));

    cursor += parse_fragment(input.substr(cursor));

    return uri_;
  }

  auto uri::parse_scheme(const std::string &raw) -> const uint8_t
  {
    auto input_ = ws_stl::trim_string(raw);

    uint16_t current = 0;
    if (!is_alpha(input_[current])) return 0;
    ++current;

    for (; current < input_.size(); ++current)
    {
      if (!is_scheme(input_[current])) break;
    }

    auto scheme = input_.substr(0, current);

    uri_.scheme = scheme;

    return current;
  }

  auto uri::parse_hier(const std::string &input) -> const uint8_t
  {
    // //localhost:443/path
    uint8_t cursor = 0;

    if (input[cursor] == slash)
    {
      ++cursor;
      if (input[cursor] == slash)
      {
        ++cursor;
        uint8_t aut_cursor = parse_authority(input.substr(cursor));
        if (aut_cursor == 0) return 0;
        cursor += aut_cursor;

        uint8_t path_cursor = parse_path(input.substr(cursor));
        if (path_cursor == 0) return 0;
        cursor += path_cursor;
      }
    }

    return cursor;
  }

  auto uri::parse_authority(const std::string &input) -> const uint8_t
  {
    // host.com:443/path
    auto cursor = 0;
    cursor += parse_host(input);
    if (cursor == 0) return 0;

    cursor += parse_port(input.substr(cursor));

    return cursor;
  }

  auto uri::parse_query(const std::string &input) -> const uint8_t
  {
    // ?key=value&test=1#index
    uint8_t cursor = 0;
    if (input[cursor] != question) return 0;
    ++cursor;

    for (; cursor < input.size(); ++cursor)
    {
      if (input[cursor] == number_sign) break;
    }

    if (cursor == 0) return 0;

    auto q = input.substr(0, cursor);
    int question_ = q.find_first_not_of(question);
    if (question_ > 0)
    {
      q = q.substr(question_);
    }

    uri_.query = q;

    return cursor;
  }

  auto uri::parse_fragment(const std::string &input) -> const uint8_t
  {
    // #index
    uint8_t cursor = 0;
    if (input[cursor] != number_sign) return 0;
    ++cursor;

    for (; cursor < input.size(); ++cursor)
    {
      if (!is_alpha(input[cursor]))
      {
        break;
      }
    }

    auto f = input.substr(0, cursor);
    int number = f.find_first_not_of(number_sign);
    if (number > 0)
    {
      f = f.substr(number);
    }

    uri_.fragment = f;

    return cursor;
  }

  auto uri::parse_host(const std::string &input) -> const uint8_t
  {
    // host.com:443/path
    uint8_t cursor = 0;
    if (!is_digit(input[cursor]))
    {
      cursor += parse_regname(input.substr(cursor));
    }

    return cursor;
  }

  auto uri::parse_regname(const std::string &input) -> const uint8_t
  {
    // host.com:443/path
    uint8_t cursor = 0;
    for (; cursor < input.size(); ++cursor)
    {
      if (input[cursor] == slash) break;
      if (input[cursor] == colon)
      {
        ++cursor;
        break;
      }
    }

    auto host = input.substr(0, cursor);
    int colon_ = host.find_last_of(colon);
    if (colon_ > 0)
    {
      host = host.substr(0, colon_);
    }

    uri_.host = host;

    return cursor;
  }

  auto uri::parse_port(const std::string &input) -> const uint8_t
  {
    // 443/path
    uint8_t cursor = 0;
    bool is_valid = false;
    for (; cursor < input.size(); ++cursor)
    {
      if (is_digit(input[cursor])) is_valid = true;
      if (input[cursor] == slash) break;
    }

    if (!is_valid)
    {
      uri_.port = 0;
      return cursor;
    }

    if (cursor == 0)
    {
      uri_.port = 0;
      return cursor;
    }


    auto p = input.substr(0, cursor);
    uint16_t port = 0;

    try
    {
      port = std::stoi(p);
    }
    catch (std::invalid_argument &e)
    {
    }
    catch (std::out_of_range &e)
    {
    }

    uri_.port = port;

    return cursor;
  }

  auto uri::parse_path(const std::string &input) -> const uint8_t
  {
    if (input.empty()) return 0;

    // /path
    uint8_t cursor = 0;
    if (input[cursor] != slash) return 0;
    ++cursor;

    for (; cursor < input.size(); cursor++)
    {
      if (input[cursor] == question || input[cursor] == number_sign)
      {
        break;
      }
    }

    auto path = input.substr(0, cursor);
    uri_.path = path;

    return cursor;
  }

  auto uri::is_alpha(const char &view) -> const bool
  {
    switch (view)
    {
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5A:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
    case 0x6C:
    case 0x6D:
    case 0x6E:
    case 0x6F:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A: return true;
    default: return false;
    }
  }

  auto uri::is_digit(const char &view) -> const bool
  {
    switch (view)
    {
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39: return true;
    default: return false;
    }
  }

  auto uri::is_scheme(const val_type &view) -> const bool
  {
    switch (view)
    {
    case minus:
    case dot:
    case plus:
    case tilde: return true;
    default: return (is_alpha(view) || is_digit(view));
    }
  }
} // namespace http
