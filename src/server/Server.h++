#pragma once

#include <layers/tcp.h++>
#include <map>
#include <memory>

#include "Connection.h++"

namespace Websock
{
  class Server
  {
  public:
    constexpr static const unsigned int MAX_CONN = 50;

  public:
    Server(int port);

    ~Server();

    auto Listen() -> int;

    auto Connect() -> void;

    auto Disconnect() -> void;

    auto ShutDown() -> void;

  protected:
    auto onOpen(Connection *conn) -> void;
    auto onClose(Connection *conn) -> void;
    auto onMessage(Connection *conn) -> void;

    auto DisconnectAllClients() -> void;

    auto HandleClient(const Connection &conn) -> void;

    // ---------------------------------------------------------------------------
    // Loop for waiting incomming connections
    auto WaitConnectionHandler() -> void;

    // ---------------------------------------------------------------------------
    // Loop for waiting messages from connected clients
    auto WaitClientMessageHandler() -> void;

    auto AddClient(std::size_t hash, std::unique_ptr<Connection> &&conn) -> void;

    static auto MakeHash(const int id) -> std::size_t;

  private:
    int port;
    bool isRunning;

    Layers::Tcp *tcp;

    // connected clients storage
    std::map<std::size_t, std::unique_ptr<Connection>> m_clients;
  };
} // namespace Websock
