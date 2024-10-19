#include <http/request.h++>
#include <http/response.h++>

#include "server.h++"

using request = http::request;
using response = http::response;
using method = request::methods;

int main(int argc, char *argv[])
{
  ::examples::server server{"127.0.0.1", 3044};
  server.add_route("/", {method::Get, method::Post},
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

  server.add_route("/about", method::Get,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        res->with_view("/about.html");
      });

  server.add_route("/faq", method::Get,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "text/html;charset=utf-8");
        res->with_view("/faq.html");
      });

  server.add_route("/api", method::Post,
      [](request *req, response *res)
      {
        res->with_added_header("Server", ::examples::server::NAME_DEFAULT);
        res->with_added_header("Content-Type", "application/json;charset=utf-8");
        res->with_body({'{', '"', 'v', 'e', 'r', 's', 'i', 'o', 'n', '"', ':', '"', '1', '"', '}'});
      });

  server.run();
  server.shutdown();

  return 0;
}
