#pragma once

#include <layers/TcpAbstract.h++>

#include "io/epoll/epoll.h++"

namespace Server
{
  class Client : public Layers::TcpAbstract
  {
  private:
    typedef io::Epoll::Events IOEvents;

  public:
    Client();

    Client(const int socket);

    ~Client() override;

    auto SendData(const char *buf) -> int;

    auto SetNonBlocking() -> void;

    auto Open() -> bool override;

    auto Bind() -> bool override;

    auto Listen(const int max) -> bool override;

    auto Accept() -> int override;

    auto GetSocket() -> int { return fd; };

    /**
     * Wait a new connections. For loop
     **/
    auto WatchConnections() -> void;

    /**
     * Use IO for socket watching. Needed on server client
     **/
    auto UseIO() -> void;

    /**
     * New accepted connection
     **/
    auto OnNewConnection(std::function<void(int)> &&callback) -> void { onNewConnection = callback; };

    auto OnCloseConnection(std::function<void(int)> &&callback) -> void { onCloseConnection = callback; };

  private:
    io::Epoll *io;

    std::function<void(int)> onNewConnection;
    std::function<void(int)> onCloseConnection;
  };
} // namespace Server
