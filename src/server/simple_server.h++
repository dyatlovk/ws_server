#pragma once

#include "io/epoll/epoll.h++"
#include "io/sockets/inet_socket.h++"

namespace examples
{
  class server
  {
  public:
    server();

    ~server();

    auto run() -> void;

    auto shutdown() -> void;

  private:
    io::inet_socket *srv_;
    io::epoll *epoll_;
  };
} // namespace examples
