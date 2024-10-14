#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <http/request.h++>
#include <http/response.h++>

#include "server.h++"

using request = http::request;
using response = http::response;

auto view(const std::string &p) -> const std::string;

int main(int argc, char *argv[])
{
  ::examples::server server{"127.0.0.1", 3044};
  const auto static_dir = server.get_static_dir();
  server.add_route("/", request::methods::Get,
      [&static_dir](const request *req) -> response
      {
        std::string content = view(static_dir + "/index.html");
        response res{200, "OK"};
        res.add_header("Server", ::examples::server::NAME);
        res.add_header("Content-Type", "text/html;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.add_route("/about", request::methods::Get,
      [&static_dir](const request *req) -> response
      {
        std::string content = view(static_dir + "/about.html");
        response res{200, "OK"};
        res.add_header("Server", ::examples::server::NAME);
        res.add_header("Content-Type", "text/html;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.add_route("/api", request::methods::Post,
      [](const request *req) -> response
      {
        const std::string content = "{\"version\": \"0.2.0\"}";
        response res{200, "OK"};
        res.add_header("Server", ::examples::server::NAME);
        res.add_header("Content-Type", "application/json;charset=utf-8");
        res.add_header("Content-Length", std::to_string(content.size()).c_str());
        res.append_body(content.data(), content.size());
        return res;
      });

  server.run();
  server.shutdown();

  return 0;
}

auto view(const std::string &p) -> const std::string
{
  const auto is_regular_file = std::filesystem::is_regular_file(p);
  const auto is_file_exist = std::filesystem::exists(p);
  auto ext = std::filesystem::path(p).extension().string().erase(0, 1);
  const auto is_html = ext == "html";

  if (!is_regular_file || !is_file_exist || !is_html)
  {
    return "";
  }

  std::stringstream buf;
  std::ifstream file(p, std::ios::binary);
  buf << file.rdbuf();
  file.close();
  return buf.str();
}
