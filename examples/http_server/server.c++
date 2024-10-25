#include "server.h++"

#include <atomic>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <fmt/core.h>
#include <http/mime.h++>
#include <http/parser.h++>
#include <io/epoll/epoll.h++>
#include <stdexcept>

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
      , router_()
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
      epoll_->set_timeout(1000);
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
          { // server error
            const char *phrase = "Server error";
            http::response res{500, phrase};
            res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
            res.with_added_header("Content-Type", "text/html;charset=utf-8");
            res.with_added_header("Content-Length", std::to_string(std::strlen(phrase)).c_str());
            auto msg = res.get_message();
            srv_->write(socket, msg, std::strlen(msg));
            epoll_->unwatch(socket);
            router_.reset();
            return;
          };

          { // serve static files
            const auto file = this->is_file_exist(request->uri.c_str());
            if (file)
            {
              auto ext = std::filesystem::path(request->uri.c_str()).extension().string().erase(0, 1);
              http::mime mime;
              auto file_mime = mime.get_ext(ext);
              http::response res{200, "OK"};
              res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
              res.with_added_header("Content-Type", file_mime.mime.append(";charset=utf-8").c_str());
              res.with_added_header("Cache-Control", "max-age=100,immutable");
              res.with_view(request->uri.c_str());
              auto msg = res.get_message();
              srv_->write(socket, msg, std::strlen(msg));
              epoll_->unwatch(socket);
              router_.reset();
              return;
            }
          }

          // handle routers
          auto router = router_.match(request->uri.c_str());
          if (!router)
          {
            http::response res{404, "Not Found"};
            res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
            res.with_added_header("Content-Type", "text/html;charset=utf-8");
            res.with_view("/404.html");
            auto msg = res.get_message();
            srv_->write(socket, msg, std::strlen(msg));
            epoll_->unwatch(socket);
            router_.reset();
            return;
          };

          const auto is_method_allowed = router_.is_method_allowed(router, request->method);
          if (!is_method_allowed)
          {
            http::response res{405, "Not Allowed"};
            res.with_added_header("Content-Type", "text/html;charset=utf-8");
            res.with_added_header("Server", ::examples::server::NAME_DEFAULT);
            res.with_view("/405.html");
            auto msg = res.get_message();
            srv_->write(socket, msg, std::strlen(msg));
            epoll_->unwatch(socket);
            router_.reset();
            return;
          }

          http::response response{200, "OK"};
          req.http_req_.params = router->params;
          router->handler(&req, &response);
          auto msg = response.get_message();
          srv_->write(socket, msg, std::strlen(msg));
          epoll_->unwatch(socket);
          router_.reset();
        });

    epoll_->on_close([](int socket) { fmt::println("client closed {}", socket); });

    server_running = true;
  }

  server::~server()
  {
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

  auto server::is_file_exist(const std::string &p) -> bool
  {
    const std::string path = static_dir_ + p;

    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
  }
} // namespace examples
