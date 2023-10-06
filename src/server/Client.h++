#pragma once

#include <layers/TcpAbstract.h++>
#include <string>

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

    /**
     * Send data to client
     **/
    auto SendData(const char *buf) -> int;

    /**
     *  Force socket fd to non blocking mode. Old flags is keeping
     **/
    auto SetNonBlocking() -> void;

    /**
     * Create a socket
     **/
    auto Open() -> bool override;

    /**
     * Bind socket to a local address
     **/
    auto Bind() -> bool override;

    /**
     * Make socket ready listening to a new connection
     **/
    auto Listen(const int max) -> bool override;

    /**
     * Accept a new connection on a socket
     **/
    auto Accept() -> int override;

    /**
     * Client have sended a data
     **/
    auto AcceptData(const int socket) -> void;

    /**
     * Read data from socket (blocking mode)
     *
     * This method get data until fd returning eof.
     * Because system read() call not guaranteed reading all data at once.
     *
     * @param int bufSize Size of chunk
     **/
    auto ReadData(const int bufSize = 1024) -> const std::string &;

    /**
     * Wait a new connections.
     * Must running in a loop
     **/
    auto WatchConnections() -> void;

    /**
     * Use IO for socket events.
     **/
    auto UseIO() -> void;

    /**
     * New accepted connection
     **/
    auto OnNewConnection(std::function<void(int)> &&callback) -> void { onNewConnection = callback; };

    /**
     * Wnen connection closed
     **/
    auto OnCloseConnection(std::function<void(int)> &&callback) -> void { onCloseConnection = callback; };

    /**
     * When client sending message
     **/
    auto OnClientMessage(std::function<void(int)> &&callback) -> void { onClientMessage = callback; };

  private:
    io::Epoll *io;

    std::function<void(int)> onNewConnection;
    std::function<void(int)> onCloseConnection;
    std::function<void(int)> onClientMessage;

    std::string readBuf; // buffer for messages
  };
} // namespace Server
