#include "parser.h++"

#include <algorithm>

#include "../stl/string/ws_string.h++"

namespace http
{
  parser::parser(const char *str)
      : input_(str)
      , body_("")
      , header_("")
      , req_line_("")
      , is_buf_valid(false)
      , body_pos_(0)
      , head_end_pos_(0)
  {
    // Reserve space for efficiency but don't resize to avoid corrupting input
    input_.reserve(BUFFER_LIMIT);
    tokenize();
  }

  parser::~parser()
  {
    clear_buffer();
  }

  auto parser::tokenize() -> void
  {
    try
    {
      split_body();
      if (head_end_pos_ > 0) is_buf_valid = true;
    }
    catch (std::out_of_range &e)
    {
    }

    if (!is_buf_valid) return;

    tokenize_headers();
  }

  auto parser::get_headers() -> head_container
  {
    return headers_;
  }

  auto parser::get_req_line() -> const std::string &
  {
    return req_line_;
  }

  auto parser::find_body() -> const std::string &
  {
    body_.clear(); // Clear previous body content
    const size_t input_len = input_.length();

    for (size_t i = static_cast<size_t>(body_pos_); i < input_len; i++)
    {
      body_ += input_[i];
    }

    return body_;
  }

  auto parser::split_body(const int header_limit) -> void
  {
    const size_t input_len = input_.length();
    const size_t limit = std::min(static_cast<size_t>(header_limit), input_len);

    for (size_t i = 0; i < limit; i++)
    {
      header_ += input_.at(i);
      // found crlf - check bounds before accessing next characters
      if (input_.at(i) == '\r' && (i + 3) < input_len)
      {
        if (input_.at(i + 1) == '\n' && input_.at(i + 2) == '\r' && input_.at(i + 3) == '\n')
        {
          // Include the \n after the first \r to complete the last header line
          header_ += input_.at(i + 1);
          head_end_pos_ = static_cast<int>(i + 1);
          body_pos_ = static_cast<int>(i + 4);
          break;
        }
      }
    }
  }

  auto parser::tokenize_headers() -> void
  {
    std::string line;
    bool first_line = true;

    // Process the header_ content that was built by split_body()
    for (size_t i = 0; i < header_.length(); i++)
    {
      char current_char = header_.at(i);

      if (current_char != '\r' && current_char != '\n')
      {
        line += current_char;
      }
      else if (current_char == '\r' && i + 1 < header_.length() && header_.at(i + 1) == '\n')
      {
        // Found CRLF - process the line
        if (first_line)
        {
          req_line_ = line;
          first_line = false;
        }
        else if (!line.empty()) // Only process non-empty lines as headers
        {
          auto kv = ws_stl::split_string(line, ':');
          if (kv.size() > 0)
          {
            headers_.insert({kv.begin()->first, kv.begin()->second});
          }
        }
        line.clear();
        i++; // Skip the \n character
      }
    }

    // Process any remaining line at the end (in case there's no trailing CRLF)
    if (!line.empty() && !first_line)
    {
      auto kv = ws_stl::split_string(line, ':');
      if (kv.size() > 0)
      {
        headers_.insert({kv.begin()->first, kv.begin()->second});
      }
    }
  }

  auto parser::clear_buffer() -> void
  {
    input_.clear();
    req_line_.clear();
    body_.clear();
    is_buf_valid = false;
  }
} // namespace http
