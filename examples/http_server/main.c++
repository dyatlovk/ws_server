#include <filesystem>
#include <fmt/core.h>
#include <http/request.h++>
#include <http/response.h++>

#include "server.h++"

using request = http::request;

int main(int argc, char *argv[])
{
  ::examples::server server{"127.0.0.1", 3044};
  const std::string public_dir = std::filesystem::current_path().string() + "/public";
  server.serve_static(public_dir.c_str());
  server.add_route("/", request::methods::Get,
      [](const http::request *req) -> http::response
      {
        fmt::println("router match: {uri}", fmt::arg("uri", req->http_req_.uri));
        std::string content = "Home page";
        http::response res{200, "OK"};
        res.add_header("Server", "WS");
        res.add_header("Content-Type", "text/html;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.add_route("/about", request::methods::Get,
      [](const http::request *req) -> http::response
      {
        fmt::println("router match: {uri}", fmt::arg("uri", req->http_req_.uri));
        std::string content = "About page";
        http::response res{200, "OK"};
        res.add_header("Server", "WS");
        res.add_header("Content-Type", "text/html;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.run();
  server.shutdown();

  return 0;
}
