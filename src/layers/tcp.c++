#include "tcp.h++"

#include <arpa/inet.h>
#include <cstdio>
#include <fmt/core.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Layers
{
  Tcp::Tcp(const char *host, const int port, const int max)
      : port(port)
      , host(host)
      , sockFd(new SocketFD)
      , max_conn(max)
      , state(State::Close)
      , epfd(-1)
      , evlist(new epoll_event)
      , wait_events(0)
  {
    if (!this->Init())
    {
      fmt::println("Error on open socket");
      state = State::ErrInit;
      return;
    }
    CreateEpoll();
  }

  Tcp::Tcp(int socket)
      : port(0)
      , host("")
      , sockFd(new SocketFD)
      , max_conn(1)
      , state(State::Close)
      , epfd(-1)
      , evlist(new epoll_event)
      , wait_events(0)
  {
    sockFd->fd = socket;

    struct sockaddr_in serv_addr;
    socklen_t len;
    getpeername(socket, (struct sockaddr *)&serv_addr, &len);
    port = serv_addr.sin_port;
    host = inet_ntoa(serv_addr.sin_addr);

    sockFd->addr.sin_family = serv_addr.sin_family;
    sockFd->addr.sin_port = serv_addr.sin_port;
    sockFd->addr.sin_addr.s_addr = serv_addr.sin_addr.s_addr;
    state = State::Up;
  }

  Tcp::~Tcp()
  {
    Close(sockFd->fd);
    if (sockFd)
    {
      delete sockFd;
    }
    delete evlist;
    fmt::println("Connection desctruct [OK]");
  }

  auto Tcp::Init() -> bool
  {
    if (!Open())
    {
      return false;
    }

    if (!Bind())
    {
      return false;
    }

    if (!Listen(max_conn))
    {
      return false;
    }

    state = State::Up;
    fmt::println("socket ready {host}::{port}", fmt::arg("host", host), fmt::arg("port", port));

    return true;
  }

  auto Tcp::Open() -> bool
  {
    sockFd->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockFd->fd)
    {
      fmt::println("Error on socket() call");
      state = State::ErrOpen;
      Close(sockFd->fd);
      return false;
    }

    CloseImmediately(sockFd->fd);

    sockFd->addr.sin_family = AF_INET;
    sockFd->addr.sin_port = htons(port);
    sockFd->addr.sin_addr.s_addr = inet_addr(host);

    fmt::println("Socket opened [OK]");

    return true;
  }

  auto Tcp::Bind() -> bool
  {
    const auto b = bind(sockFd->fd, (const struct sockaddr *)&sockFd->addr, sizeof(sockFd->addr));
    if (b == -1)
    {
      fmt::println("Error on binding socket");
      state = State::ErrBind;
      Close(sockFd->fd);
      return false;
    }

    fmt::println("Socket binded [OK]");

    return true;
  }

  auto Tcp::Listen(int max) -> bool
  {
    if (listen(sockFd->fd, max) == -1)
    {
      fmt::println("Error on listen call");
      state = State::ErrListening;
      Close(sockFd->fd);
      return false;
    }

    fmt::println("Socket listen [OK]");

    return true;
  }

  auto Tcp::Accept() -> int
  {
    const auto descriptor = GetDescriptor();

    wait_events = epoll_wait(epfd, events, MAX_EVENTS, -1);

    int socket = -1;
    for (int i = 0; i < wait_events; ++i)
    {
      const auto event = events[i];
      auto s = event.data.fd;

      if (s == serverSocket)
      {
        fmt::println("activity {current}, master: {server}", fmt::arg("current", s), fmt::arg("server", serverSocket));
        // Accept new client connection
        unsigned int sock_len = 0;
        if ((socket = accept(descriptor->fd, (struct sockaddr *)&descriptor->addr, &sock_len)) == -1)
        {
          fmt::println("accept failed");
          return -1;
        }
        SetNonBlocking(socket);
        CloseImmediately(socket);

        // Add client socket to epoll
        evlist->events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        evlist->data.fd = socket;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, socket, evlist) == -1)
        {
          fmt::println("epoll_ctl: conn_sock");
          return -1;
        }

        // handle the client connection
        fmt::println("new client {sock}, {host}::{port}", fmt::arg("host", host), fmt::arg("port", port),
            fmt::arg("sock", socket));
        return socket;
      }
      else
      { // activity sockets
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int byte_count = read(s, buf, 1024);
        if (byte_count == 0)
        {
          close(s);
          fmt::println("socket disconnected {}", s);
        }
        return socket;
      }
    }

    return -1;
  }

  auto Tcp::IsDisconnected() -> int
  {
    const auto descriptor = GetDescriptor();

    for (int i = 0; i < wait_events; ++i)
    {
      const auto event = events[i];
      if (event.data.fd == descriptor->fd)
      {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int byte_count = read(event.data.fd, buf, 1024);
        if (byte_count == 0)
        {
          return event.data.fd;
        }
      }
    }

    return -1;
  }

  auto Tcp::Close(const int socket) -> bool
  {
    close(socket);
    close(epfd);
    delete sockFd;
    fmt::println("close socket, {host}::{port} {sock}", fmt::arg("host", host), fmt::arg("port", port),
        fmt::arg("sock", socket));
    state = State::Close;
    return true;
  }

  auto Tcp::CreateEpoll() -> bool
  {
    epfd = epoll_create(5);

    if (epfd == -1)
    {
      fmt::println("epoll_create error");
      return false;
    }

    evlist->events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
    evlist->data.fd = sockFd->fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockFd->fd, evlist) == -1)
    {
      fmt::println("epoll_ctl: listen_sock");
      return false;
    }

    fmt::println("epoll create [OK]");

    return true;
  }

  auto Tcp::SetNonBlocking(const int socket) -> void
  {
    int old_flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, old_flags | O_NONBLOCK);
  }

  auto Tcp::CloseImmediately(const int socket) -> void
  {
    linger lin;
    lin.l_onoff = 0;
    lin.l_linger = 0;
    setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char *)&lin, sizeof(int));
  }

} // namespace Layers
