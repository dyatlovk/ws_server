#include "Server.h++"

#include <arpa/inet.h>
#include <memory>
#include <thread>
#include <unistd.h>

#include "fmt/core.h"

namespace Websock
{
  Server::Server(int port)
      : port(port)
      , isRunning(false)
      , sockFd(nullptr)
  {
    sockFd = new SocketFD;
    if (!SocketInit())
    {
      fmt::println("Error on open socket");
      return;
    }
  }

  Server::~Server()
  {
    this->ShutDown();
    delete sockFd;
    m_clients.clear();
  }

  auto Server::Listen() -> int
  {
    fmt::println("Server start listen {}", port);
    isRunning = true;
    std::thread thr(&Server::WaitConnectionHandler, this);
    thr.detach();
    std::thread dataThr(&Server::WaitClientMessageHandler, this);
    dataThr.join();
    fmt::println("Server exit ok");
    return 0;
  }

  auto Server::ShutDown() -> void
  {
    fmt::println("Server shut down");
    isRunning = false;
  }

  //============================================================================
  // Private functions
  //============================================================================
  auto Server::SocketOpen() -> bool
  {
    sockFd->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockFd->fd)
    {
      fmt::println("Error on socket() call");
      return false;
    }

    sockFd->addr.sin_family = AF_INET;
    sockFd->addr.sin_port = htons(port);
    sockFd->addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    return true;
  }

  auto Server::SocketAccept() -> int
  {
    const auto descriptor = GetDescriptor();
    int socket = -1;
    unsigned int sock_len = 0;
    if ((socket = accept(descriptor->fd, (struct sockaddr *)&descriptor->addr, &sock_len)) == SOCKET_ERROR)
    {
      // std::cout << "error accept socket" << std::endl;
      return SOCKET_ERROR;
    }

    return socket;
  }

  auto Server::SocketBind() -> bool
  {
    const auto b = bind(sockFd->fd, (const struct sockaddr *)&sockFd->addr, sizeof(sockFd->addr));
    if (b == -1)
    {
      fmt::println("Error on binding socket");
      return false;
    }

    return true;
  }

  auto Server::SocketListen(int max) -> bool
  {
    if (listen(sockFd->fd, max) == -1)
    {
      fmt::println("Error on listen call");
      return false;
    }

    return true;
  }

  auto Server::SocketInit() -> bool
  {
    if (!SocketOpen())
    {
      return false;
    }

    if (!SocketBind())
    {
      return false;
    }

    if (!SocketListen(MAX_CONN))
    {
      return false;
    }

    return true;
  }

  auto Server::GetDescriptor() -> SocketFD *
  {
    return sockFd;
  }

  auto Server::SocketClose() -> void
  {
    close(sockFd->fd);
    fmt::println("close socket, {}", sockFd->fd);
  }

  auto Server::SocketAcceptNonBlocking() -> int
  {
    const auto descriptor = GetDescriptor();
    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int activity;
    FD_ZERO(&readfds);
    FD_SET(descriptor->fd, &readfds);
    getsockopt(descriptor->fd, SOL_SOCKET, SO_RCVTIMEO, NULL, NULL);

    activity = select(descriptor->fd + 1, &readfds, NULL, NULL, &timeout);

    if ((activity < 0) && (errno != EINTR))
    {
      // fmt::println("select error");
    }

    if (activity > 0)
    {
      if (FD_ISSET(descriptor->fd, &readfds))
      {
        int sock = SocketAccept();
        if (sock == SOCKET_ERROR)
        {
          return -1;
        }
        return sock;
      }
    }

    return -1;
  }

  auto Server::HandleClient(const Connection &conn) -> void
  {
    fmt::println("Client connected");
  }

  auto Server::DisconnectClient(const int socket) -> void
  {
    close(socket);
    fmt::println("Client disconnected: {}", socket);
  }

  auto Server::WaitConnectionHandler() -> void
  {
    while (isRunning)
    {
      int sock = SocketAccept();
      if (sock == -1)
      {
        if (errno == EWOULDBLOCK)
        {
          fmt::println("No pending connections;");
          break;
        }
        else
        {
          // fmt::println("error when accepting connection");
          continue;
        }
      }

      if (sock > 0)
      {
        const auto ip = inet_ntoa(sockFd->addr.sin_addr);
        const auto port = (int)ntohs(sockFd->addr.sin_port);
        auto client = std::make_unique<Connection>(ip, port);
        m_clients.push_back(std::move(client));
        HandleClient(*client);
      }
    }
  }

  auto Server::WaitClientMessageHandler() -> void
  {
    while (isRunning)
    {
    }
  }
} // namespace Websock
