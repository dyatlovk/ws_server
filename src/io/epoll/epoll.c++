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
      , timeout(WAIT_TIMEOUT_DEFAULT)
      , fd(-1)
  {
    evlist.events = 0;
  }

  epoll::~epoll()
  {
    fmt::println("[Epoll] [{fd}] destructed [OK]", fmt::arg("fd", fd));
  }

  auto epoll::create() -> void
  {
    fd = epoll_create(maxEvents);
    if (fd == -1) throw std::runtime_error("[Epoll] create error");
    fmt::println("[Epoll] [{fd}] create [OK]", fmt::arg("fd", fd));
  }

  auto epoll::wait() -> void
  {
    if (masterSocket == -1) throw std::runtime_error("[Epoll] master socket not defined.");
    int num = epoll_wait(fd, events_, maxEvents, timeout);
    if (num == -1) throw std::runtime_error("[Server] epoll wait error");

    for (int i = 0; i < num; ++i)
    {
      const auto event = events_[i];
      const int sock = event.data.fd;
      int events = events_[i].events;

      if (sock == masterSocket)
      {
        if (events & Events::READ)
        {
          if (peer_accept() == -1)
          {
            continue;
          }
        }
      }

      if (sock != masterSocket)
      {
        if (events & EPOLLET | Events::WRITE)
        {
          fmt::println("[Epoll] write #{}", sock);
          peer_read(sock);
        }

        if (events & Events::CLOSE)
        {
          try
          {
            unwatch(sock);
          }
          catch (std::runtime_error &e)
          {
            throw std::runtime_error(e);
          }
          if (on_close_) on_close_(sock);
        }
      }
    }
  }

  auto epoll::shutdown() -> void
  {
    try
    {
      unwatch_all();
    }
    catch (std::runtime_error &e)
    {
      fmt::println("[Epoll] shutdown error {}", e.what());
    }
    close(fd);
    close(masterSocket);
    fmt::println("[Epoll] shutdown [OK]", fmt::arg("fd", fd));
  }

  auto epoll::watch(const int socket, const uint32_t e) -> void
  {
    if (masterSocket == socket) return;

    if (add(socket, e) == -1) std::runtime_error("[Epoll] add error");

    watched_.insert(socket);
    fmt::println("[Epoll] register client socket: {} [OK]", socket);
    fmt::println("[Epoll] [{fd}] ({cl}) clients observing", fmt::arg("fd", fd), fmt::arg("cl", watched_size()));
  }

  auto epoll::register_master(const int socket, const uint32_t e) -> void
  {
    if (masterSocket > 0) return;
    masterSocket = socket;
    if (add(socket, e) == -1) throw std::runtime_error("[Epoll] error register master socket");
    fmt::println("[Epoll] register master socket: {} [OK]", socket);
  }

  auto epoll::unwatch(const int socket) -> void
  {
    if (socket == masterSocket) return;
    if (remove(socket) == -1) throw std::runtime_error("[Epoll] unregister socket");

    watched_.erase(socket);
    close(socket);

    fmt::println("[Epoll] [{fd}] unregister socket [OK]", fmt::arg("fd", socket));
  }

  auto epoll::unwatch_all() -> void
  {
    for (const auto &client : watched_)
    {
      try
      {
        unwatch(client);
      }
      catch (std::runtime_error &e)
      {
        fmt::println("runtime error unwatch_all");
        throw std::runtime_error(e);
      }
    }

    fmt::println("[Epoll] [{fd}] ({cl}) clients observing", fmt::arg("fd", fd), fmt::arg("cl", watched_size()));
  }

  auto epoll::peer_accept() -> int
  {
    struct sockaddr_in addr_;
    socklen_t peer_len = sizeof(addr_);
    int peer = accept(masterSocket, (struct sockaddr *)&addr_, &peer_len);
    fcntl(peer, F_SETFL, fcntl(peer, F_GETFL, 0) | O_NONBLOCK);

    watch(peer);
    if (on_connection_) on_connection_(peer);

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

    if (on_write_) on_write_(peer, readBuf.c_str());

    return readBuf.c_str();
  }

  auto epoll::add(const int socket, const uint32_t e) -> int
  {
    evlist.events = e;
    evlist.data.fd = socket;
    return epoll_ctl(fd, EPOLL_CTL_ADD, socket, &evlist);
  }

  auto epoll::remove(const int socket) -> int
  {
    return epoll_ctl(fd, EPOLL_CTL_DEL, socket, &evlist);
  }
} // namespace io
