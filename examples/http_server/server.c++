#include "server.h++"

#include <atomic>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <http/parser.h++>
#include <http/request.h++>
#include <http/response.h++>
#include <io/epoll/epoll.h++>
#include <stdexcept>

#include "http/mime.h++"

namespace examples
{
  std::atomic<bool> server_running = false;
  typedef ::io::inet_socket sock;

  auto signal_handler(int s) -> void
  {
    server_running = 0;
  }

  server::server(const char *host, int port)
      : srv_(sock::make_tcp(host, port))
      , host_(host)
      , port_(port)
      , epoll_(new io::epoll())
      , static_dir_(std::filesystem::current_path().string() + "/public")
  {
    srv_->open();
    srv_->set_non_blocking();
    srv_->reuse_addr(true);
    srv_->reuse_port(true);
    srv_->bind();
    srv_->listen();

    const auto receive_buffer = srv_->get_opt(SOL_SOCKET, SO_RCVBUF);
    fmt::println("The socket receive buffer size is {}", receive_buffer);

    try
    {
      epoll_->create();
      epoll_->set_timeout(2000);
      epoll_->register_master(srv_->get_fd(), io::epoll::Events::READ);
    }
    catch (std::runtime_error &e)
    {
      fmt::println("{}", e.what());
      shutdown();
      return;
    }
    epoll_->on_connection([](int sock) { fmt::println("new connection {}", sock); });

    epoll_->on_write(
        [this](int socket, const char *buf)
        {
          http::parser parser{buf};
          auto req_line = parser.get_req_line();
          http::request req;
          auto request = req.parse(req_line);
          if (!request)
          {
            auto response = http::response::server_error();
            auto msg = response->get_message();
            srv_->write(socket, msg.c_str(), msg.size());
            epoll_->unwatch(socket);
            return;
          };

          { // serve static files
            const auto file = this->read_path(request->uri.c_str());
            if (!file.empty())
            {
              auto ext = std::filesystem::path(request->uri.c_str()).extension().string().erase(0, 1);
              http::mime mime;
              auto file_mime = mime.get_ext(ext);
              http::response res{200, "OK"};
              res.add_header("Server", ::examples::server::NAME);
              res.add_header("Content-Type", file_mime.mime.append(";charset=utf-8").c_str());
              res.add_header("Content-Length", std::to_string(file.size()).c_str());
              res.add_header("Cache-Control", "max-age=100,immutable");
              res.append_body(file.data(), file.size());
              auto msg = res.get_message();
              srv_->write(socket, msg.c_str(), msg.size());
              epoll_->unwatch(socket);
              return;
            }
          }

          // handle routers
          auto router = this->match_route(request->uri.c_str());
          if (!router)
          {
            const auto html = this->read_path("/404.html");
            http::response res{404, "Not Found"};
            res.add_header("Server", ::examples::server::NAME);
            res.add_header("Content-Type", "text/html;charset=utf-8");
            res.add_header("Content-Length", std::to_string(html.size()).c_str());
            res.append_body(html.data(), html.size());
            auto msg = res.get_message();
            srv_->write(socket, msg.c_str(), msg.size());
            epoll_->unwatch(socket);
            return;
          };

          if (router->method != request->method)
          {
            const auto html = this->read_path("/405.html");
            http::response res{405, "Not Allowed"};
            res.add_header("Server", ::examples::server::NAME);
            res.add_header("Content-Type", "text/html;charset=utf-8");
            res.add_header("Content-Length", std::to_string(html.size()).c_str());
            res.append_body(html.data(), html.size());
            auto msg = res.get_message();
            srv_->write(socket, msg.c_str(), msg.size());
            epoll_->unwatch(socket);
            return;
          }

          auto res = router->handler(&req);
          auto msg = res.get_message();
          srv_->write(socket, msg.c_str(), msg.size());
          epoll_->unwatch(socket);
        });

    epoll_->on_close([](int socket) { fmt::println("client closed {}", socket); });

    server_running = true;
  }

  server::~server()
  {
    for (const auto &router : routes_)
    {
      delete router;
    }
    if (epoll_) delete epoll_;
    if (srv_) delete srv_;
    fmt::println("server is shutting down");
  }

  auto server::run() -> void
  {
    // Catch user CTRL+C
    std::signal(SIGINT, &signal_handler);

    fmt::println("server listening on {host}:{port}", fmt::arg("host", host_), fmt::arg("port", port_));
    fmt::println("public dir: {}", static_dir_);

    while (server_running)
    {
      try
      {
        epoll_->wait();
      }
      catch (std::runtime_error &e)
      {
        fmt::println("exception reason: {}", e.what());
      }
    }
  }

  auto server::shutdown() -> void
  {
    server_running = false;
  }

  auto server::read_path(const std::string &p) -> const std::string
  {
    const std::string path = static_dir_ + p;
    const auto is_regular_file = std::filesystem::is_regular_file(path);
    const auto is_file_exist = std::filesystem::exists(path);

    if (!is_regular_file || !is_file_exist)
    {
      return "";
    }

    std::stringstream buf;
    std::ifstream file(path, std::ios::binary);
    buf << file.rdbuf();
    file.close();
    return buf.str();
  }

  auto server::add_route(const char *url, req::methods method, router_func &&handler) -> void
  {
    router *router_ = new router;
    router_->url = url;
    router_->method = method;
    router_->handler = std::move(handler);
    this->routes_.push_back(router_);
  }

  auto server::match_route(const char *url) -> router *
  {
    for (const auto &route : routes_)
    {
      if (strcmp(route->url, url) != 0)
      {
        continue;
      }

      return route;
    }
    return nullptr;
  }
} // namespace examples
