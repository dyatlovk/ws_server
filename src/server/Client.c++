#include "Client.h++"

#include <cstring> // strlen
#include <errno.h> // errno
#include <fcntl.h>
#include <fmt/core.h>
#include <unistd.h> // socket write

namespace Server
{
  Client::Client()
      : io(nullptr)
  {
  }

  Client::Client(const int socket)
      : io(nullptr)
  {
    fd = socket;
  }

  Client::~Client()
  {
    if (io)
    {
      delete io;
    }
    CloseConnection();

    fmt::println("[Client] free {}", fd);
  }

  auto Client::SetNonBlocking() -> void
  {
    int old_flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, old_flags | O_NONBLOCK);
  }

  auto Client::SendData(const char *buf) -> int
  {
    if (fd == -1)
    {
      return -1;
    }

    int r = write(fd, buf, strlen(buf));

    if (r == -1)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {
        return -1;
      }
      CloseConnection();
    }
    return -1;
  }

  auto Client::Accept() -> int
  {
    int socket = -1;
    socklen_t peer_len = sizeof(addr);
    if ((socket = accept(fd, (struct sockaddr *)&addr, &peer_len)) == -1)
    {
      return -1;
    }

    getsockname(socket, (struct sockaddr *)&addr, &peer_len);

    const auto ip = inet_ntoa(addr.sin_addr);
    const auto p = (int)ntohs(addr.sin_port);

    if (io)
    {
      io->RegisterSocket(socket, IOEvents::INCOMING | IOEvents::WRITE | IOEvents::CLOSE);
      SetNonBlocking();
    }

    fmt::println("[Client] new client {sock}, {host}:{port}", fmt::arg("host", ip), fmt::arg("port", p),
        fmt::arg("sock", socket));

    onNewConnection(socket);

    return socket;
  }

  auto Client::Open() -> bool
  {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
      return false;
    }

    auto ip = inet_addr(host);

    auto p = (int)htons(port);

    addr.sin_family = AF_INET;
    addr.sin_port = p;
    addr.sin_addr.s_addr = ip;

    fmt::println("[Server] Socket opened [OK]");

    return true;
  }

  auto Client::Bind() -> bool
  {
    const auto b = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (b == -1)
    {
      return false;
    }

    fmt::println("[Server] Socket binded [OK]");

    return true;
  }

  auto Client::Listen(const int max) -> bool
  {
    if (listen(fd, max) == -1)
    {
      return false;
    }

    fmt::println("[Server] Socket listen [OK]");

    return true;
  }

  auto Client::WatchConnections() -> void
  {
    if (io)
    {
      io->Wait();
    }
  }

  auto Client::UseIO() -> void
  {
    io = new io::Epoll(1000);
    io->Create();
    io->SetMasterSocket(fd);
    io->RegisterSocket(fd, IOEvents::INCOMING | IOEvents::WRITE | IOEvents::CLOSE);
    io->OnIncoming(
        [this](int sock, bool isMaster)
        {
          if (isMaster)
          {
            Accept();
          }
        });
    io->OnClose([this](int sock, bool isMaster) { onCloseConnection(sock); });
    io->OnWrite([this](int sock, bool isMaster) {});
  }
} // namespace Server
