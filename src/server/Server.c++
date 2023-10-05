#include "Server.h++"

#include <thread>

#include "fmt/core.h"

namespace Websock
{
  using namespace std::chrono_literals;

  Server::Server(int port)
      : port(port)
      , isRunning(false)
      , tcp(new Layers::Tcp("127.0.0.1", port, 10))
  {
    const auto serverSocket = tcp->GetDescriptor()->fd;
    tcp->SetServerSocket(serverSocket);
  }

  Server::~Server()
  {
    this->ShutDown();
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
    DisconnectAllClients();
    delete tcp;
  }

  //============================================================================
  // Private functions
  //============================================================================

  auto Server::HandleClient(const Connection &conn) -> void
  {
    // fmt::println("Client connected");
  }

  auto Server::DisconnectAllClients() -> void
  {
    m_clients.clear();
  }

  auto Server::WaitConnectionHandler() -> void
  {
    while (isRunning)
    {
      int sock = tcp->Accept();
      if (sock == -1)
        continue;

      // auto client = new Layers::Tcp(sock);
      // const auto ip = inet_ntoa(client->GetDescriptor()->addr.sin_addr);
      // const auto port = (int)ntohs(client->GetDescriptor()->addr.sin_port);
      // auto connection = std::make_unique<Connection>(ip, port, sock);
      // auto connHash = MakeHash(connection->GetId());
      // AddClient(connHash, std::move(connection));
      // HandleClient(*connection);
      // fmt::println("active clients: {}", m_clients.size());
    }
  }

  auto Server::WaitClientMessageHandler() -> void
  {
    while (isRunning)
    {
    }
  }

  auto Server::AddClient(std::size_t hash, std::unique_ptr<Connection> &&conn) -> void
  {
    m_clients.emplace(hash, std::move(conn));
  }

  auto Server::MakeHash(const int id) -> std::size_t
  {
    return std::hash<int>{}(id);
  }
} // namespace Websock
