#pragma once

#include <io/epoll/epoll.h++>
#include <io/sockets/inet_socket.h++>
#include <utils/thread_pool.h++>
#include <vector>

#include "middleware_interface.h++"
#include "options_interface.h++"
#include "request.h++"
#include "response.h++"
#include "router.h++"

namespace http
{
  class server
  {
  private:
    using router = http::router;
    using request = http::request;
    using sock = ::io::inet_socket;
    using response = http::response;
    static constexpr const char *CRLF = "\r\n";

  public:
    server(options_interface *options);

    ~server();

    auto listen() -> int;

    auto add_middleware(const middleware *mdw) -> void { middlewares_.push_back(const_cast<middleware *>(mdw)); }

    auto shutdown() -> void;

    auto with_routers(const router *r) -> void { this->router_ = *r; }

    auto is_running() -> bool { return this->running_; };

  private:
    auto static signal_handler(int s) -> void { instance->running_ = false; }

  private:
    static server *instance;
    options_interface *options_;
    router router_;

    io::epoll *epoll_;
    io::inet_socket *srv_;

    utils::thread_pool thr_pool;
    std::atomic<bool> running_ = false;
    std::vector<middleware *> middlewares_;
  };
} // namespace http
