#include "tcp.h++"

#include <fmt/core.h>
#include <unistd.h>

namespace Layers
{
  Tcp::Tcp(const char *host, const int port, const int max)
      : port(port)
      , host(host)
      , sockFd(new SocketFD)
      , max_conn(max)
      , state(State::Close)
  {
    if (!this->Init())
    {
      fmt::println("Error on open socket");
      state = State::ErrInit;
      return;
    }
  }

  Tcp::~Tcp()
  {
    delete sockFd;
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
      return false;
    }

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
      return false;
    }

    fmt::println("Socket listen [OK]");

    return true;
  }

  auto Tcp::Accept() -> int
  {
    const auto descriptor = GetDescriptor();

    int socket = -1;
    unsigned int sock_len = 0;
    if ((socket = accept(descriptor->fd, (struct sockaddr *)&descriptor->addr, &sock_len)) == -1)
    {
      return -1;
    }

    return socket;
  }

  auto Tcp::Close() -> bool
  {
    close(sockFd->fd);
    fmt::println("close socket, {host}::{port}", fmt::arg("host", host), fmt::arg("port", port));
    state = State::Close;
    return true;
  }


} // namespace Layers
