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
      , static_dir_(std::filesystem::current_path().c_str())
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
    // epoll_->on_write(
    //     [this](int sock, const char *data) { thr_pool.enqueue([this, &sock, &data]() { on_response(sock, data); });
    //     });
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

          auto router = this->match_route(request->uri.c_str());
          if (!router)
          {
            auto response = http::response::not_found();
            auto msg = response->get_message();
            srv_->write(socket, msg.c_str(), msg.size());
            epoll_->unwatch(socket);
            return;
          };

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

  auto server::read_tpl(const std::string &p) -> const std::string
  {
    std::stringstream buf;
    std::ifstream file(p, std::ios::binary);
    buf << file.rdbuf();
    file.close();
    return buf.str();
  }

  auto server::from_char(const unsigned char *data) -> std::vector<char>
  {
    std::vector<char> buf;
    int i{0};
    while (i < 656)
    {
      buf.push_back(data[i++]);
    }
    return buf;
  }

  auto server::list_served_files() -> void
  {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(static_dir_))
    {
      fmt::println("{}", entry.path().c_str());
    }
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