#pragma once
#include <csignal>
#include <layers/TcpAbstract.h++>

#include "ServerAbstract.h++"
#include "io/epoll/epoll.h++"

namespace Server
{
  class TcpServer : public Layers::TcpAbstract, public ServerAbstract
  {
    typedef io::Epoll::Events IOEvents;

  private:
    constexpr static const unsigned int MAX_EVENTS = 1000;

  public:
    TcpServer(const char *host, const int port);

    auto Run() -> void override;

    auto ShutDown() -> void override;

  protected:
    auto Open() -> bool override;

    auto Bind() -> bool override;

    auto Listen(const int max) -> bool override;

    auto Accept() -> int override;

  private:
    auto SetNonBlocking(const int sock) -> void;

    auto AcceptNewConnection() -> int;

    auto AcceptData(const int clientSocket = -1) -> void;

    auto WaitConnectionHandler() -> void;

    auto MainLoopHandler() -> void;

    auto IncomingHandler(const int socket, const bool isMaster) -> void;

    auto CloseHandler(const int socket, const bool isMaster) -> void;

  private:
    int port;
    const char *host;
    struct sockaddr_in serverAddr;

    io::Epoll *io;
  };
} // namespace Server
