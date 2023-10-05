#include "epoll.h++"

#include <fmt/core.h>
#include <stdexcept>

namespace io
{
  Epoll::Epoll(const int max)
      : evlist(new epoll_event)
      , max(max)
  {
  }

  Epoll::~Epoll()
  {
    delete evlist;
  }

  auto Epoll::Create() -> void
  {
    fd = epoll_create(max);
    if (fd == -1)
    {
      throw std::runtime_error("[Server] epoll_create error");
    }

    fmt::println("[Server] epoll create [OK]");
  }

  auto Epoll::Wait() -> void {}

  auto Epoll::Close() -> void
  {
    close(fd);
  }

  auto Epoll::RegisterSocket(const int socket, const ::Epoll::Events &e) -> void
  {
    evlist->events = e;
    evlist->data.fd = socket;

    if (epoll_ctl(fd, EPOLL_CTL_ADD, socket, evlist) == -1)
    {
      throw std::runtime_error("[Server] epoll error register socket");
    }

    fmt::println("[Server] epoll register new socket [OK]");
  }

  auto Epoll::UnregisterSocket(const int socket) -> void
  {
    if (epoll_ctl(fd, EPOLL_CTL_DEL, socket, evlist) == -1)
    {
      fmt::println("[Server] error epoll unregister socket");
    }

    fmt::println("[Server] epoll unregister socket [OK]");
  }
} // namespace io
