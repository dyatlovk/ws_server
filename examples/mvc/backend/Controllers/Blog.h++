#pragma once

#include <http/request.h++>
#include <http/response.h++>

#include "../Models/Blog.h++"

namespace Controllers
{
  class Blog
  {
    using json = miniJson::Json;
    using request = http::request;
    using response = ::http::response;

  public:
    inline Blog()
        : model(new Models::Blog)
    {
    }

    inline ~Blog() { delete model; }

    inline auto list(request *req, response *res) -> response
    {
      res->with_added_header("Server", "Server Name");
      res->with_added_header("Content-Type", "application/json;charset=utf-8");
      auto list = this->model->get_articles();
      res->with_json(&list);

      return *res;
    }

    inline auto entry(request *req, response *res) -> response
    {
      res->with_added_header("Server", "Server Name");
      res->with_added_header("Content-Type", "application/json;charset=utf-8");

      auto params = req->http_req_.params;
      if (params.empty())
      {
        res->with_status(404, "Not Found");
        json j = miniJson::Json::_object{{"error", "404"}};
        res->with_json(&j);
        return *res;
      }

      const int id = atoi(params[0].c_str());
      auto article = this->model->find_one(id);
      if (article.size() <= 0)
      {
        res->with_status(404, "Not Found");
        json j = miniJson::Json::_object{{"error", "404"}};
        res->with_json(&j);
        return *res;
      }
      res->with_json(&article);
      return *res;
    }

  private:
    Models::Blog *model;
  };
} // namespace Controllers
