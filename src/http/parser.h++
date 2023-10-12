#pragma once
#include <string>
#include <vector>

namespace http
{
  struct parser
  {
  public:
    // resize input buffer to this limit
    constexpr static int BUFFER_LIMIT = 1024 * 1000;

    parser(const char *str);

    ~parser();

    auto tokenize() -> void;

    auto get_body() -> const std::string & { return body_; };

    auto find_body() -> const std::string &;

    auto get_req_line() -> const std::string &;

    auto get_headers() -> std::vector<std::string>;

    auto is_buffer_valid() -> bool { return is_buf_valid; }

    auto clear_buffer() -> void;

  private:
    /**
     * @brief      Split a body and headers
     *
     * @param[in]  header_limit  Limit headers size in bytes. Parser will be
     *                           breaking splitting after this limit
     *
     * @return     void
     */
    auto split_body(const int header_limit = 1024 * 50) -> void;

    /**
     * @brief      Split by lines and store in container
     *
     * @return     void
     */
    auto tokenize_headers() -> void;

  private:
    // input buffer
    std::string input_;

    // raw header
    // Key: value
    std::string header_;
    int head_end_pos_; // last char include

    // raw headers line
    std::vector<std::string> headers_raw_;

    // request line
    // GET /uri HTTP/2
    std::string req_line_;

    // response body
    std::string body_;
    int body_pos_; // first char include

    bool is_buf_valid;
  };
} // namespace http
