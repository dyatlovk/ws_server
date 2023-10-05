#include "TcpServer.h++"

#include <atomic>
#include <fcntl.h>
#include <fmt/core.h>
#include <stdexcept>
#include <thread>

namespace Server
{
  static std::atomic_uint isRunning = 0;

  TcpServer::TcpServer(const char *host, const int port)
      : host(host)
      , port(port)
      , serverAddr(addr)
      , io(new io::Epoll(1000))
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

    // Create IO for server socket
    io->Create();
    io->SetMasterSocket(fd);
    io->RegisterSocket(fd, IOEvents::INCOMING | IOEvents::WRITE | IOEvents::CLOSE);
    io->OnIncoming([this](int sock, bool isMaster) { IncomingHandler(sock, isMaster); });
    io->OnClose([this](int sock, bool isMaster) { CloseHandler(sock, isMaster); });

    fmt::println("[Server] {host}:{port} ready ...", fmt::arg("host", host), fmt::arg("port", port));

    isRunning = 1;
    std::thread thr(&TcpServer::WaitConnectionHandler, this);
    thr.detach();

    std::thread mainTrh(&TcpServer::MainLoopHandler, this);
    mainTrh.join();
  }

  auto TcpServer::ShutDown() -> void
  {
    isRunning = 0;
    delete io;
    fmt::println("[Server] shutdown");
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

    fmt::println("[Server] Socket listen [OK]");

    return true;
  }

  auto TcpServer::Accept() -> int
  {
    return -1;
  }


  // ---------------------------------------------------------------------------
  // PRIVATE FUNCTIONS
  // ---------------------------------------------------------------------------

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
      return -1;
    }

    SetNonBlocking(socket);

    io->RegisterSocket(socket, IOEvents::INCOMING | IOEvents::WRITE | IOEvents::CLOSE);

    getsockname(socket, (struct sockaddr *)&peer, &peer_len);

    const auto ip = inet_ntoa(peer.sin_addr);
    const auto p = (int)ntohs(peer.sin_port);

    fmt::println("[Server] new client {sock}, {host}:{port}", fmt::arg("host", ip), fmt::arg("port", p),
        fmt::arg("sock", socket));

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

    fmt::println("[Server] client {socket} send data: {data}", fmt::arg("socket", clientSocket), fmt::arg("data", buf));
  }

  auto TcpServer::WaitConnectionHandler() -> void
  {
    while (isRunning)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      io->Wait();
    }
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

  auto TcpServer::IncomingHandler(const int socket, const bool isMaster) -> void
  {
    if (isMaster)
    {
      AcceptNewConnection();
    }

    if (!isMaster)
    {
      AcceptData(socket);
    }
  }

  auto TcpServer::CloseHandler(const int socket, const bool isMaster) -> void {}
} // namespace Server
