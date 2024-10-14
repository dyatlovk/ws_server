#pragma once

#include <map>
#include <string>
#include <vector>

namespace utils
{
  /**
   * Parse command options.
   *
   * Options structure: key=value (equal required).
   * Option key may contain any prefix or without it. (--key, -k or key).
   * Pairs group delimit with space.
   */
  class args
  {
  public:
    constexpr static int MAX_INPUT_DEFAUL = 32;

    args(int argc, char **argv);

    ~args();

    /**
     * Find parameter value.
     * If not empty string will be return.
     */
    auto find_option(const std::string &key) -> const std::string;

  private:
    using key_value = std::map<std::string, std::string>;

    /**
     * Find arguments and store pairs in map;
     */
    auto parse_options() -> void;

    /**
     * Split key_value line to 2 tokens
     */
    auto split_option(const std::string &option) -> const key_value;

  private:
    const int argc_;
    char **argv_;

    // Limit for argc variable
    int input_limit_;

    // storage for arguments
    std::vector<std::string> args_map_;

    // tokenized options (--option=value)
    std::vector<key_value> options_;
  };
} // namespace utils
