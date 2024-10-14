#include "../public/args/parser.h++"

#include <cstring>
#include <sstream>
#include <stdexcept>

namespace utils
{
  args::args(int argc, char **argv)
      : argc_(argc)
      , argv_(argv)
      , args_map_()
      , options_()
      , input_limit_(MAX_INPUT_DEFAUL)
  {
    if (argc > input_limit_)
    {
      throw std::runtime_error("too many input parameters");
    }

    args_map_.assign(argv_ + 1, argv_ + argc_);
    parse_options();
  }

  args::~args() = default;

  auto args::find_option(const std::string &key) -> const std::string
  {
    for (const auto &pair : options_)
    {
      auto found = pair.begin()->first == key;
      if (!found) continue;
      return pair.begin()->second;
    }
    return "";
  }

  auto args::parse_options() -> void
  {
    for (const auto &arg : args_map_)
    {
      auto is_option = arg.starts_with("--");
      if (is_option)
      {
        auto pair = this->split_option(arg);
        if (!pair.empty())
        {
          options_.push_back(pair);
        }
      }
    }
  }

  auto args::split_option(const std::string &option) -> const key_value
  {
    std::istringstream stream(option);
    std::string key, value;
    std::getline(stream, key, '=');
    stream >> std::ws;

    while (std::getline(stream >> std::ws, value))
    {
      key_value pair;
      pair.emplace(key, value);
      return pair;
    }

    return {};
  }

} // namespace utils
