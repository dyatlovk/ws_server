#pragma once

#include <http/request.h++>
#include <http/response.h++>
#include <http/router.h++>
#include <io/epoll/epoll.h++>
#include <io/sockets/inet_socket.h++>
#include <utils/thread_pool.h++>

namespace examples
{
  class server
  {
    static constexpr int PORT_DEFAULT = 3044;
    static constexpr const char *CRLF = "\r\n";
    static constexpr const char *HOST_DEFAULT = "127.0.0.1";

    using req = http::request;
    using router = http::router;

  public:
    constexpr static const char *NAME_DEFAULT = "BlackMesa";

  public:
    server(const char *host = HOST_DEFAULT, int port = PORT_DEFAULT);

    ~server();

    auto run() -> void;

    auto serve_static(const char *dir) -> void { this->static_dir_ = dir; }

    auto get_static_dir() -> const std::string { return this->static_dir_; }

    auto shutdown() -> void;

    auto is_file_exist(const std::string &p) -> bool;

    auto with_routers(const router *r) -> void { this->router_ = *r; }

  private:
    int port_;
    const char *host_;

    io::epoll *epoll_;
    io::inet_socket *srv_;

    std::string static_dir_;
    utils::thread_pool thr_pool;

  private:
    router router_;
  };
} // namespace examples
