#pragma once
#include <io/epoll/epoll.h++>
#include <layers/TcpAbstract.h++>
#include <memory>
#include <unordered_map>

#include "Client.h++"
#include "ServerAbstract.h++"

namespace Server
{
  class TcpServer : public ServerAbstract
  {
  private:
    constexpr static const unsigned int MAX_CONNECTIONS = 1000;
    typedef std::unique_ptr<Client> ClientT;
    typedef std::unordered_map<int, ClientT> ClientsPool;

  public:
    TcpServer(const char *host, const int port);

    auto Run() -> void override;

    auto ShutDown() -> void override;

  private:
    auto WaitConnectionHandler() -> void;

    auto MainLoopHandler() -> void;

  private:
    /**
     * Add new client to pool
     **/
    auto RegisterClient(const int socket) -> void;

    /**
     * Remove client from pool
     **/
    auto DisconnectClient(const int id) -> void;

    /**
     * Find client in pool
     **/
    auto FindClient(const int id) -> ClientT;

    auto SendData(ClientT &&client, const char *buf) -> void;

    /**
     * Create client as server
     **/
    auto CreateServer() -> void;

  private:
    int port;
    const char *host;

    ClientsPool clients;
    Client *server;
  };
} // namespace Server
