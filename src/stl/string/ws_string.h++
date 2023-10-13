#pragma once

#include <sstream>
#include <string>
#include <unordered_map>

namespace ws_stl
{
  // {key, value}
  typedef std::unordered_map<std::string, std::string> split_pairs;

  inline auto trim_string(const std::string &str) -> std::string const
  {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
      return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
  }

  inline auto split_string(const std::string &input, char delim) -> split_pairs
  {
    split_pairs m;
    std::istringstream stream(input);
    std::string key, value;

    std::getline(stream, key, delim);
    stream >> std::ws;
    while (std::getline(stream >> std::ws, value))
    {
      auto k_trimmed = trim_string(key);
      auto v_trimmed = trim_string(value);
      m.insert({k_trimmed, v_trimmed});
    }

    return m;
  }
} // namespace ws_stl
