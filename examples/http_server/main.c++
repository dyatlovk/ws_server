#include <http/middlewares/response.h++>
#include <http/server.h++>
#include <vector>

using request = http::request;
using response = http::response;
using method = request::methods;
using router = http::router;

auto get_options() -> http::server::options;

int main(int argc, char *argv[])
{
  router router;
  router.add("/", {method::Get, method::Post},
      [](request *req, response *res)
      {
        if (req->req.method == method::Post)
        {
          res->with_redirect("/about");
          return;
        }
        res->with_view("/index.html");
      });

  router.add("/about", method::Get, [](request *req, response *res) { res->with_view("/about.html"); });
  router.add("/about/\\d+", method::Get, [](request *req, response *res) { res->with_view("/about.html"); });

  router.add("/faq", method::Get, [](request *req, response *res) { res->with_view("/faq.html"); });

  router.add("/api", method::Post,
      [](request *req, response *res)
      {
        res->with_added_header("Content-Type", "application/json;charset=utf-8");

        miniJson::Json json = miniJson::Json::_object{
            {"key1", "value1"},
            {"key2", false},
            {"key3", miniJson::Json::_array{1, 2, 3}},
        };
        res->with_json(&json);
      });

  router.add("/params/\\D+", method::Get,
      [](request *req, response *res)
      {
        const auto params = req->req.params;
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

  const auto options = get_options();
  http::server app(&options);
  http::middlewares::response response_middleware;
  app.add_middleware(&response_middleware);
  app.with_routers(&router);
  const auto exit_code = app.listen();

  router.shutdown();

  return exit_code;
}

auto get_options() -> http::server::options
{
  http::server::options options;
  options.port = 3044;
  options.host = "127.0.0.1";
  options.name = "Black Mesa";
  options.public_dir = "/public";

  return options;
}
