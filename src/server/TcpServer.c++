#include "TcpServer.h++"

#include <atomic>
#include <csignal>
#include <fmt/core.h>
#include <thread>

namespace Server
{
  // Server is running until this flag is 1
  static std::atomic_uint isRunning = 0;

  TcpServer::TcpServer(const char *host, const int port)
      : host(host)
      , port(port)
  {
  }

  auto TcpServer::Run() -> void
  {
    if (port == 0 || !host)
    {
      throw std::runtime_error("[Server] host and port required");
    }

    CreateServer();
    server->OnNewConnection([this](int socket) -> void { RegisterClient(socket); });
    server->OnCloseConnection([this](int socket) -> void { DisconnectClient(socket); });
    server->OnClientMessage([this](int socket) -> void { ReadClientMessage(socket); });

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
    clients.clear();
    delete server;
    fmt::println("[Server] shutdown");
  }

  // ---------------------------------------------------------------------------
  // PRIVATE FUNCTIONS
  // ---------------------------------------------------------------------------

  auto TcpServer::WaitConnectionHandler() -> void
  {
    while (isRunning)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      server->WatchConnections();
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

  auto TcpServer::CreateServer() -> void
  {
    server = new Client();
    server->SetHost(host);
    server->SetPort(port);
    if (!server->Open())
    {
      throw std::runtime_error("[Server] Error open socket");
    }

    if (!server->Bind())
    {
      throw std::runtime_error("[Server] Error bind socket");
    }

    if (!server->Listen(MAX_CONNECTIONS))
    {
      throw std::runtime_error("[Server] Error listen socket");
    }

    server->SetNonBlocking();
    server->UseIO();
  }

  auto TcpServer::RegisterClient(const int socket) -> void
  {
    auto client = std::make_shared<Client>(socket);
    client->SetNonBlocking();
    clients.emplace(socket, std::move(client));
  }

  auto TcpServer::DisconnectClient(const int id) -> void
  {
    auto client = FindClient(id);
    if (!client)
      return;

    clients.erase(id);
    client->CloseConnection();

    fmt::println("[Server] disconnect client {}", id);
  }

  auto TcpServer::ReadClientMessage(const int id) -> void
  {
    auto client = FindClient(id);
    if (!client)
      return;

    auto msg = client->ReadData();
    fmt::println("msg: {}", msg);
  }

  auto TcpServer::FindClient(const int id) -> ClientT
  {
    if (clients.size() == 0)
      return nullptr;
    try
    {
      return clients.at(id);
    }
    catch (std::out_of_range &e)
    {
      return nullptr;
    }

    return nullptr;
  }
} // namespace Server
