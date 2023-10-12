#include "parser.h++"

#include <cstdio>

#include "fmt/core.h"

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
    input_.resize(BUFFER_LIMIT);
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

  auto parser::get_headers() -> std::vector<std::string>
  {
    return headers_raw_;
  }

  auto parser::get_req_line() -> const std::string &
  {
    return req_line_;
  }

  auto parser::find_body() -> const std::string &
  {
    for (int i = 0; i <= input_.length(); i++)
    {
      if (i >= body_pos_)
      {
        char ch[1] = {input_[i]};
        body_.append(ch);
      }
    }
    return body_;
  }

  auto parser::split_body(const int header_limit) -> void
  {
    for (int i = 0; i <= header_limit; i++)
    {
      header_ += input_.at(i);
      // found crlf
      if (input_.at(i) == '\r' && input_.at(i + 1) == '\n')
      {
        if (input_.at(i + 2) == '\r' && input_.at(i + 3) == '\n')
        {
          head_end_pos_ = i + 1;
          body_pos_ = i + 4;
          break;
        }
      }
    }
  }

  auto parser::tokenize_headers() -> void
  {
    const std::string b(input_);
    std::string line;
    for (int i = 0; i <= head_end_pos_; i++)
    {
      if (i + 1 <= head_end_pos_)
      {
        char ch[1] = {b.at(i)};
        if (b[i] != '\r' && b[i] != '\n')
        {
          line.append(ch);
        }
        if (b[i] == '\r' && b[i + 1] == '\n') // found eol
        {
          headers_raw_.push_back(line);
          line.clear(); // empty line for next iterator
        }
      }
    }

    // get request line and remove from headers container
    if (headers_raw_.size() > 0) req_line_ = headers_raw_.front();
    headers_raw_.erase(headers_raw_.begin() + 0);
  }

  auto parser::clear_buffer() -> void
  {
    input_.clear();
    req_line_.clear();
    body_.clear();
    is_buf_valid = false;
  }
} // namespace http
