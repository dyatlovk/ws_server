#include "epoll.h++"

#include <arpa/inet.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <unistd.h>

namespace io
{
  epoll::epoll(const int max)
      : evlist()
      , maxEvents(max)
      , masterSocket(-1)
      , wait_events(0)
      , timeout(WAIT_TIMEOUT_DEFAULT)
      , fd(-1)
  {
    evlist.events = 0;
  }

  epoll::~epoll()
  {
    ::close(evlist.data.fd);
    close();
    fmt::println("[Epoll] [{fd}] resources free", fmt::arg("fd", fd));
  }

  auto epoll::create() -> void
  {
    fd = epoll_create(maxEvents);
    if (fd == -1)
    {
      throw std::runtime_error("[Epoll] create error");
    }

    fmt::println("[Epoll] [{fd}] create [OK]", fmt::arg("fd", fd));
  }

  auto epoll::wait() -> void
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

      if (event.events & EPOLLIN)
      {
        if (on_incoming_)
          on_incoming_(sock, sock == masterSocket);

        if (sock == masterSocket)
        {
          const auto peer = peer_accept();
          if (peer == -1)
            continue;
          register_socket(peer, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
          if (on_connection_)
            on_connection_(peer);
        }
      }

      if (event.events & EPOLLOUT)
      {
        if (sock != masterSocket)
        {
          const auto data = peer_read(sock);
          if (on_write_)
            on_write_(sock, data);
        }
      }

      if (event.events & EPOLLRDHUP)
      {
        if (sock != masterSocket)
        {
          unregister_socket(sock);
          if (on_close_)
            on_close_(sock);
        }
      }
    }
  }

  auto epoll::close() -> void
  {
    ::close(fd);
    fmt::println("[Epoll] [{fd}] close fd", fmt::arg("fd", fd));
  }

  auto epoll::register_socket(const int socket, const uint32_t e) -> void
  {
    evlist.events = e;
    evlist.data.fd = socket;

    if (epoll_ctl(fd, EPOLL_CTL_ADD, socket, &evlist) == -1)
    {
      fmt::println("[Epoll] socket error {}", socket);
      throw std::runtime_error("[Epoll] error register socket");
    }

    fmt::println("[Epoll] register new socket: {} [OK]", socket);
  }

  auto epoll::unregister_socket(const int socket) -> void
  {
    if (epoll_ctl(fd, EPOLL_CTL_DEL, socket, &evlist) == -1)
    {
      throw std::runtime_error("[Epoll] unregister socket");
    }

    fmt::println("[Epoll] [{fd}] unregister socket [OK]", fmt::arg("fd", socket));
  }

  auto epoll::peer_accept() -> int
  {
    struct sockaddr_in addr_;
    socklen_t peer_len = sizeof(addr_);
    int peer = accept(masterSocket, (struct sockaddr *)&addr_, &peer_len);
    fcntl(peer, F_SETFL, fcntl(peer, F_GETFL, 0) | O_NONBLOCK);

    return peer;
  }

  auto epoll::peer_read(const int peer, const int bufSize) -> const char *
  {
    readBuf.clear();
    char buf[bufSize];
    memset(buf, 0, sizeof(buf));
    int byte_count = 0;
    while ((byte_count = ::read(peer, buf, bufSize) > 0))
    {
      readBuf.append(buf);
    }

    return readBuf.c_str();
  }
} // namespace io
