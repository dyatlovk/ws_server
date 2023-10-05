#include "TcpServer.h++"

#include <atomic>
#include <cstdlib>
#include <fcntl.h>
#include <fmt/core.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "io/epoll/events.h++"
#include "server/Client.h++"

namespace Server
{
  static std::atomic_uint isRunning = 0;

  TcpServer::TcpServer(const char *host, const int port)
      : host(host)
      , port(port)
      , evlist(new epoll_event)
      , wait_events(0)
      , epfd(-1)
      , serverAddr(addr)
  {
  }

  auto TcpServer::Run() -> void
  {
    if (port == 0 || !host)
    {
      throw std::runtime_error("[Server] host and port required");
    }

    if (!Open())
    {
      throw std::runtime_error("[Server] Error open socket");
    }

    if (!Bind())
    {
      throw std::runtime_error("[Server] Error bind socket");
    }

    if (!Listen(MAX_CONN))
    {
      throw std::runtime_error("[Server] Error listen socket");
    }

    SetNonBlocking(fd);

    if (acceptType == AcceptType::Epoll)
    {
      CreateEpoll();
    }

    fmt::println("[Server] {host}:{port} ready ...", fmt::arg("host", host), fmt::arg("port", port));

    isRunning = 1;
    std::thread thr(&TcpServer::WaitConnectionHandler, this);
    thr.detach();

    // std::thread mainTrh(&TcpServer::MainLoopHandler, this);
    // mainTrh.join();
    MainLoopHandler();
  }

  auto TcpServer::ShutDown() -> void
  {
    isRunning = 0;
    CloseAllClients();
    delete evlist;
    fmt::println("Server shutdown");
  }

  auto TcpServer::Open() -> bool
  {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
      return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host);

    fmt::println("[Server] Socket opened [OK]");

    return true;
  }

  auto TcpServer::Bind() -> bool
  {
    const auto b = bind(fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (b == -1)
    {
      return false;
    }

    fmt::println("[Server] Socket binded [OK]");

    return true;
  }

  auto TcpServer::Listen(const int max) -> bool
  {
    if (listen(fd, max) == -1)
    {
      return false;
    }

    return true;
  }

  auto TcpServer::Accept() -> int
  {
    wait_events = epoll_wait(epfd, events, MAX_EVENTS, -1);

    int socket = -1;
    for (int i = 0; i < wait_events; ++i)
    {
      const auto event = events[i];

      // Server socket
      if (event.data.fd == fd)
      {
        // Coming connections
        if (event.events & Epoll::Events::INCOMING)
        {
          return AcceptNewConnection();
        }

        if (event.events & Epoll::Events::WRITE)
        {
          fmt::println("server socket writing");
        }
      }

      // Client socket
      if (event.data.fd != fd)
      {
        const auto clientSock = event.data.fd;

        if (event.events & Epoll::Events::WRITE)
        {
          fmt::println("client socket writing");
        }

        if (event.events & Epoll::Events::INCOMING)
        {
          AcceptData(clientSock);
        }

        if (event.events & Epoll::Events::CLOSE)
        {
          fmt::println("close connection {}", clientSock);
          close(clientSock);
          return -1;
        }
      }
    }
    return -1;
  }


  // ---------------------------------------------------------------------------
  // PRIVATE FUNCTIONS
  // ---------------------------------------------------------------------------

  auto TcpServer::CreateEpoll() -> bool
  {
    epfd = epoll_create(5);
    if (epfd == -1)
    {
      CloseConnection();
      throw std::runtime_error("[Server] epoll_create error");
    }

    evlist->events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
    evlist->data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, evlist) == -1)
    {
      CloseConnection();
      throw std::runtime_error("[Server] epoll_ctl: listen_sock");
    }

    fmt::println("[Server] epoll create [OK]");

    return true;
  }

  auto TcpServer::CloseConnection() -> void
  {
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, evlist) == -1)
    {
      fmt::println("error unregister server socket epoll");
    }
    close(epfd);
    close(fd);
  }

  auto TcpServer::CloseAllClients() -> void
  {
    for (int i = 0; i < wait_events; ++i)
    {
      const auto event = events[i];
      if (event.data.fd != fd)
      {
        const auto clientSock = event.data.fd;
        close(clientSock);
      }
    }
    fmt::println("disconnected all clients");
  }

  auto TcpServer::SetNonBlocking(const int sock) -> void
  {
    int old_flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, old_flags | O_NONBLOCK);
  }

  auto TcpServer::AcceptNewConnection() -> int
  {
    int socket = -1;
    struct sockaddr_in peer;
    socklen_t peer_len = sizeof(peer);

    if ((socket = accept(fd, (struct sockaddr *)&peer, &peer_len)) == -1)
    {
      fmt::println("accept failed");
      return -1;
    }

    SetNonBlocking(socket);

    // Add client socket to epoll
    evlist->events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    evlist->data.fd = socket;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, socket, evlist) == -1)
    {
      fmt::println("epoll_ctl: conn_sock");
      return -1;
    }

    getsockname(socket, (struct sockaddr *)&peer, &peer_len);

    const auto ip = inet_ntoa(peer.sin_addr);
    const auto p = (int)ntohs(peer.sin_port);

    fmt::println(
        "new client {sock}, {host}::{port}", fmt::arg("host", ip), fmt::arg("port", p), fmt::arg("sock", socket));

    return socket;
  }

  auto TcpServer::AcceptData(const int clientSocket) -> void
  {
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int byte_count = read(clientSocket, buf, 1024);
    if (byte_count == 0)
    {
      // close(clientSocket);
      // fmt::println("socket disconnected {}", clientSocket);
      return;
    }

    fmt::println("client {socket} send data", fmt::arg("socket", clientSocket));
  }

  auto TcpServer::WaitConnectionHandler() -> void
  {
    while (isRunning)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      int client = Accept();
      if (client == -1)
      {
        continue;
      }
    }
    CloseAllClients();
    CloseConnection();
  }

  auto TcpServer::MainLoopHandler() -> void
  {
    // Catch user CTRL+C
    std::signal(SIGINT, [](int signal) { isRunning = 0; });

    while (isRunning)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
} // namespace Server
