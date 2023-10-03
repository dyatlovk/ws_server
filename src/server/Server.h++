#pragma once

#include <arpa/inet.h>
#include <memory>
#include <netdb.h>
#include <sys/un.h>
#include <vector>

#include "Connection.h++"

namespace Websock
{
  class Server
  {
  public:
    constexpr static const unsigned int MAX_CONN = 50;
    constexpr static const int SOCKET_ERROR = -1;

  private:
    struct SocketFD;
    SocketFD *sockFd;

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

    // ---------------------------------------------------------------------------
    // Create and open socket file description
    auto SocketOpen() -> bool;

    // ---------------------------------------------------------------------------
    // Bind a name to a opened socket
    auto SocketBind() -> bool;

    // ---------------------------------------------------------------------------
    // Start listen for connection
    auto SocketListen(int max) -> bool;

    // ---------------------------------------------------------------------------
    // Call all sockets routine in on function
    auto SocketInit() -> bool;

    // ---------------------------------------------------------------------------
    // Accept a connection on a socket
    // This is run in a separate thread for each socket.
    auto SocketAccept() -> int;

    // ---------------------------------------------------------------------------
    // Accept a non blocking connection on a socket
    // This is run in a separate thread for each socket.
    auto SocketAcceptNonBlocking() -> int;

    auto SocketClose() -> void;

    // ---------------------------------------------------------------------------
    // Process a connection.
    // This is run in a separate thread for each socket.
    auto ConnectionHandler(const int client) -> void;

    auto DisconnectClient(const int socket) -> void;

    auto GetDescriptor() -> SocketFD *;

    auto HandleClient(const Connection &conn) -> void;

    // ---------------------------------------------------------------------------
    // Loop for waiting incomming connections
    auto WaitConnectionHandler() -> void;

    // ---------------------------------------------------------------------------
    // Loop for waiting messages from connected clients
    auto WaitClientMessageHandler() -> void;

  private:
    struct SocketFD
    {
      int fd = 0;
      int fd_len = 0;
      struct sockaddr_in addr;
      unsigned int socket_len = 0;
    };

  private:
    int port;
    bool isRunning;

    // connected clients storage
    std::vector<std::unique_ptr<Connection>> m_clients;
  };
} // namespace Websock
