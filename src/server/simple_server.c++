#include "simple_server.h++"

#include <atomic>
#include <csignal>
#include <fmt/core.h>
#include <thread>
#include "http/parser.h++"
#include "http/request.h++"

namespace examples
{
  std::atomic<bool> server_run = false;
  typedef ::io::inet_socket sock;

  auto signal_handler(int s) -> void
  {
    server_run = 0;
  }

  server::server()
      : srv_(sock::make_tcp("127.0.0.1", 3044))
      , epoll_(new io::epoll())
  {
    srv_->open();
    srv_->bind();
    srv_->listen();
    srv_->set_non_blocking();

    try
    {
      epoll_->create();
      epoll_->register_master(srv_->get_fd(), EPOLLIN);
    }
    catch (std::runtime_error &e)
    {
      fmt::println("{}", e.what());
      shutdown();
      return;
    }
    epoll_->on_connection([](int sock) { fmt::println("new connection {}", sock); });
    epoll_->on_write([this](int sock, const char *data) {
      auto parser = new http::parser(data);
      auto req_line = parser->get_req_line();
      delete parser;

      auto req = new http::request();
      auto request = req->parse(req_line);
      if(request)
      {
        fmt::println("method: {} uri: {}", http::request::method_string(request->method), request->uri);
      }
      delete req;
    });
    epoll_->on_close([](int socket) { fmt::println("client closed {}", socket); });

    server_run = true;
  }

  server::~server()
  {
    if (srv_) delete srv_;
    if (epoll_) delete epoll_;
    fmt::println("server is shutting down");
  }

  auto server::run() -> void
  {
    // Catch user CTRL+C
    std::signal(SIGINT, &signal_handler);

    // TODO: mem leak on SIGINT
    std::thread thr(
        [this]()
        {
          while (server_run)
          {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            try
            {
              epoll_->wait();
            }
            catch (std::runtime_error &e)
            {
              fmt::println("exception reason: {}", e.what());
            }
          }
        });
    thr.detach();

    while (server_run)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  auto server::shutdown() -> void
  {
    server_run = false;
  }
} // namespace examples
