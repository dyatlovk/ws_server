#pragma once

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <string_view>

namespace utils
{

  /**
   * @brief Log levels for the logging system
   */
  enum class LogLevel
  {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
  };

  /**
   * @brief Simple logging utility using fmt library
   *
   * This logger provides colored output with timestamps and log levels.
   * It uses the fmt library for efficient formatting and supports
   * different log levels with appropriate styling.
   */
  class Logger
  {
  public:
    /**
     * @brief Set the minimum log level to display
     * @param level Minimum log level (DEBUG, INFO, WARN, ERROR)
     */
    static void set_level(LogLevel level) { min_level_ = level; }

    /**
     * @brief Get the current minimum log level
     * @return Current minimum log level
     */
    static LogLevel get_level() { return min_level_; }

    /**
     * @brief Log a debug message
     * @param format Format string (fmt style)
     * @param args Arguments for the format string
     */
    template<typename... Args>
    static void debug(fmt::format_string<Args...> format, Args &&...args)
    {
      log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log an info message
     * @param format Format string (fmt style)
     * @param args Arguments for the format string
     */
    template<typename... Args>
    static void info(fmt::format_string<Args...> format, Args &&...args)
    {
      log(LogLevel::INFO, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log a warning message
     * @param format Format string (fmt style)
     * @param args Arguments for the format string
     */
    template<typename... Args>
    static void warn(fmt::format_string<Args...> format, Args &&...args)
    {
      log(LogLevel::WARN, format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log an error message
     * @param format Format string (fmt style)
     * @param args Arguments for the format string
     */
    template<typename... Args>
    static void error(fmt::format_string<Args...> format, Args &&...args)
    {
      log(LogLevel::ERROR, format, std::forward<Args>(args)...);
    }

  private:
    static LogLevel min_level_;

    /**
     * @brief Internal logging function
     * @param level Log level of this message
     * @param format Format string
     * @param args Arguments for the format string
     */
    template<typename... Args>
    static void log(LogLevel level, fmt::format_string<Args...> format, Args &&...args)
    {
      if (level < min_level_)
      {
        return;
      }

      auto now = std::chrono::system_clock::now();
      auto message = fmt::format(format, std::forward<Args>(args)...);

      // Choose output stream based on log level
      auto *stream = (level >= LogLevel::WARN) ? stderr : stdout;

      // Format and print the log message with color
      fmt::print(stream, "{} {} {}\n", get_timestamp(now), get_level_string(level), message);
    }

    /**
     * @brief Get formatted timestamp string
     * @param time_point Time point to format
     * @return Formatted timestamp string
     */
    static std::string get_timestamp(const std::chrono::system_clock::time_point &time_point)
    {
      return fmt::format("[{}]", fmt::format("{:%Y-%m-%d %H:%M:%S}", time_point));
    }

    /**
     * @brief Get colored level string for the given log level
     * @param level Log level
     * @return Formatted and colored level string
     */
    static std::string get_level_string(LogLevel level)
    {
      switch (level)
      {
      case LogLevel::DEBUG: return fmt::format(fmt::fg(fmt::color::cyan), "[DEBUG]");
      case LogLevel::INFO: return fmt::format(fmt::fg(fmt::color::green), "[INFO ]");
      case LogLevel::WARN: return fmt::format(fmt::fg(fmt::color::yellow), "[WARN ]");
      case LogLevel::ERROR: return fmt::format(fmt::fg(fmt::color::red), "[ERROR]");
      default: return "[UNKNOWN]";
      }
    }
  };

} // namespace utils

// Convenience macros for easier logging
#define LOG_DEBUG(...) utils::Logger::debug(__VA_ARGS__)
#define LOG_INFO(...) utils::Logger::info(__VA_ARGS__)
#define LOG_WARN(...) utils::Logger::warn(__VA_ARGS__)
#define LOG_ERROR(...) utils::Logger::error(__VA_ARGS__)
