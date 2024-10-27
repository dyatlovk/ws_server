#include <http/request.h++>
#include <http/response.h++>
#include <http/router.h++>
#include <minijson/json.h>
#include <vector>

#include "server.h++"

using request = http::request;
using response = http::response;
using method = request::methods;
using router = http::router;

int main(int argc, char *argv[])
{
  router router;
  router.add("/", {method::Get, method::Post},
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        if (req->http_req_.method == method::Post)
        {
          res->with_redirect("/about");
          return;
        }
        res->with_view("/index.html");
      });

  router.add("/about", method::Get,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        res->with_view("/about.html");
      });
  router.add("/about/\\d+", method::Get,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        res->with_view("/about.html");
      });

  router.add("/faq", method::Get,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        res->with_view("/faq.html");
      });

  router.add("/api", method::Post,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "application/json;charset=utf-8");

        miniJson::Json json = miniJson::Json::_object{
            {"key1", "value1"},
            {"key2", false},
            {"key3", miniJson::Json::_array{1, 2, 3}},
        };
        auto str = json.serialize();

        std::vector<char> body{};
        for (const auto &c : str)
        {
          body.push_back(c);
        }
        res->with_body(body);
      });

  router.add("/params/\\D+", method::Get,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        const auto params = req->http_req_.params;
        std::vector<char> body{};
        for (const auto &c : params)
        {
          for (const auto &a : c)
          {
            body.push_back(a);
          }
        }
        res->with_body(body);
      });

  ::examples::server server{"127.0.0.1", 3044};
  server.with_routers(&router);
  server.run();
  server.shutdown();
  router.shutdown();

  return 0;
}
