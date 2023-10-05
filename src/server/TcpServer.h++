#pragma once
#include <layers/TcpAbstract.h++>
#include <csignal>
#include <sys/epoll.h>

#include "ServerAbstract.h++"

namespace Server
{
  class TcpServer : public Layers::TcpAbstract, public ServerAbstract
  {
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
    auto CreateEpoll() -> bool;

    auto CloseConnection() -> void override;

    auto CloseAllClients() -> void;

    auto SetNonBlocking(const int sock) -> void;

    auto AcceptNewConnection() -> int;

    auto AcceptData(const int clientSocket = -1) -> void;

    auto WaitConnectionHandler() -> void;

    auto MainLoopHandler() -> void;

  private:
    int port;
    const char *host;
    int serverSocket;
    struct sockaddr_in serverAddr;

    int epfd; // epoll instance
    struct epoll_event *evlist, events[MAX_EVENTS];
    int wait_events;
  };
} // namespace Server
