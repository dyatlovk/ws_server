#include <http/request.h++>
#include <http/response.h++>

#include "server.h++"

using request = http::request;
using response = http::response;

auto view(const std::string &p) -> const std::string;

int main(int argc, char *argv[])
{
  ::examples::server server{"127.0.0.1", 3044};
  server.add_route("/", request::methods::Get,
      [](const request *req) -> response
      {
        response res{200, "OK"};
        res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res.with_added_header("Content-Type", "text/html;charset=utf-8");
        res.with_view("/index.html");
        return res;
      });

  server.add_route("/about", request::methods::Get,
      [](const request *req) -> response
      {
        response res{200, "OK"};
        res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res.with_added_header("Content-Type", "text/html;charset=utf-8");
        res.with_view("/about.html");
        return res;
      });

  server.add_route("/faq", request::methods::Get,
      [](const request *req) -> response
      {
        response res{200, "OK"};
        res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res.with_added_header("Content-Type", "text/html;charset=utf-8");
        res.with_view("/faq.html");
        return res;
      });

  server.add_route("/api", request::methods::Post,
      [](const request *req) -> response
      {
        response res{200, "OK"};
        res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res.with_added_header("Content-Type", "application/json;charset=utf-8");
        res.with_body({'{', '"', 'v', 'e', 'r', 's', 'i', 'o', 'n', '"', ':', '"', '1', '"', '}'});
        return res;
      });

  server.run();
  server.shutdown();

  return 0;
}
