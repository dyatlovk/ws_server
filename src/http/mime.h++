#pragma once

#include <string>
#include <vector>

namespace http
{
  struct type
  {
    std::string ext;
    std::string mime;
  };

  struct mime
  {
  public:
    static constexpr type MIME_DEFAULT = {"html", "text/html"};

  public:
    mime()
        : db_()
    {
      init();
    }

    ~mime() {}

    inline auto init() -> void
    {
      this->add("css", "text/css");
      this->add("html", "text/html");
      this->add("js", "text/javascript");
      this->add("txt", "text/plain");

      this->add("jpg", "image/jpeg");
      this->add("jpeg", "image/jpeg");
      this->add("png", "image/png");
      this->add("svg", "image/svg+xml");

      this->add("json", "application/json");
    }

    inline auto add(const std::string &ext, const std::string &mime) -> void { db_.push_back({ext, mime}); }

    inline auto get_ext(const std::string &ext) -> type
    {
      for (const auto &mime : db_)
      {
        if (mime.ext == ext)
        {
          return mime;
        }
      }
      return MIME_DEFAULT;
    }

  private:
    std::vector<type> db_;
  };

} // namespace http
