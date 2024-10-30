#pragma once

#include <list>
#include <minijson/json.h>

namespace Models
{
  class Blog
  {
  public:
    struct article
    {
      int id;
      const char *title;
      const char *summary;
      const char *text;
    };
    using json = miniJson::Json;
    using articles = std::list<article>;

  public:
    Blog()
    {
      this->list_ = {
          {1, "The day was a canvas",
              "<p>Oh, the day was a canvas, brushed with light, Morning whispers, gold-tipped and bright. Time wove "
              "its threads in a delicate dance, Moments like raindrops, a fleeting romance.</p>",
              "<p>The sun stretched its arms, embracing the sky, While shadows played hide and seek as they sigh. With "
              "laughter like wind chimes, ringing so clear,</p>"},
          {2, "Yet within the bright tapestry",
              "<p>Yet within the bright tapestry, shadows would creep, In corners of thoughts where the silence could "
              "weep. For joy and for sorrow, entwined they reside, Both woven together, like the earth and the "
              "tide.</p>",
              "So, how was my day? A symphony vast, A blend of the present, the future, the past. In the ebb and the "
              "flow, a story unfolds, Each moment a treasure, each heartbeat a gold."},
          {3, "A symphony vast",
              "<p>So, how was my day? A symphony vast, A blend of the present, the future, the past. In the ebb and "
              "the flow, a story unfolds,</p>",
              "<p>Morning whispers, gold-tipped and bright. Time wove its threads in a delicate dance</p>"},
      };
    }

    ~Blog() = default;

    inline auto get_articles_list() -> articles { return this->list_; }

    inline auto get_articles() -> const json
    {
      const auto list = get_articles_list();
      json json = miniJson::Json::_array{};
      for (const auto &article : list)
      {
        auto arr = json.toArray();
        const auto object = json::_object{
            {"id", article.id}, {"text", article.text}, {"summary", article.summary}, {"title", article.title}};
        arr.push_back(object);
        json = arr;
      }

      return json;
    };

    inline auto find_one(const int id) -> const json
    {
      for (const auto &item : list_)
      {
        if (item.id == id)
        {
          const auto object =
              json::_object{{"id", item.id}, {"text", item.text}, {"summary", item.summary}, {"title", item.title}};
          return object;
        }
      }

      return json::_object{};
    }

    inline auto find_one(const char *title) -> const json
    {
      json json = miniJson::Json::_array{};

      for (const auto &item : list_)
      {
        if (item.title == title)
        {
          const auto object =
              json::_object{{"id", item.id}, {"text", item.text}, {"summary", item.summary}, {"title", item.title}};
          return object;
        }
      }

      return nullptr;
    }

    inline auto add(const article article) -> void { list_.push_back(article); }

    inline auto remove(const char *title) -> void
    {
      for (auto it = list_.begin(); it != list_.end(); ++it)
      {
        if (it->title == title)
        {
          it = list_.erase(it);
          --it;
        }
      }
    }

  private:
    articles list_;
  };
} // namespace Models
