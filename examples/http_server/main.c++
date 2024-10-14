#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <http/request.h++>
#include <http/response.h++>

#include "server.h++"

using request = http::request;

auto read_path(const std::string &p) -> const std::string;

int main(int argc, char *argv[])
{
  ::examples::server server{"127.0.0.1", 3044};
  const auto statid_dir = server.get_static_dir();
  server.add_route("/", request::methods::Get,
      [&statid_dir](const http::request *req) -> http::response
      {
        fmt::println("router match: {uri}", fmt::arg("uri", req->http_req_.uri));
        std::string content = read_path(statid_dir + "/index.html");
        http::response res{200, "OK"};
        res.add_header("Server", ::examples::server::SERVER_NAME);
        res.add_header("Content-Type", "text/html;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.add_route("/about", request::methods::Get,
      [&statid_dir](const http::request *req) -> http::response
      {
        fmt::println("router match: {uri}", fmt::arg("uri", req->http_req_.uri));
        std::string content = read_path(statid_dir + "/about.html");
        http::response res{200, "OK"};
        res.add_header("Server", ::examples::server::SERVER_NAME);
        res.add_header("Content-Type", "text/html;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.run();
  server.shutdown();

  return 0;
}

auto read_path(const std::string &p) -> const std::string
{
  const auto is_regular_file = std::filesystem::is_regular_file(p);
  const auto is_file_exist = std::filesystem::exists(p);

  if (!is_regular_file || !is_file_exist)
  {
    return "";
  }

  std::stringstream buf;
  std::ifstream file(p, std::ios::binary);
  buf << file.rdbuf();
  file.close();
  return buf.str();
}
