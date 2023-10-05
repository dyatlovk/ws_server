#include "epoll.h++"

#include <fmt/core.h>
#include <stdexcept>

namespace io
{
  Epoll::Epoll(const int max)
      : evlist(new epoll_event)
      , maxEvents(max)
      , masterSocket(-1)
      , wait_events(0)
      , timeout(WAIT_TIMEOUT_DEFAULT)
      , fd(-1)
  {
  }

  Epoll::~Epoll()
  {
    Close(fd);
    delete evlist;
    fmt::println("[Epoll] resources free");
  }

  auto Epoll::Create() -> void
  {
    fd = epoll_create(maxEvents);
    if (fd == -1)
    {
      throw std::runtime_error("[Epoll] create error");
    }

    fmt::println("[Epoll] create {} [OK]", fd);
  }

  auto Epoll::Wait() -> void
  {
    if (masterSocket == -1)
    {
      throw std::runtime_error("[Epoll] master socket not defined.");
    }

    wait_events = epoll_wait(fd, events, maxEvents, timeout);

    if (wait_events == -1)
    {
      throw std::runtime_error("[Server] epoll wait error");
    }

    for (int i = 0; i < wait_events; ++i)
    {
      const auto event = events[i];
      const int sock = event.data.fd;

      if (event.events & Events::INCOMING)
      {
        if (onIncoming)
          onIncoming(sock, event.data.fd == masterSocket);
      }

      if (event.events & Events::WRITE)
      {
        if (onWrite)
          onWrite(sock, event.data.fd == masterSocket);
      }

      if (event.events & Events::CLOSE)
      {
        UnregisterSocket(sock);
        Close(sock);
        if (onClose)
        {
          onClose(sock, event.data.fd == masterSocket);
        }
      }
    }
  }

  auto Epoll::Close(const int sock) -> void
  {
    close(sock);
    fmt::println("[Epoll] close fd {}", sock);
  }

  auto Epoll::RegisterSocket(const int socket, const uint32_t e) -> void
  {
    evlist->events = e;
    evlist->data.fd = socket;

    if (epoll_ctl(fd, EPOLL_CTL_ADD, socket, evlist) == -1)
    {
      fmt::println("[Epoll] socket error {}", socket);
      throw std::runtime_error("[Epoll] error register socket");
    }

    fmt::println("[Epoll] epoll register new socket: {} [OK]", socket);
  }

  auto Epoll::UnregisterSocket(const int socket) -> void
  {
    if (epoll_ctl(fd, EPOLL_CTL_DEL, socket, evlist) == -1)
    {
      throw std::runtime_error("[Epoll] unregister socket");
    }

    fmt::println("[Epoll] unregister socket {} [OK]", socket);
  }
} // namespace io
